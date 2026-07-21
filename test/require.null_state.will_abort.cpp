// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/detail/run_state.hpp>

TEST_CASE(
    require_null_state, "REQUIRE aborts when there is no active run_state"
)
{
    eggs::test::detail::run_state::set_current(nullptr);
    REQUIRE(true); // aborts
}
