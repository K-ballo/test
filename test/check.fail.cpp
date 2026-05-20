// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

// This test case is intentionally failing.

TEST_CASE(check_false, "CHECK with a false expression is reported as a failure")
{
    CHECK(1 + 2 == 4);
}
