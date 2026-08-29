// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/cli.hpp>

#include <span>
#include <string_view>
#include <vector>
#include <version>

TEST_CASE(parse_cli_list, "--list is consumed and sets run_options.list")
{
    char const* argv[] = {"--list"};
    auto const result = eggs::test::parse_cli(argv);
    REQUIRE(result.error.empty());

    CHECK(result.options.list == true);
    CHECK(result.unknown.empty());
}

TEST_CASE(parse_cli_ns_list, "--ns:list is consumed and sets run_options.list")
{
    char const* argv[] = {"--ns:list"};
    auto const result = eggs::test::parse_cli(argv, "ns");
    REQUIRE(result.error.empty());

    CHECK(result.options.list == true);
    CHECK(result.unknown.empty());
}

TEST_CASE(
    parse_cli_verbose, "--verbose is consumed and sets run_options.verbose"
)
{
    char const* argv[] = {"--verbose"};
    auto const result = eggs::test::parse_cli(argv);
    REQUIRE(result.error.empty());

    CHECK(result.options.verbose == true);
    CHECK(result.unknown.empty());
}

TEST_CASE(
    parse_cli_ns_verbose,
    "--ns:verbose is consumed and sets run_options.verbose"
)
{
    char const* argv[] = {"--ns:verbose"};
    auto const result = eggs::test::parse_cli(argv, "ns");
    REQUIRE(result.error.empty());

    CHECK(result.options.verbose == true);
    CHECK(result.unknown.empty());
}

TEST_CASE(parse_cli_run, "--run=foo is consumed and populates run_options.run")
{
    char const* argv[] = {"--run=foo"};
    auto const result = eggs::test::parse_cli(argv);
    REQUIRE(result.error.empty());

    CHECK(result.options.run == std::vector<std::string_view>{"foo"});
    CHECK(result.unknown.empty());
}

TEST_CASE(parse_cli_run_bare, "--run (no '=') reports an error")
{
    char const* argv[] = {"--run"};
    auto const result = eggs::test::parse_cli(argv);
    REQUIRE(!result.error.empty());

    CHECK(result.error == "missing value for --run");
}

TEST_CASE(parse_cli_run_missing_value, "--run= (empty value) reports an error")
{
    char const* argv[] = {"--run="};
    auto const result = eggs::test::parse_cli(argv);
    REQUIRE(!result.error.empty());

    CHECK(result.error == "missing value for --run");
}

TEST_CASE(
    parse_cli_ns_run, "--ns:run=foo is consumed and populates run_options.run"
)
{
    char const* argv[] = {"--ns:run=foo"};
    auto const result = eggs::test::parse_cli(argv, "ns");
    REQUIRE(result.error.empty());

    CHECK(result.options.run == std::vector<std::string_view>{"foo"});
    CHECK(result.unknown.empty());
}

TEST_CASE(parse_cli_ns_run_bare, "--ns:run (no '=') reports an error")
{
    char const* argv[] = {"--ns:run"};
    auto const result = eggs::test::parse_cli(argv, "ns");
    REQUIRE(!result.error.empty());

    CHECK(result.error == "missing value for --ns:run");
}

TEST_CASE(
    parse_cli_ns_run_missing_value, "--ns:run= (empty value) reports an error"
)
{
    char const* argv[] = {"--ns:run="};
    auto const result = eggs::test::parse_cli(argv, "ns");
    REQUIRE(!result.error.empty());

    CHECK(result.error == "missing value for --ns:run");
}

TEST_CASE(parse_cli_run_multiple, "--run=a --run=b are both consumed in order")
{
    char const* argv[] = {"--run=a", "--run=b"};
    auto const result = eggs::test::parse_cli(argv);
    REQUIRE(result.error.empty());

    CHECK(result.options.run == std::vector<std::string_view>{"a", "b"});
    CHECK(result.unknown.empty());
}

TEST_CASE(
    parse_cli_run_error_returns_defaults,
    "an error returns default options and an empty unknown list"
)
{
    char const* argv[] = {"--unknown", "--run"};
    auto const result = eggs::test::parse_cli(argv);
    REQUIRE(!result.error.empty());

    CHECK(result.error == "missing value for --run");

    CHECK(result.options.list == false);
    CHECK(result.options.run.empty());
    CHECK(result.options.verbose == false);
    CHECK(result.unknown.empty());
}

TEST_CASE(parse_cli_unknown, "unknown arg is returned in .unknown")
{
    char const* argv[] = {"--unknown"};
    auto const result = eggs::test::parse_cli(argv);
    REQUIRE(result.error.empty());

    CHECK(result.options.list == false);
    CHECK(result.options.run.empty());
    CHECK(result.options.verbose == false);

#if defined(__cpp_lib_format_ranges) && __cpp_lib_format_ranges >= 202207L
    CONTEXT("unknown={}", result.unknown);
#endif
    REQUIRE(result.unknown.size() == 1);
    CHECK(std::string_view{result.unknown[0]} == "--unknown");
}

TEST_CASE(parse_cli_help, "--help is not consumed by parse_cli")
{
    char const* argv[] = {"--help"};
    auto const result = eggs::test::parse_cli(argv);
    REQUIRE(result.error.empty());

    REQUIRE(result.unknown.size() == 1);
    CHECK(std::string_view{result.unknown[0]} == "--help");
}

