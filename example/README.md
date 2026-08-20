# Examples

## basic

Demonstrates `TEST_CASE`, `CHECK`, and `REQUIRE` with arithmetic, string, and
container checks.

## exceptions

Demonstrates `CHECK_THROWS`, `CHECK_THROWS_AS`, `CHECK_CATCHES_AS`, and
`CHECK_NOTHROW` using a small `parse_int` function that throws on bad input.

## helpers

Shows how `CHECK` and `REQUIRE` can be called from helper functions outside the
`TEST_CASE` body.  On failure the framework prints a stacktrace of user-code
frames so the call site is easy to find.

## parametric

Demonstrates parameterized `TEST_CASE` and `REGISTER_P`, registering several
named instances of the same test body with different arguments, including a
templated (`auto`) test case that deduces a different type per registration.
