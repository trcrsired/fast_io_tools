use std::hint::black_box;
use std::time::Instant;

#[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
#[repr(C)]
struct Rec16 {
    key: u64,
    payload: u64,
}

#[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
#[repr(C)]
struct Big {
    key: u64,
    payload: [u64; 127],
}

#[derive(Clone, Copy)]
enum Pattern {
    Ascending,
    Descending,
    Random,
    D20,
    P5,
    S95,
}

impl Pattern {
    fn name(self) -> &'static str {
        match self {
            Pattern::Ascending => "ascending",
            Pattern::Descending => "descending",
            Pattern::Random => "random",
            Pattern::D20 => "d20",
            Pattern::P5 => "p5",
            Pattern::S95 => "s95",
        }
    }
}

struct XorShift {
    state: u64,
}

impl XorShift {
    fn new(seed: u64) -> Self {
        Self {
            state: if seed == 0 { 1 } else { seed },
        }
    }

    fn next(&mut self) -> u64 {
        self.state ^= self.state << 13;
        self.state ^= self.state >> 7;
        self.state ^= self.state << 17;
        self.state
    }
}

fn make_keys(n: usize, pattern: Pattern) -> Vec<u64> {
    let mut rng = XorShift::new(0x9e37_79b9_7f4a_7c15);
    let mut keys = Vec::with_capacity(n);
    for i in 0..n {
        let key = match pattern {
            Pattern::Ascending => i as u64,
            Pattern::Descending => (n - 1 - i) as u64,
            Pattern::Random => rng.next(),
            Pattern::D20 => rng.next() % 21,
            Pattern::P5 => {
                if rng.next() % 100 < 5 {
                    rng.next()
                } else {
                    0
                }
            }
            Pattern::S95 => {
                if i < n * 95 / 100 {
                    i as u64
                } else {
                    rng.next()
                }
            }
        };
        keys.push(key);
    }
    keys
}

fn checksum_u64(value: &u64) -> u64 {
    *value
}

fn checksum_rec(value: &Rec16) -> u64 {
    value.key
}

fn checksum_big(value: &Big) -> u64 {
    value.key
}

fn bench<T: Copy, F>(original: &[T], reps: usize, mut sort: F, mut checksum: impl FnMut(&T) -> u64) -> f64
where
    F: FnMut(&mut [T]),
{
    let mut work = original.to_vec();
    let mut best = f64::INFINITY;
    let mut sink = 0u64;
    for _ in 0..reps {
        work.copy_from_slice(original);
        let start = Instant::now();
        sort(&mut work);
        let elapsed = start.elapsed().as_nanos() as f64;
        if elapsed < best {
            best = elapsed;
        }
        let sum: u64 = work.iter().map(&mut checksum).sum();
        sink ^= sum;
    }
    black_box(sink);
    best / original.len() as f64
}

fn run_u64(sizes: &[usize], patterns: &[Pattern], reps: usize) {
    for &n in sizes {
        for &pattern in patterns {
            let keys = make_keys(n, pattern);
            let std_ns = bench(&keys, reps, |v| v.sort_unstable(), checksum_u64);
            let ipn_ns = bench(&keys, reps, |v| ipnsort::sort(v), checksum_u64);
            println!(
                "u64,{},{},{},{:.3},{:.3},{:.3}",
                pattern.name(),
                "std",
                n,
                std_ns,
                ipn_ns,
                std_ns / ipn_ns
            );
        }
    }
}

fn run_rec16(sizes: &[usize], patterns: &[Pattern], reps: usize) {
    for &n in sizes {
        for &pattern in patterns {
            let keys = make_keys(n, pattern);
            let original: Vec<Rec16> = keys
                .iter()
                .map(|&key| Rec16 { key, payload: key })
                .collect();
            let std_ns = bench(
                &original,
                reps,
                |v| v.sort_unstable_by(|a, b| a.key.cmp(&b.key)),
                checksum_rec,
            );
            let ipn_ns = bench(
                &original,
                reps,
                |v| ipnsort::sort_by(v, |a, b| a.key.cmp(&b.key)),
                checksum_rec,
            );
            println!(
                "rec16,{},{},{},{:.3},{:.3},{:.3}",
                pattern.name(),
                "std",
                n,
                std_ns,
                ipn_ns,
                std_ns / ipn_ns
            );
        }
    }
}

fn run_big(sizes: &[usize], patterns: &[Pattern], reps: usize) {
    for &n in sizes {
        for &pattern in patterns {
            let keys = make_keys(n, pattern);
            let original: Vec<Big> = keys
                .iter()
                .map(|&key| {
                    let mut payload = [0u64; 127];
                    for (i, slot) in payload.iter_mut().enumerate() {
                        *slot = key.wrapping_add(i as u64);
                    }
                    Big { key, payload }
                })
                .collect();
            let std_ns = bench(
                &original,
                reps,
                |v| v.sort_unstable_by(|a, b| a.key.cmp(&b.key)),
                checksum_big,
            );
            let ipn_ns = bench(
                &original,
                reps,
                |v| ipnsort::sort_by(v, |a, b| a.key.cmp(&b.key)),
                checksum_big,
            );
            println!(
                "big1k,{},{},{},{:.3},{:.3},{:.3}",
                pattern.name(),
                "std",
                n,
                std_ns,
                ipn_ns,
                std_ns / ipn_ns
            );
        }
    }
}

fn main() {
    let quick = std::env::args().nth(1).as_deref() == Some("--quick");
    let reps = if quick { 2 } else { 4 };

    let u64_sizes: Vec<usize> = if quick {
        vec![1000, 10000, 100000]
    } else {
        vec![1000, 10000, 100000, 1000000]
    };
    let patterns = [
        Pattern::Ascending,
        Pattern::Descending,
        Pattern::Random,
        Pattern::D20,
        Pattern::P5,
        Pattern::S95,
    ];

    run_u64(&u64_sizes, &patterns, reps);
    run_rec16(&[10000, 100000], &patterns, reps);
    run_big(&[1000, 10000], &[Pattern::Ascending, Pattern::Random], reps);
}
