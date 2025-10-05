# oryx-crt-cpp

Common C++ Runtime used for personal C++ Projects.

## Third Party libraries

- [httplib](https://github.com/yhirose/cpp-httplib) MIT license
- [thread_pool](https://github.com/bshoshany/thread-pool.git) MIT license
- [enchantum](https://github.com/ZXShady/enchantum.git) MIT license
- [spsc_queue](https://github.com/facebook/folly.git) Apache-2.0 license

## Build Locally

```bash
cmake -GNinja -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Debug -Bbuild -H.
      ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      Only needed for clangd   
```

```bash
cmake --build build -j32
```

## Adding this library to your project

```cmake
include(FetchContent)

FetchContent_Declare(
    oryx-crt-cpp
    GIT_REPOSITORY https://github.com/BestITUserEUW/oryx-crt-cpp.git
    GIT_TAG main
    OVERRIDE_FIND_PACKAGE
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(oryx-crt-cpp)

find_package(oryx-crt-cpp REQUIRED)

target_link_libraries(my_project PUBLIC
    oryx::oryx-crt-cpp
)
```

## IDE Setup

### Clangd Extension (Recommended)

- Install clangd language server `apt install clangd`
- Go to Extension and install `llvm-vs-code-extensions.vscode-clangd`