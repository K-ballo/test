// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

TEST_CASE(
    zero_assertions_pass, "a test case with no assertions is reported as passed"
)
{
}

TEST_CASE(
    zero_assertions_fail,
    "a test case that throws before any assertion is reported as failed"
)
{
    throw 1;
}
