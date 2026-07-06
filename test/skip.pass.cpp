// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

TEST_CASE(skip_reason, "SKIP marks the test case as skipped, not failed")
{
    SKIP("not supported on this platform");
    // The runner must not reach this; if it does, the test case is reported
    // as failed instead of skipped.
    CHECK(false);
}

TEST_CASE(skip_after_checks_pass, "SKIP after passing checks is still skipped")
{
    CHECK(1 + 1 == 2);
    SKIP("skipping the rest of this test case");
    CHECK(false); // must not be reached
}
