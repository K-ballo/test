# eggs.test

A minimal, header-only C++ test framework. Requires C++23.

## Usage

```cpp
#include <eggs/test.hpp>

TEST_CASE(my_test, "adds two integers") {
    CHECK(1 + 1 == 2);
    REQUIRE(1 + 1 == 2);  // stops the test case on failure
}

int main() {
    return eggs::test::run();
}
```

`CHECK(expr)` — reports failure and continues.  
`REQUIRE(expr)` — reports failure and stops the current test case.  
Both print a stacktrace when called from a helper function.

## Build

```bash
cmake --preset dev-gcc        # or dev-clang, dev-clang-libcxx, dev-msvc
cmake --build --preset dev-gcc-debug
ctest --preset dev-gcc-debug
```

See `CMakePresets.json` for the full list of presets (GCC, Clang, Clang+libc++, MSVC, ASan, TSan).

## CMake integration

```cmake
find_package(eggs.test REQUIRED)
target_link_libraries(my_tests PRIVATE Eggs::Test Eggs::TestMain)
```

## License

See [LICENSE.txt](LICENSE.txt).
