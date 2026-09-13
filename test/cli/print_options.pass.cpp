// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/cli.hpp>

#include <cstdio>
#include <string>

#include "../support.hpp"

TEST_CASE(
    print_options_bare, "print_options without namespace lists bare flags"
)
{
    auto const out = eggs::test_support::capture([](std::FILE* f) {
        eggs::test::print_options(f);
    });

    auto const list_pos = out.find("--list");
    REQUIRE(list_pos != std::string::npos);
    CHECK(out.find("list selected test case", list_pos) != std::string::npos);

    CHECK(out.find("--run=<test_case>") != std::string::npos);
}

TEST_CASE(
    print_options_ns, "print_options with ns prefixes flags with --<ns>:<flag>"
)
{
    auto const out = eggs::test_support::capture([](std::FILE* f) {
        eggs::test::print_options(f, "ns");
    });

    auto const list_pos = out.find("--ns:list");
    REQUIRE(list_pos != std::string::npos);
    CHECK(out.find("list selected test case", list_pos) != std::string::npos);

    CHECK(out.find("--ns:run=<test_case>") != std::string::npos);

    CHECK(out.find("  --list") == std::string::npos);
}

TEST_CASE(
    print_options_desc_col,
    "small desc_col wraps flags wider than the column to next line"
)
{
    auto const out = eggs::test_support::capture([](std::FILE* f) {
        eggs::test::print_options(f, /*ns:*/ {}, 18U);
    });

    CHECK(out.find("--run=<test_case>\n") != std::string::npos);
    CHECK(out.find("--run=<test_case> ") == std::string::npos);
}
