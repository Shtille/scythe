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

## Package

### Deployment

On deployment we should enable all available options:

```bash
conan create . --build=missing -s compiler.cppstd=17 -o OpenGL=True
```

### Comsuming

To consume package we should find package in CMakeLists.txt file:
```cmake
find_package(scythe CONFIG REQUIRED)
target_link_libraries(example scythe::scythe)
```
And in `conanfile.py`:
```python
def requirements(self):
    self.requires("scythe/1.0", options={"OpenGL": True})
```