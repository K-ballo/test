// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

TEST_CASE(per_case_pass, "a passing test case with one assertion")
{
    CHECK(true);
}

TEST_CASE(per_case_fail, "a failing test case with one assertion")
{
    CHECK(1 + 2 == 4);
}
