// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <concepts>
#include <cstdlib>
#include <string_view>

// A TEST_CASE with parameters doesn't auto-register; each REGISTER_P call
// registers one named instance (as "name/instance").

TEST_CASE(is_even, "parity check", int n)
{
    CHECK(n % 2 == 0);
}

REGISTER_P(is_even, "zero", 0);
REGISTER_P(is_even, "two", 2);
REGISTER_P(is_even, "negative_four", -4);

// REGISTER_P forwards as many arguments as the TEST_CASE declares.

TEST_CASE(
    starts_with, "prefix check", std::string_view s, std::string_view prefix
)
{
    CHECK(s.starts_with(prefix));
}

REGISTER_P(starts_with, "greeting", "hello world", "hello");
REGISTER_P(starts_with, "empty_prefix", "hello world", "");

// A parameter can also be `auto` (optionally constrained), deducing a
// different type for each REGISTER_P instance.

TEST_CASE(
    abs_is_non_negative, "abs never returns negative",
    std::signed_integral auto value
)
{
    CHECK(std::abs(value) >= 0);
}

REGISTER_P(abs_is_non_negative, "int", -7);
REGISTER_P(abs_is_non_negative, "long", -7L);
REGISTER_P(abs_is_non_negative, "short", short{-7});
