// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/cli.hpp>

#include <cstdio>

#include "../support.hpp"

TEST_CASE(
    print_option_fits, "disp and first desc line share a row when disp fits"
)
{
    auto const out = eggs::test_support::capture([](std::FILE* f) {
        eggs::test::print_option(
            f, "-h, --help", {"print this help message and exit"}
        );
    });

    CHECK(
        out ==
        "  -h, --help                  print this help message and exit\n"
    );
}

TEST_CASE(
    print_option_wraps, "disp wider than desc_col wraps desc to the next line"
)
{
    auto const out = eggs::test_support::capture([](std::FILE* f) {
        eggs::test::print_option(
            f, "--very-long-flag-name", {"description text"}, 10U
        );
    });

    CHECK(
        out == "  --very-long-flag-name\n"
               "           description text\n"
    );
}

TEST_CASE(
    print_option_multi_desc,
    "desc lines past the first are indented to desc_col"
)
{
    auto const out = eggs::test_support::capture([](std::FILE* f) {
        eggs::test::print_option(f, "flag", {"first line", "second line"}, 20U);
    });

    CHECK(
        out == "  flag               first line\n"
               "                     second line\n"
    );
}

TEST_CASE(print_option_empty_desc, "empty desc prints only disp")
{
    auto const out = eggs::test_support::capture([](std::FILE* f) {
        eggs::test::print_option(f, "--flag", {});
    });

    CHECK(out == "  --flag\n");
}
