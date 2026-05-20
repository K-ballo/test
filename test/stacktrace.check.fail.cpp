// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

// Verify that CHECK fired from helper functions produces a stacktrace in its
// diagnostic.  Every test case here fails, so the executable exits 1.

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

EGGS_TEST_NOINLINE void outer_helper()
{
    failing_check_helper();

    // Prevent tail-call elimination: optimizer must preserve this frame
    // because the volatile read is observable after the call returns.
    volatile bool done = true;
    (void)done;
}

} // namespace

TEST_CASE(
    stacktrace_check_one_level,
    "failed CHECK one level deep prints a stacktrace"
)
{
    failing_check_helper();
}

TEST_CASE(
    stacktrace_check_two_levels,
    "failed CHECK two levels deep prints all user frames"
)
{
    outer_helper();
}
