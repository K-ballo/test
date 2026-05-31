// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/detail/noinline.hpp>

// Verify that CHECK_THROWS fired from helper functions produces a stacktrace
// in its diagnostic.

namespace {

EGGS_TEST_NOINLINE void failing_throws_helper()
{
    CHECK_THROWS(static_cast<void>(1 + 1));
}

EGGS_TEST_NOINLINE void outer_throws_helper()
{
    failing_throws_helper();

    // Prevent tail-call elimination.
    volatile bool done = true;
    (void)done;
}

} // namespace

TEST_CASE(
    stacktrace_throws_one_level,
    "failed CHECK_THROWS one level deep prints a stacktrace"
)
{
    failing_throws_helper();
}

TEST_CASE(
    stacktrace_throws_two_levels,
    "failed CHECK_THROWS two levels deep prints all user frames"
)
{
    outer_throws_helper();
}
