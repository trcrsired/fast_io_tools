#!/usr/bin/env python3
"""Merge the C++ and Rust sort benchmark CSVs into one comparison table."""

import csv
import sys


def load(path):
    rows = {}
    with open(path, newline="") as f:
        for row in csv.reader(f):
            if len(row) != 7:
                continue
            type_name, pattern, _, n, std_ns, fast_ns, _ = row
            rows[(type_name, pattern, int(n))] = (float(std_ns), float(fast_ns))
    return rows


def main():
    if len(sys.argv) != 3:
        print(f"usage: {sys.argv[0]} cpp.csv rust.csv", file=sys.stderr)
        return 2
    cpp = load(sys.argv[1])
    rust = load(sys.argv[2])
    keys = sorted(cpp.keys() & rust.keys(), key=lambda k: (k[0], k[1], k[2]))

    header = (
        f"{'type':8} {'pattern':11} {'n':>8} "
        f"{'libc++ std':>11} {'fast_io':>10} {'ipnsort':>10} {'rust std':>10} "
        f"{'fast/ipn':>9} {'fast/libstd':>12}"
    )
    print(header)
    print("-" * len(header))
    for key in keys:
        type_name, pattern, n = key
        std_cpp, fast_io = cpp[key]
        std_rust, ipnsort = rust[key]
        print(
            f"{type_name:8} {pattern:11} {n:8d} "
            f"{std_cpp:11.2f} {fast_io:10.2f} {ipnsort:10.2f} {std_rust:10.2f} "
            f"{fast_io / ipnsort:9.3f} {std_cpp / fast_io:12.3f}"
        )
    print()
    print("ns/element, lower is better. fast/ipn is C++ fast_io over Rust ipnsort.")
    print("fast/libstd is libc++ std::sort over C++ fast_io.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
