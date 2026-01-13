# Fuzzing Results Jan. 2026

This covers a manually executed fuzzing run of the head of this repository in January 2026.

⏱️ The fuzz test was run for 5 days = 120 h.

✅ No issues or crashes were encountered.

📃 The full log: [fuzz_test_run_2026_01.log](./fuzz_test_run_2026_01.log)


## Setup

- OS: Ubuntu 24.04.3 LTS
- SW:
  - Ubuntu clang version 18.1.3 (1ubuntu1)
  - GNU ld (GNU Binutils for Ubuntu) 2.42
  - ldd (Ubuntu GLIBC 2.39-0ubuntu8.6) 2.39
  - cmake version 3.28.3
  - GNU Make 4.3
  - llvm-15 1:15.0.7-14build3
- HW: Alienware 15 R2
  - Intel(R) Core(TM) i7-6700HQ CPU @ 2.60GHz
  - 16 GB RAM

```
$ cd yaclap
$ git pull
Already up to date.
$ git rev-parse HEAD
e6eb4b83bb922ec4278f79dfe9928a484154d6d7
$ git status
On branch fuzztest
Your branch is up to date with 'origin/fuzztest'.

nothing to commit, working tree clean
$ cd fuzzing/
$ rm -rf build/
$ mkdir build
$ cd build
$ CC=clang CXX=clang++ cmake -DCMAKE_BUILD_TYPE=Release -DFUZZTEST_FUZZING_MODE=on ..
$ CMAKE_BUILD_PARALLEL_LEVEL=8 cmake --build .
```

Run:
```
$ screen -S fuzzsession
$ ./yaclap_fuzz_test --fuzz=Yaclap.FuzzTest --fuzz_for=120h 2>&1 | tee fuzz_test_run.log

Ctrl+a  d

$ screen -r fuzzsession

exit
```

## Results

📃 The full log: [fuzz_test_run_2026_01.log](./fuzz_test_run_2026_01.log)

Excerpt:

```
[.] Sanitizer coverage enabled. Counter map size: 53897, Cmp map size: 262144
Note: Google Test filter = Yaclap.FuzzTest
[==========] Running 1 test from 1 test suite.
[----------] Global test environment set-up.
[----------] 1 test from Yaclap
[ RUN      ] Yaclap.FuzzTest
FUZZTEST_PRNG_SEED=Hd-HiuI4QL-VLWH8VQqaVoF4NpGjLlinb3Fw58clmbE
[*] Corpus size:     1 | Edges covered:   1689 | Fuzzing time:       2.426221ms | Total runs:  1.00e+00 | Runs/secs:   412 | Max stack usage:     6416
[*] Corpus size:     2 | Edges covered:   1767 | Fuzzing time:        3.31898ms | Total runs:  2.00e+00 | Runs/secs:   602 | Max stack usage:     6416
[*] Corpus size:     3 | Edges covered:   1772 | Fuzzing time:       4.294517ms | Total runs:  3.00e+00 | Runs/secs:   698 | Max stack usage:     6480
[*] Corpus size:     4 | Edges covered:   1779 | Fuzzing time:       5.182873ms | Total runs:  4.00e+00 | Runs/secs:   771 | Max stack usage:     6480

...

[*] Corpus size:  1754 | Edges covered:   3893 | Fuzzing time: 118h40m24.962022576s | Total runs:  4.20e+08 | Runs/secs:   984 | Max stack usage:     6480
[*] Corpus size:  1755 | Edges covered:   3893 | Fuzzing time: 119h33m52.080835043s | Total runs:  4.24e+08 | Runs/secs:   983 | Max stack usage:     6480

[.] Fuzzing was terminated.

=================================================================
=== Fuzzing stats

Elapsed time: 120h0.000447639s
Total runs: 425047131
Edges covered: 3893
Total edges: 53897
Corpus size: 1755
Max stack used: 6480

[       OK ] Yaclap.FuzzTest (431999952 ms)
[----------] 1 test from Yaclap (431999952 ms total)

[----------] Global test environment tear-down
[==========] 1 test from 1 test suite ran. (431999952 ms total)
[  PASSED  ] 1 test.
```
