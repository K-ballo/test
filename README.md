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

`CONTEXT(fmt, args...)` - records a scoped diagnostic message shown alongside every `CHECK`/`REQUIRE` diagnostic produced while still in scope.

### Parameterized tests

```cpp
TEST_CASE(add, "adds two integers", int a, int b) {
    CHECK(a + b == b + a);
}

REGISTER_P(add, "one_two", 1, 2);
REGISTER_P(add, "zero_five", 0, 5);
```

Parameter types can also be deduced:

```cpp
TEST_CASE(add, "adds two numbers", auto a, auto b) {
    CHECK(a + b == b + a);
}

REGISTER_P(add, "ints", 1, 2);
REGISTER_P(add, "doubles", 1.5, 2.5);
```

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
