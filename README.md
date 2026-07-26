# vvhl

## Dependencies
- gcc
- Vulkan SDK
- glslc (included in Vulkan SDK)
- slangc (optional, for slang shaders)

## Configure

Generate the build system:

```bash
# Debug
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug

# Development
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Release
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

---

## Build

Compile the project:

```bash
cmake --build build
```

---

## Output

Executables and libraries are generated inside the `build/` directory.

A recommended output layout is:

```cmake
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
```

Result:

```text
build/
├── bin/
└── lib/
```

## Clean Previous Configuration

Remove the previous build directory before reconfiguring:

```bash
rm -rf build
```

---