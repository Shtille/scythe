# Build details

## Requirements
[Conan](https://conan.io/) is required

## Prepare
With Conan at first we need to make sure that our profile is configured:
```bash
conan profile detect --force
```

## Build
From project directory:
```bash
conan install . --build=missing -s compiler.cppstd=17
cmake --preset conan-default
cmake --build --preset conan-release
```

## To test
Need to test possibility to run Conan configuration directly from CMake.
```bash
cmake -S . -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=conan_provider.cmake -DCMAKE_BUILD_TYPE=Release
```