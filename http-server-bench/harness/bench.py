#!/usr/bin/env python3
"""Start one HTTP server variant, drive it with loadgen, record throughput,
latency, CPU and memory. Results go to stdout as JSON."""
import json
import os
import re
import signal
import subprocess
import sys
import time

ROOT = "/tmp/bench/root"
LOADGEN = "/tmp/bench/loadgen"
LOGDIR = "/tmp/bench/logs"
CLK = os.sysconf("SC_CLK_TCK")
PAGE = os.sysconf("SC_PAGE_SIZE")


def listening_pid(port):
    out = subprocess.run(
        ["ss", "-tlnpH", f"sport = :{port}"], capture_output=True, text=True
    ).stdout
    m = re.search(r"pid=(\d+)", out)
    return int(m.group(1)) if m else None


def wait_listen(port, timeout):
    t0 = time.time()
    while time.time() - t0 < timeout:
        if listening_pid(port):
            return
        time.sleep(0.2)
    raise RuntimeError(f"port {port} never listened")


def proc_cpu_ticks(pid):
    with open(f"/proc/{pid}/stat") as f:
        s = f.read()
    rest = s[s.rfind(")") + 2 :].split()
    return int(rest[11]) + int(rest[12])


def proc_rss(pid):
    try:
        with open(f"/proc/{pid}/status") as f:
            for line in f:
                if line.startswith("VmRSS:"):
                    return int(line.split()[1]) * 1024
    except FileNotFoundError:
        pass
    return 0


def proc_hwm(pid):
    try:
        with open(f"/proc/{pid}/status") as f:
            for line in f:
                if line.startswith("VmHWM:"):
                    return int(line.split()[1]) * 1024
    except FileNotFoundError:
        pass
    return 0


def load(port, path, conns, secs):
    out = subprocess.run(
        [LOADGEN, str(port), path, str(conns), str(secs)],
        capture_output=True,
        text=True,
        timeout=secs + 120,
    )
    if out.returncode != 0:
        raise RuntimeError(f"loadgen failed: {out.stderr}")
    stats = {}
    for token in out.stdout.split():
        k, _, v = token.partition("=")
        try:
            stats[k] = float(v)
        except ValueError:
            stats[k] = v
    return stats


def measure(pid, port, path, conns, secs):
    c0 = proc_cpu_ticks(pid)
    r0 = proc_rss(pid)
    st = load(port, path, conns, secs)
    c1 = proc_cpu_ticks(pid)
    wall = st["duration_s"]
    st["server_cpu_pct"] = (c1 - c0) / CLK / wall * 100.0
    st["server_rss_before"] = r0
    st["server_rss_after"] = proc_rss(pid)
    return st


def main():
    variant = sys.argv[1]
    port = int(sys.argv[2])
    os.makedirs(LOGDIR, exist_ok=True)
    log = open(f"{LOGDIR}/{variant}.log", "w")

    cmds = {
        "linux": ["/tmp/bench/http-server-linux-uring", "-p", str(port), ROOT],
        "linux-tuned": ["/tmp/bench/http-server-linux-uring", "-p", str(port), ROOT],
        "wasm-mtg": [
            "wavm",
            "run",
            "--enable",
            "threads",
            "--enable",
            "networks",
            "--enable",
            "memtag-soft",
            "--mount-root",
            ROOT,
            "/tmp/bench/http-server-mtg-lto-maxmem.wasm",
            "-p",
            str(port),
            ".",
        ],
        "wasm-nomtg": [
            "wavm",
            "run",
            "--enable",
            "threads",
            "--enable",
            "networks",
            "--mount-root",
            ROOT,
            "/tmp/bench/http-server-nomtg-maxmem.wasm",
            "-p",
            str(port),
            ".",
        ],
        "wine": ["wine", "/tmp/bench/http-server-win.exe", "-p", str(port), ROOT],
        "httplib": ["/tmp/bench/others/httplib_server", str(port), ROOT],
        "mongoose": ["/tmp/bench/others/mongoose_server", str(port), ROOT],
        "python": ["python3", "/tmp/bench/others/py_http.py", str(port), ROOT],
        "rust": [
            "/tmp/bench/others/simple-http-server/target/release/simple-http-server",
            "--ip",
            "127.0.0.1",
            "-p",
            str(port),
            "--nocache",
            "-s",
            ROOT,
        ],
        "sws": ["/tmp/bench/others/sws_server", str(port), ROOT],
        "go": ["/tmp/bench/others/go_fileserver", str(port), ROOT],
        "node": [
            "node",
            "/home/cqwrteur/softwares/httpserver/node_modules/http-server/bin/http-server",
            "-p",
            str(port),
            "-a",
            "127.0.0.1",
            "-c-1",
            "-s",
            ROOT,
        ],
    }
    cmd = cmds[variant]
    env = dict(os.environ)
    env.setdefault("WINEDEBUG", "-all")
    if variant == "linux":
        env["LD_LIBRARY_PATH"] = (
            "/home/cqwrteur/toolchains/gnu/x86_64-linux-gnu/x86_64-linux-gnu/lib64:"
            + env.get("LD_LIBRARY_PATH", "")
        )
    if variant == "linux-tuned":
        env["LD_LIBRARY_PATH"] = (
            "/home/cqwrteur/toolchains/gnu/x86_64-linux-gnu/x86_64-linux-gnu/lib64:"
            + env.get("LD_LIBRARY_PATH", "")
        )
        # stop glibc from returning the per-request arena pages to the kernel
        env["MALLOC_TRIM_THRESHOLD_"] = "268435456"
        env["MALLOC_MMAP_THRESHOLD_"] = "268435456"
        env["MALLOC_TOP_PAD_"] = "67108864"
    proc = subprocess.Popen(
        cmd,
        cwd=ROOT if variant in ("linux", "linux-tuned", "wine", "node") else "/tmp/bench",
        stdout=log,
        stderr=subprocess.STDOUT,
        env=env,
        start_new_session=True,
    )
    result = {"variant": variant, "cmd": cmd, "port": port}
    try:
        wait_listen(port, 120 if variant.startswith("wasm") else 60)
        time.sleep(1.0)
        pid = listening_pid(port)
        if pid is None:
            raise RuntimeError("no listening pid")
        # the wasm/wine host process is the same pid here
        result["pid"] = pid
        result["idle_rss"] = proc_rss(pid)
        # warmup
        load(port, "/index.html", 64, 4)
        load(port, "/output.mp4", 16, 4)
        # measured runs
        result["small"] = [
            measure(pid, port, "/index.html", 64, 15) for _ in range(2)
        ]
        result["mid"] = [measure(pid, port, "/mid.bin", 32, 15) for _ in range(2)]
        result["large"] = [
            measure(pid, port, "/output.mp4", 16, 15) for _ in range(2)
        ]
        result["peak_rss"] = proc_hwm(pid)
        result["final_rss"] = proc_rss(pid)
    finally:
        try:
            os.killpg(os.getpgid(proc.pid), signal.SIGTERM)
        except ProcessLookupError:
            pass
        try:
            proc.wait(timeout=10)
        except subprocess.TimeoutExpired:
            try:
                os.killpg(os.getpgid(proc.pid), signal.SIGKILL)
            except ProcessLookupError:
                pass
        log.close()
    print(json.dumps(result))
    with open(f"/tmp/bench/{variant}.json", "w") as f:
        json.dump(result, f, indent=1)


if __name__ == "__main__":
    main()
