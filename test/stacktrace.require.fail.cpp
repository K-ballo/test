// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/detail/noinline.hpp>

// Verify that REQUIRE fired from helper functions produces a stacktrace in its
// diagnostic.

namespace {

EGGS_TEST_NOINLINE void failing_require_helper()
{
    REQUIRE(false);
}

EGGS_TEST_NOINLINE void outer_helper()
{
    failing_require_helper();
}

} // namespace

TEST_CASE(
    stacktrace_require_one_level,
    "failed REQUIRE one level deep prints a stacktrace"
)
{
    failing_require_helper();
}

TEST_CASE(
    stacktrace_require_two_levels,
    "failed REQUIRE two levels deep prints all user frames"
)
{
    outer_helper();
}
