# Eggs.Test

A minimal C++ test framework. Requires C++23.

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

`CHECK(expr)` - evaluates *expr*; reports failure and continues.  
`REQUIRE(expr)` - like `CHECK`, but stops the current test case on failure.

`CHECK_THROWS(expr)` - passes if *expr* throws any exception.  
`CHECK_THROWS_AS(ExcType, expr)` - passes if *expr* throws an exception of type *ExcType*; returns `std::exception_ptr` on success.  
`CHECK_CATCHES_AS(ExcType, expr) { }` - like `CHECK_THROWS_AS`; the body runs on success with the caught exception bound to `exc`.  
`CHECK_NOTHROW(expr)` - passes if *expr* does not throw.

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
target_link_libraries(my_tests PRIVATE Eggs::TestMain)

include(EggsTest)
eggs_discover_tests(my_tests)
```

`Eggs::Test` provides the test macros and the `eggs::test::run()` entry point.  
`Eggs::TestMain` additionally supplies a `main()` with CLI argument handling.

`eggs_discover_tests()` registers one CTest test per `TEST_CASE` in `my_tests`, discovered at
test time (so added/renamed/removed test cases don't require re-running CMake):

```cmake
eggs_discover_tests(target
    [TEST_PREFIX prefix] [TEST_SUFFIX suffix]
    [EXTRA_ARGS args...]
    [WORKING_DIRECTORY dir]
    [PROPERTIES name1 value1...]
    [TEST_LIST var]
)
```

## License

See [LICENSE.txt](LICENSE.txt).
