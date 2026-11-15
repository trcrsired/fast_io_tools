use std::hint::black_box;
use std::time::Instant;

#[derive(Clone, Copy)]
#[repr(C)]
struct Rec16 {
    key: u64,
    payload: u64,
}

#[derive(Clone, Copy)]
#[repr(C)]
struct Big {
    key: u64,
    payload: [u64; 127],
}

fn shift_name(index: usize) -> &'static str {
    match index {
        0 => "1",
        1 => "n/8",
        2 => "n/4",
        3 => "n/2",
        4 => "7n/8",
        5 => "n-1",
        _ => "?",
    }
}

fn shift_of(index: usize, n: usize) -> usize {
    match index {
        0 => 1,
        1 => n / 8,
        2 => n / 4,
        3 => n / 2,
        4 => n - 1 - n / 8,
        5 => n - 1,
        _ => 0,
    }
}

fn checksum_u64(values: &[u64]) -> u64 {
    let mut sum = 0u64;
    for value in values {
        sum = sum.wrapping_mul(31).wrapping_add(*value);
    }
    sum
}

fn checksum_rec(values: &[Rec16]) -> u64 {
    let mut sum = 0u64;
    for value in values {
        sum = sum.wrapping_mul(31).wrapping_add(value.key);
    }
    sum
}

fn checksum_big(values: &[Big]) -> u64 {
    let mut sum = 0u64;
    for value in values {
        sum = sum.wrapping_mul(31).wrapping_add(value.key);
    }
    sum
}

fn bench<T: Copy, F, C>(
    original: &[T],
    shift: usize,
    reps: usize,
    mut rotate: F,
    checksum: C,
) -> f64
where
    F: FnMut(&mut [T], usize),
    C: Fn(&[T]) -> u64,
{
    let mut work = original.to_vec();
    let mut best = f64::INFINITY;
    let mut sink = 0u64;
    for _ in 0..reps {
        work.copy_from_slice(original);
        let start = Instant::now();
        rotate(&mut work, shift);
        let elapsed = start.elapsed().as_nanos() as f64;
        if elapsed < best {
            best = elapsed;
        }
        sink ^= checksum(&work);
    }
    black_box(sink);
    best / original.len() as f64
}

fn run_u64(sizes: &[usize]) {
    for &n in sizes {
        let original: Vec<u64> = (0..n).map(|i| i as u64).collect();
        let reps = if n >= 1_000_000 { 20 } else { 100 };
        for s in 0..6 {
            let shift = shift_of(s, n);
            let ns = bench(&original, shift, reps, |v, k| v.rotate_left(k), checksum_u64);
            println!("u64,{},{},{:.4}", shift_name(s), n, ns);
        }
    }
}

fn run_rec16(sizes: &[usize]) {
    for &n in sizes {
        let original: Vec<Rec16> = (0..n)
            .map(|i| Rec16 {
                key: i as u64,
                payload: i as u64,
            })
            .collect();
        let reps = if n >= 1_000_000 { 20 } else { 100 };
        for s in 0..6 {
            let shift = shift_of(s, n);
            let ns = bench(&original, shift, reps, |v, k| v.rotate_left(k), checksum_rec);
            println!("rec16,{},{},{:.4}", shift_name(s), n, ns);
        }
    }
}

fn run_big(sizes: &[usize]) {
    for &n in sizes {
        let original: Vec<Big> = (0..n)
            .map(|i| Big {
                key: i as u64,
                payload: [i as u64; 127],
            })
            .collect();
        let reps = if n >= 100_000 { 8 } else { 20 };
        for s in 0..6 {
            let shift = shift_of(s, n);
            let ns = bench(&original, shift, reps, |v, k| v.rotate_left(k), checksum_big);
            println!("big1k,{},{},{:.4}", shift_name(s), n, ns);
        }
    }
}

fn main() {
    let quick = std::env::args().nth(1).as_deref() == Some("--quick");
    if quick {
        let sizes = [1000, 10000];
        run_u64(&sizes);
        run_rec16(&sizes);
        run_big(&[1000]);
    } else {
        let sizes = [1000, 10000, 100000, 1000000];
        run_u64(&sizes);
        run_rec16(&sizes);
        run_big(&[1000, 10000, 100000]);
    }
}
