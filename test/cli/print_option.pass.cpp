// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/cli.hpp>

#include <cstdio> // IWYU pragma: keep (stdout)

TEST_CASE(
    print_option_fits, "disp and first desc line share a row when disp fits"
)
{
    eggs::test::print_option(
        stdout, "-h, --help", {"print this help message and exit"}, 29
    );
}

TEST_CASE(
    print_option_wraps, "disp wider than desc_col wraps desc to the next line"
)
{
    eggs::test::print_option(
        stdout, "--very-long-flag-name", {"description text"}, 10
    );
}

TEST_CASE(
    print_option_multi_desc,
    "desc lines past the first are indented to desc_col"
)
{
    eggs::test::print_option(stdout, "flag", {"first line", "second line"}, 20);
}
