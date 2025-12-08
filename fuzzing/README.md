# Fuzz Test of yaclap
Note: the fuzzing subdirectory is not part of the CMakeLists.txt project in the parent directory on purpose.
The fuzzing test is a very special target, and those is managed on it's own within this subdirectory.

The fuzzing test is written with the google [FuzzTest framework](https://github.com/google/fuzztest).

The test is implemented in `yaclap_fuzz_test.cc`, based on the generic module tests.
The parser setup and input test data is chosen to reach a high level of coverage in the implementation.
(Note: the coverage will not reach 100% as many branches will never be hit.
Those might be doubled checks in the context of defensive programming,
or checks during setup of the parser, i.e. not relevant to fuzzing as they are not exposed to user input data.)

The fuzz test source code is then compiled with `clang` from a dedicated `CMakeLists.txt` file.
It is configured to instrument for coverage based fuzzing test data mutation, and coverage reporting to judge fuzzing result quality.

You can use the info in the section [Docker](#docker) below, to test fuzzing on your machine, without impacting your development setup in any way.

A full fuzzing test should run for multiple hours or days, with billions of repetitions, to increase the likelihood of uncovering issues.

🚧 TODO Doc full fuzzing run

🚧 TODO Doc fuzzing results

## Docker
Build the prepared Docker image:
```
docker build -t yaclap-fuzz .
```

You can run the docker image interactively:
```
docker run -it --rm -v "${PWD}/..:/usr/src/app" yaclap-fuzz
```

### Build Test Program

Then, inside the dockered session:
```
cd fuzzings

rm -rf build
mkdir build

cd build
```

Build "Debug" for coverage info or Release or RelWithDebug for faster test runs
```
CC=clang CXX=clang++ cmake -DCMAKE_BUILD_TYPE=Debug -DFUZZTEST_FUZZING_MODE=on ..
```
```
CC=clang CXX=clang++ cmake -DCMAKE_BUILD_TYPE=RelWithDebug -DFUZZTEST_FUZZING_MODE=on ..
```

Then build:
```
CMAKE_BUILD_PARALLEL_LEVEL=8 cmake --build .
```

### Run Fuzz Test

Then run, with or without coverage:
```
LLVM_PROFILE_FILE="coverage.profraw" ./yaclap_fuzz_test --fuzz=Yaclap.FuzzTest
```
```
./yaclap_fuzz_test --fuzz=Yaclap.FuzzTest
```
Optionally add: `--fuzz_for=60s`, `--fuzz_for=5m`, `--fuzz_for=1h` or similar.

### Report Coverage

You should delete all coverage report files _before_ running the fuzz test:
```
rm -fr coverage*
```

If coverage files are generated, they can be merged:
```
llvm-profdata-15 merge -sparse coverage.profraw -o coverage.profdata
```

Summary Report is written by:
```
llvm-cov-15 report ./yaclap_fuzz_test -instr-profile=coverage.profdata --ignore-filename-regex='fuzzing/(external|build)/.*' 
```

Detailed Report is written by:
```
llvm-cov-15 show ./yaclap_fuzz_test -instr-profile=coverage.profdata --ignore-filename-regex='fuzzing/(external|build)/.*' -format=html -output-dir=coverage_html
```

As one-liner for a whole run:
```
rm -fr coverage*; LLVM_PROFILE_FILE="coverage.profraw" ./yaclap_fuzz_test --fuzz=Yaclap.FuzzTest --fuzz_for=2m; llvm-profdata-15 merge -sparse coverage.profraw -o coverage.profdata; llvm-cov-15 show ./yaclap_fuzz_test -instr-profile=coverage.profdata --ignore-filename-regex='fuzzing/(external|build)/.*' -format=html -output-dir=coverage_html; llvm-cov-15 report ./yaclap_fuzz_test -instr-profile=coverage.profdata --ignore-filename-regex='fuzzing/(external|build)/.*'
```
