// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/detail/noinline.hpp>

// Verify that CHECK_NOTHROW fired from helper functions produces a stacktrace
// in its diagnostic.

namespace {

EGGS_TEST_NOINLINE void failing_nothrow_helper()
{
    CHECK_NOTHROW(throw 42);
}

EGGS_TEST_NOINLINE void outer_nothrow_helper()
{
    failing_nothrow_helper();

    // Prevent tail-call elimination.
    volatile bool done = true;
    (void)done;
}

} // namespace

TEST_CASE(
    stacktrace_nothrow_one_level,
    "failed CHECK_NOTHROW one level deep prints a stacktrace"
)
{
    failing_nothrow_helper();
}

TEST_CASE(
    stacktrace_nothrow_two_levels,
    "failed CHECK_NOTHROW two levels deep prints all user frames"
)
{
    outer_nothrow_helper();
}
