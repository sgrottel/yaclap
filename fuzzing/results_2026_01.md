# Fuzzing Results Jan. 2026

This covers a manually executed fuzzing run of the head of this repository in January 2026.

## Setup

- OS: Ubuntu 24.04.3 LTS
- SW:
  - Ubuntu clang version 18.1.3 (1ubuntu1)
  - GNU ld (GNU Binutils for Ubuntu) 2.42
  - ldd (Ubuntu GLIBC 2.39-0ubuntu8.6) 2.39
  - cmake version 3.28.3
  - GNU Make 4.3
  - llvm-15 1:15.0.7-14build3
- HW: 🚧 TODO
  - Intel(R) Core(TM) i7-6700HQ CPU @ 2.60GHz
  - 🚧 TODO RAM

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

[🚧 TODO: The full log]()

```
🚧 TODO: The run log excerpt beginning ...
...
🚧 TODO: ... and end.
```
