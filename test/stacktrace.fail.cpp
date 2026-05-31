// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/detail/noinline.hpp>

#include <stdexcept>

// Verify that assertion macros fired from helper functions produce a stacktrace
// in their diagnostic.

namespace {

EGGS_TEST_NOINLINE void failing_check_helper()
{
    CHECK(false);
}

} // namespace

TEST_CASE(
    stacktrace_check_one_level,
    "failed CHECK one level deep prints a stacktrace"
)
{
    failing_check_helper();
}

namespace {

EGGS_TEST_NOINLINE void outer_check_helper()
{
    failing_check_helper();
}

} // namespace

TEST_CASE(
    stacktrace_check_two_levels,
    "failed CHECK two levels deep prints all user frames"
)
{
    outer_check_helper();
}

namespace {

EGGS_TEST_NOINLINE void failing_require_helper()
{
    REQUIRE(false);
}

} // namespace

TEST_CASE(
    stacktrace_require_one_level,
    "failed REQUIRE one level deep prints a stacktrace"
)
{
    failing_require_helper();
}

namespace {

EGGS_TEST_NOINLINE void outer_require_helper()
{
    failing_require_helper();
}

} // namespace

TEST_CASE(
    stacktrace_require_two_levels,
    "failed REQUIRE two levels deep prints all user frames"
)
{
    outer_require_helper();
}

namespace {

EGGS_TEST_NOINLINE void failing_throws_helper()
{
    CHECK_THROWS(1 + 1);
}

} // namespace

TEST_CASE(
    stacktrace_throws_one_level,
    "failed CHECK_THROWS one level deep prints a stacktrace"
)
{
    failing_throws_helper();
}

namespace {

EGGS_TEST_NOINLINE void outer_throws_helper()
{
    failing_throws_helper();
}

} // namespace

TEST_CASE(
    stacktrace_throws_two_levels,
    "failed CHECK_THROWS two levels deep prints all user frames"
)
{
    outer_throws_helper();
}

namespace {

EGGS_TEST_NOINLINE void failing_throws_as_helper()
{
    CHECK_THROWS_AS(std::runtime_error, 1 + 1);
}

} // namespace

TEST_CASE(
    stacktrace_throws_as_one_level,
    "failed CHECK_THROWS_AS one level deep prints a stacktrace"
)
{
    failing_throws_as_helper();
}

namespace {

EGGS_TEST_NOINLINE void outer_throws_as_helper()
{
    failing_throws_as_helper();
}

} // namespace

TEST_CASE(
    stacktrace_throws_as_two_levels,
    "failed CHECK_THROWS_AS two levels deep prints all user frames"
)
{
    outer_throws_as_helper();
}

namespace {

EGGS_TEST_NOINLINE void failing_catches_helper()
{
    CHECK_CATCHES_AS(std::runtime_error, 1 + 1)
    {
        // unreachable: expression does not throw
    }
}

} // namespace

TEST_CASE(
    stacktrace_catches_one_level,
    "failed CHECK_CATCHES_AS one level deep prints a stacktrace"
)
{
    failing_catches_helper();
}

namespace {

EGGS_TEST_NOINLINE void outer_catches_helper()
{
    failing_catches_helper();
}

} // namespace

TEST_CASE(
    stacktrace_catches_two_levels,
    "failed CHECK_CATCHES_AS two levels deep prints all user frames"
)
{
    outer_catches_helper();
}

namespace {

EGGS_TEST_NOINLINE void failing_nothrow_helper()
{
    CHECK_NOTHROW(throw 42);
}

} // namespace

TEST_CASE(
    stacktrace_nothrow_one_level,
    "failed CHECK_NOTHROW one level deep prints a stacktrace"
)
{
    failing_nothrow_helper();
}

namespace {

EGGS_TEST_NOINLINE void outer_nothrow_helper()
{
    failing_nothrow_helper();
}

} // namespace

TEST_CASE(
    stacktrace_nothrow_two_levels,
    "failed CHECK_NOTHROW two levels deep prints all user frames"
)
{
    outer_nothrow_helper();
}
