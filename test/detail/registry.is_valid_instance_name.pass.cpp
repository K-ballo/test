// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test/detail/registry.hpp>

#include "support.hpp"

int main()
{
    // An empty name is invalid.
    EGGS_TEST_DETAIL_ASSERT(!eggs::test::detail::is_valid_instance_name(""));

    // A single letter, upper or lower case, is valid.
    EGGS_TEST_DETAIL_ASSERT(eggs::test::detail::is_valid_instance_name("a"));
    EGGS_TEST_DETAIL_ASSERT(eggs::test::detail::is_valid_instance_name("Z"));

    // A name starting with a digit is invalid.
    EGGS_TEST_DETAIL_ASSERT(!eggs::test::detail::is_valid_instance_name("1st"));

    // A name starting with any of the allowed symbols is still invalid: the
    // first character must be a letter.
    EGGS_TEST_DETAIL_ASSERT(!eggs::test::detail::is_valid_instance_name("_x"));
    EGGS_TEST_DETAIL_ASSERT(!eggs::test::detail::is_valid_instance_name(".x"));
    EGGS_TEST_DETAIL_ASSERT(!eggs::test::detail::is_valid_instance_name("/x"));
    EGGS_TEST_DETAIL_ASSERT(!eggs::test::detail::is_valid_instance_name("-x"));

    // Letters, digits, and '_', '.', '/', '-' are all valid after the first
    // character.
    EGGS_TEST_DETAIL_ASSERT(
        eggs::test::detail::is_valid_instance_name("Az9_./-")
    );

    // A name containing a character outside letters/digits/symbols is
    // invalid.
    EGGS_TEST_DETAIL_ASSERT(
        !eggs::test::detail::is_valid_instance_name("two words")
    );
    EGGS_TEST_DETAIL_ASSERT(!eggs::test::detail::is_valid_instance_name("a@b"));

    return 0;
}
