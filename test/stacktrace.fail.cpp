// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

// Verify that CHECK/REQUIRE fired from helper functions produce a stacktrace
// in their diagnostic.

#ifdef _MSC_VER
#    define EGGS_TEST_NOINLINE __declspec(noinline)
#else
#    define EGGS_TEST_NOINLINE __attribute__((noinline))
#endif

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