TEST_CASE(parse_cli_mixed, "known flags consumed, unknowns preserved in order")
{
    char const* argv[] = {"--list", "--unknown-a", "--run=foo", "--unknown-b"};
    auto const result = eggs::test::parse_cli(argv);
    REQUIRE(result.error.empty());

    CHECK(result.options.list == true);
    CHECK(result.options.run == std::vector<std::string_view>{"foo"});

#if defined(__cpp_lib_format_ranges) && __cpp_lib_format_ranges >= 202207L
    CONTEXT("unknown={}", result.unknown);
#endif
    REQUIRE(result.unknown.size() == 2);
    CHECK(std::string_view{result.unknown[0]} == "--unknown-a");
    CHECK(std::string_view{result.unknown[1]} == "--unknown-b");
}

TEST_CASE(
    parse_cli_ns, "namespace prefix: known flags consumed, bare flags left"
)
{
    char const* argv[] = {
        "--eggs:list", "--unknown", "--eggs:run=foo", "--list"
    };
    auto const result = eggs::test::parse_cli(argv, "eggs");
    REQUIRE(result.error.empty());

    CHECK(result.options.list == true);
    CHECK(result.options.run == std::vector<std::string_view>{"foo"});

#if defined(__cpp_lib_format_ranges) && __cpp_lib_format_ranges >= 202207L
    CONTEXT("unknown={}", result.unknown);
#endif
    REQUIRE(result.unknown.size() == 2);
    CHECK(std::string_view{result.unknown[0]} == "--unknown");
    CHECK(std::string_view{result.unknown[1]} == "--list");
}

TEST_CASE(
    parse_cli_ns_unknown_stem,
    "--ns:unknown (unrecognized stem) is left in argv"
)
{
    char const* argv[] = {"--ns:unknown"};
    auto const result = eggs::test::parse_cli(argv, "ns");
    REQUIRE(result.error.empty());

    CHECK(result.options.list == false);
    CHECK(result.options.run.empty());
    REQUIRE(result.unknown.size() == 1);
    CHECK(std::string_view{result.unknown[0]} == "--ns:unknown");
}

TEST_CASE(
    parse_cli_ns_empty_stem, "--ns: (empty stem after colon) is left in argv"
)
{
    char const* argv[] = {"--ns:"};
    auto const result = eggs::test::parse_cli(argv, "ns");
    REQUIRE(result.error.empty());

    CHECK(result.options.list == false);
    REQUIRE(result.unknown.size() == 1);
    CHECK(std::string_view{result.unknown[0]} == "--ns:");
}

TEST_CASE(
    parse_cli_ns_bare,
    "--list and --run=foo without namespace prefix are left in argv"
)
{
    char const* argv[] = {"--list", "--run=foo"};
    auto const result = eggs::test::parse_cli(argv, "ns");
    REQUIRE(result.error.empty());

    CHECK(result.options.list == false);
    CHECK(result.options.run.empty());

#if defined(__cpp_lib_format_ranges) && __cpp_lib_format_ranges >= 202207L
    CONTEXT("unknown={}", result.unknown);
#endif
    REQUIRE(result.unknown.size() == 2);
    CHECK(std::string_view{result.unknown[0]} == "--list");
    CHECK(std::string_view{result.unknown[1]} == "--run=foo");
}

TEST_CASE(
    parse_cli_ns_no_colon, "--nslist (no colon separator) is left in argv"
)
{
    char const* argv[] = {"--nslist"};
    auto const result = eggs::test::parse_cli(argv, "ns");
    REQUIRE(result.error.empty());

    CHECK(result.options.list == false);
    REQUIRE(result.unknown.size() == 1);
    CHECK(std::string_view{result.unknown[0]} == "--nslist");
}

TEST_CASE(
    parse_cli_ns_exact_prefix, "--ns (arg is exactly the namespace prefix, no "
                               "colon or stem) is left in argv"
)
{
    char const* argv[] = {"--ns"};
    auto const result = eggs::test::parse_cli(argv, "ns");
    REQUIRE(result.error.empty());

    CHECK(result.options.list == false);
    REQUIRE(result.unknown.size() == 1);
    CHECK(std::string_view{result.unknown[0]} == "--ns");
}

TEST_CASE(
    parse_cli_ns_other_ns,
    "--xyz:list and --xyz:run=foo with a different namespace are left in argv"
)
{
    char const* argv[] = {"--xyz:list", "--xyz:run=foo"};
    auto const result = eggs::test::parse_cli(argv, "ns");
    REQUIRE(result.error.empty());

    CHECK(result.options.list == false);
    CHECK(result.options.run.empty());

#if defined(__cpp_lib_format_ranges) && __cpp_lib_format_ranges >= 202207L
    CONTEXT("unknown={}", result.unknown);
#endif
    REQUIRE(result.unknown.size() == 2);
    CHECK(std::string_view{result.unknown[0]} == "--xyz:list");
    CHECK(std::string_view{result.unknown[1]} == "--xyz:run=foo");
}

TEST_CASE(
    parse_cli_mutable_argv, "char*[] argv converts implicitly to "
                            "span<char const* const>"
)
{
    char flag[] = "--list";
    char* argv[] = {flag};
    auto const result = eggs::test::parse_cli(std::span(argv));
    REQUIRE(result.error.empty());

    CHECK(result.options.list == true);
    CHECK(result.unknown.empty());
}
