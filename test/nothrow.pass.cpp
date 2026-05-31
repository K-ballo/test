// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <stdexcept>

TEST_CASE(
    check_nothrow_std_exception,
    "CHECK_NOTHROW passes when std exception does not propagate"
)
{
    std::runtime_error ex{"not thrown"};
    CHECK_NOTHROW(ex.what());
}

TEST_CASE(
    require_nothrow_std_exception,
    "REQUIRE_NOTHROW passes without stopping the test"
)
{
    std::runtime_error ex{"not thrown"};
    REQUIRE_NOTHROW(ex.what());
    CHECK(true);
}
