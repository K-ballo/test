// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

TEST_CASE(skip_after_check_failure, "SKIP cannot undo an earlier failed CHECK")
{
    CHECK(1 + 1 == 3);
    SKIP("attempt to skip after a failure");
}
