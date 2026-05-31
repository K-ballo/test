// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/detail/noinline.hpp>

#include <stdexcept>

// Verify that CHECK_CATCHES_AS fired from helper functions produces a stacktrace
// in its diagnostic.

namespace {

EGGS_TEST_NOINLINE void failing_catches_helper()
{
    CHECK_CATCHES_AS(std::runtime_error, static_cast<void>(1 + 1))
    {
        // unreachable: expression does not throw
    }
}

EGGS_TEST_NOINLINE void outer_catches_helper()
{
    failing_catches_helper();

    // Prevent tail-call elimination.
    volatile bool done = true;
    (void)done;
}

} // namespace

TEST_CASE(
    stacktrace_catches_one_level,
    "failed CHECK_CATCHES_AS one level deep prints a stacktrace"
)
{
    failing_catches_helper();
}

TEST_CASE(
    stacktrace_catches_two_levels,
    "failed CHECK_CATCHES_AS two levels deep prints all user frames"
)
{
    outer_catches_helper();
}
