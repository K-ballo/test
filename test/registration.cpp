// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

// Verify that multiple TEST_CASEs in a single translation unit all register
// and run without interfering with each other.

TEST_CASE(registration_one, "first test case registers")
{
    CHECK(true);
}

TEST_CASE(registration_two, "second test case registers independently")
{
    CHECK(1 + 1 == 2);
}

TEST_CASE(registration_three, "third test case also registers")
{
    CHECK('a' != 'b');
}
