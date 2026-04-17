# C++ Template Project

A CMake-based C++ project template for testing and implementing C++ code.

## Project Structure

```
.
├── build/         # Build directory (generated)
│   ├── bin/       # Executable files output directory
│   └── lib/       # Library files output directory
├── include/       # Header files
│   └── example.hpp
├── src/           # Source code directory
│   ├── CMakeLists.txt
│   ├── main.cpp
│   └── example.cpp
├── tests/         # Test files directory
│   ├── CMakeLists.txt
│   └── test_example.cpp
└── CMakeLists.txt
```

## Build Instructions

1. Create build directory:
```bash
mkdir build
cd build
```

2. Configure with CMake:
```bash
cmake ..
```

3. Build the project:
```bash
cmake --build .
```

4. Run the main executable:
```bash
./bin/cpp_template_main
```

5. Run tests:
```bash
./bin/test_example
```

## Adding New Code

1. Add header files to `include/` directory
2. Add source files to `src/` directory
3. Add test files to `tests/` directory (files starting with `test_` will be compiled as executables)

## Clean Build

```bash
cd build
cmake --build . --clean-first
```

Or remove the build directory:
```bash
rm -rf build
```
