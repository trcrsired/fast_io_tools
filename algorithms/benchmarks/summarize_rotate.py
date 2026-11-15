#!/usr/bin/env python3
"""Merge the C++ and Rust rotate benchmark CSVs into one comparison table."""

import csv
import sys


def load_cpp(path):
    rows = {}
    with open(path, newline="") as f:
        for row in csv.reader(f):
            if len(row) != 5:
                continue
            type_name, shift, n, std_ns, fast_ns = row
            rows[(type_name, shift, int(n))] = (float(std_ns), float(fast_ns))
    return rows


def load_rust(path):
    rows = {}
    with open(path, newline="") as f:
        for row in csv.reader(f):
            if len(row) != 4:
                continue
            type_name, shift, n, rust_ns = row
            rows[(type_name, shift, int(n))] = float(rust_ns)
    return rows


def main():
    if len(sys.argv) != 3:
        print(f"usage: {sys.argv[0]} cpp.csv rust.csv", file=sys.stderr)
        return 2
    cpp = load_cpp(sys.argv[1])
    rust = load_rust(sys.argv[2])
    keys = sorted(cpp.keys() & rust.keys(), key=lambda k: (k[0], k[2], k[1]))

    header = (
        f"{'type':8} {'shift':11} {'n':>8} "
        f"{'libc++ std':>11} {'fast_io':>10} {'rust std':>10} "
        f"{'fast/std':>9} {'fast/rust':>10}"
    )
    print(header)
    print("-" * len(header))
    for key in keys:
        type_name, shift, n = key
        std_cpp, fast_io = cpp[key]
        std_rust = rust[key]
        print(
            f"{type_name:8} {shift:11} {n:8d} "
            f"{std_cpp:11.4f} {fast_io:10.4f} {std_rust:10.4f} "
            f"{std_cpp / fast_io:9.3f} {std_rust / fast_io:10.3f}"
        )


if __name__ == "__main__":
    sys.exit(main())
