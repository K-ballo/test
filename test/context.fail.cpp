// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/detail/noinline.hpp>

// Exercises CONTEXT alongside a FAILED diagnostic (no --verbose needed: FAILED
// diagnostics are always printed).

TEST_CASE(context_fail, "CONTEXT appears in a FAILED diagnostic too")
{
    int id = 7;
    CONTEXT(id);
    CHECK(id == 42);
}

TEST_CASE(
    context_snapshot_fail,
    "CONTEXT reflects the value when CONTEXT(...) was called, not at the "
    "point of failure"
)
{
    int counter = 0;
    CONTEXT(counter);
    counter = 5;
    CHECK(counter == 999);
}

TEST_CASE(
    context_fmt_fail, "CONTEXT(fmt, args...) appears in a FAILED diagnostic too"
)
{
    int i = 1;
    int j = 2;
    CONTEXT("i={} j={}", i, j);
    CHECK(false);
}

namespace {

EGGS_TEST_NOINLINE void failing_context_helper()
{
    int id = 3;
    CONTEXT(id);
    CHECK(id == 42);
}

} // namespace

TEST_CASE(
    context_with_stacktrace_fail,
    "CONTEXT appears after the source location but before any stacktrace"
)
{
    failing_context_helper();
}
