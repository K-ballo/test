# Eggs.Test

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

### Assertion macros

`CHECK(expr)` — evaluates *expr*; reports failure and continues.  
`REQUIRE(expr)` — like `CHECK`, but stops the current test case on failure.

`CHECK_THROWS(expr)` — passes if *expr* throws any exception.  
`CHECK_THROWS_AS(ExcType, expr)` — passes if *expr* throws an exception of type *ExcType*; returns `std::exception_ptr` on success.  
`CHECK_CATCHES_AS(ExcType, expr) { }` — like `CHECK_THROWS_AS`; the body runs on success with the caught exception bound to `exc`.  
`CHECK_NOTHROW(expr)` — passes if *expr* does not throw.

Every `CHECK_*` macro has a `REQUIRE_*` variant that stops the test case on failure.

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
