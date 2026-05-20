// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

// Verify that CHECK and REQUIRE can be called from helper functions without
// crashing on the passing path (no stacktrace is printed when checks pass).

namespace {

void check_helper()
{
    CHECK(1 + 1 == 2);
}

void require_helper()
{
    REQUIRE(1 + 1 == 2);
}

void nested_check_helper()
{
    check_helper();
}

} // namespace

TEST_CASE(
    stacktrace_check_via_helper, "CHECK in a helper function passes cleanly"
)
{
    check_helper();
}

TEST_CASE(
    stacktrace_require_via_helper, "REQUIRE in a helper function passes cleanly"
)
{
    require_helper();
}

TEST_CASE(
    stacktrace_check_via_nested_helpers, "CHECK two helpers deep passes cleanly"
)
{
    nested_check_helper();
}

TEST_CASE(stacktrace_mixed, "mix of direct and helper CHECKs all pass")
{
    CHECK(true);
    check_helper();
    CHECK(2 * 2 == 4);
}
