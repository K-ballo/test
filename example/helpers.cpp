// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <string_view>

// CHECK and REQUIRE can be called from helper functions outside the TEST_CASE
// body.  When an assertion fails, the framework captures a stacktrace of all
// user-code frames above it so the failure site is easy to locate.  CONTEXT
// further pins down which call, among several to the same helper, is the one
// that failed.

namespace {

void check_equal(int a, int b)
{
    CONTEXT("a={} b={}", a, b);
    CHECK(a == b);
}

void require_non_empty(std::string_view s)
{
    CONTEXT(s);
    REQUIRE(!s.empty());
}

} // namespace

TEST_CASE(helper_check, "CHECK called from a helper function")
{
    check_equal(1, 1);
    check_equal(42, 42);
    check_equal(0, 0);
}

TEST_CASE(helper_require, "REQUIRE called from a helper function")
{
    require_non_empty("hello");
    require_non_empty("world");
}
