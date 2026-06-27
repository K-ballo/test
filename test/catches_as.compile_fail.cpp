// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <stdexcept>

TEST_CASE(
    check_catches_as_compile_fail,
    "CHECK_CATCHES_AS rejects noexcept expressions"
)
{
    CHECK_CATCHES_AS(std::runtime_error, 1 + 1) {}
}
