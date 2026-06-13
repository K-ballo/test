// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/cli.hpp>

#include <cstddef>
#include <string_view>
#include <vector>

template <typename T, std::size_t N>
constexpr int countof(T (&)[N]) noexcept
{
    return static_cast<int>(N);
}

TEST_CASE(parse_cli_list, "--list is consumed and sets opts.list")
{
    char const* argv[] = {"prog", "--list"};
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv);
    CHECK(opts.list == true);
    CHECK(argc == 1);
}

TEST_CASE(parse_cli_run, "--run=foo is consumed and populates opts.run")
{
    char const* argv[] = {"prog", "--run=foo"};
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv);
    CHECK(opts.run == std::vector<std::string_view>{"foo"});
    CHECK(argc == 1);
}

TEST_CASE(parse_cli_run_multiple, "--run=a --run=b are both consumed in order")
{
    char const* argv[] = {"prog", "--run=a", "--run=b"};
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv);
    CHECK(opts.run == std::vector<std::string_view>{"a", "b"});
    CHECK(argc == 1);
}

TEST_CASE(parse_cli_unknown, "unknown arg is left in argv")
{
    char const* argv[] = {"prog", "--unknown"};
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv);
    CHECK(opts.list == false);
    CHECK(opts.run.empty());
    CHECK(argc == 2);
    CHECK(std::string_view{argv[1]} == "--unknown");
}

TEST_CASE(parse_cli_help, "--help is not consumed by parse_cli")
{
    char const* argv[] = {"prog", "--help"};
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv);
    CHECK(argc == 2);
    CHECK(std::string_view{argv[1]} == "--help");
}

TEST_CASE(parse_cli_mixed, "known flags consumed, unknowns preserved in order")
{
    char const* argv[] = {
        "prog", "--list", "--unknown-a", "--run=foo", "--unknown-b"
    };
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv);
    CHECK(opts.list == true);
    CHECK(opts.run == std::vector<std::string_view>{"foo"});
    CHECK(argc == 3);
    CHECK(std::string_view{argv[1]} == "--unknown-a");
    CHECK(std::string_view{argv[2]} == "--unknown-b");
}
