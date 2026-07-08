// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

// First translation unit - verifies that test cases defined across separate
// TUs both register and run when linked into the same executable.

TEST_CASE(multi_tu_case_a, "test case in translation unit A")
{
    CHECK(true);
    CHECK(1 == 1);
}
