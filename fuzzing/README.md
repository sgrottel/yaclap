# Fuzz Test of yaclap
Note: the fuzzing subdirectory is not part of the CMakeLists.txt project in the parent directory on purpose.
The fuzzing test is a very special target, and those is managed on it's own within this subdirectory.

🚧 TODO Docu

## Docker
Build the prepared Docker image:
```
docker build -t yaclap-fuzz .
```

You can run the docker image interactively:
```
docker run -it --rm -v "${PWD}/..:/usr/src/app" yaclap-fuzz
```

Then, inside the dockered session:
```
cd fuzzing
mkdir build
cd build

CC=clang CXX=clang++ cmake -DCMAKE_BUILD_TYPE=RelWithDebug -DFUZZTEST_FUZZING_MODE=on ..
cmake --build .
./first_fuzz_test --fuzz=MyTestSuite.IntegerAdditionCommutes

```

Previously:
```
CC=clang CXX=clang++ cmake -DCMAKE_BUILD_TYPE=RelWithDebug ..
cmake --build .
./first_fuzz_test

```

