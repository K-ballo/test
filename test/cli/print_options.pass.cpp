// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/cli.hpp>

#include <cstdio>

TEST_CASE(
    print_options_bare, "print_options without namespace lists bare flags"
)
{
    eggs::test::print_options(stdout);
}

TEST_CASE(
    print_options_ns, "print_options with ns prefixes flags with --<ns>:<flag>"
)
{
    eggs::test::print_options(stdout, "ns");
}

TEST_CASE(
    print_options_desc_col,
    "small desc_col wraps flags wider than the column to next line"
)
{
    eggs::test::print_options(stdout, /*ns:*/ {}, 18);
}
