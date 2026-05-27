// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

// Second translation unit.

TEST_CASE(multi_tu_case_b, "test case in translation unit B")
{
    CHECK(2 + 2 == 4);
    CHECK('x' == 'x');
}
