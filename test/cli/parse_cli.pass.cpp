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

TEST_CASE(parse_cli_ns_list, "--ns:list is consumed and sets opts.list")
{
    char const* argv[] = {"prog", "--ns:list"};
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv, "ns");
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

TEST_CASE(parse_cli_ns_run, "--ns:run=foo is consumed and populates opts.run")
{
    char const* argv[] = {"prog", "--ns:run=foo"};
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv, "ns");
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

TEST_CASE(
    parse_cli_ns, "namespace prefix: known flags consumed, bare flags left"
)
{
    char const* argv[] = {
        "prog", "--eggs:list", "--unknown", "--eggs:run=foo", "--list"
    };
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv, "eggs");
    CHECK(opts.list == true);
    CHECK(opts.run == std::vector<std::string_view>{"foo"});
    CHECK(argc == 3);
    CHECK(std::string_view{argv[1]} == "--unknown");
    CHECK(std::string_view{argv[2]} == "--list");
}

TEST_CASE(
    parse_cli_ns_unknown_stem,
    "--ns:unknown (unrecognized stem) is left in argv"
)
{
    char const* argv[] = {"prog", "--ns:unknown"};
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv, "ns");
    CHECK(opts.list == false);
    CHECK(opts.run.empty());
    CHECK(argc == 2);
    CHECK(std::string_view{argv[1]} == "--ns:unknown");
}

TEST_CASE(
    parse_cli_ns_empty_stem, "--ns: (empty stem after colon) is left in argv"
)
{
    char const* argv[] = {"prog", "--ns:"};
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv, "ns");
    CHECK(opts.list == false);
    CHECK(argc == 2);
    CHECK(std::string_view{argv[1]} == "--ns:");
}

TEST_CASE(
    parse_cli_ns_bare,
    "--list and --run=foo without namespace prefix are left in argv"
)
{
    char const* argv[] = {"prog", "--list", "--run=foo"};
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv, "ns");
    CHECK(opts.list == false);
    CHECK(opts.run.empty());
    CHECK(argc == 3);
    CHECK(std::string_view{argv[1]} == "--list");
    CHECK(std::string_view{argv[2]} == "--run=foo");
}

TEST_CASE(
    parse_cli_ns_no_colon, "--nslist (no colon separator) is left in argv"
)
{
    char const* argv[] = {"prog", "--nslist"};
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv, "ns");
    CHECK(opts.list == false);
    CHECK(argc == 2);
    CHECK(std::string_view{argv[1]} == "--nslist");
}

TEST_CASE(
    parse_cli_ns_exact_prefix, "--ns (arg is exactly the namespace prefix, no "
                               "colon or stem) is left in argv"
)
{
    char const* argv[] = {"prog", "--ns"};
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv, "ns");
    CHECK(opts.list == false);
    CHECK(argc == 2);
    CHECK(std::string_view{argv[1]} == "--ns");
}

TEST_CASE(
    parse_cli_ns_other_ns,
    "--xyz:list and --xyz:run=foo with a different namespace are left in argv"
)
{
    char const* argv[] = {"prog", "--xyz:list", "--xyz:run=foo"};
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv, "ns");
    CHECK(opts.list == false);
    CHECK(opts.run.empty());
    CHECK(argc == 3);
    CHECK(std::string_view{argv[1]} == "--xyz:list");
    CHECK(std::string_view{argv[2]} == "--xyz:run=foo");
}

TEST_CASE(parse_cli_color_auto, "--color=auto is consumed and sets opts.color")
{
    char const* argv[] = {"prog", "--color=auto"};
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv);
    CHECK(opts.color == eggs::test::color_when::auto_);
    CHECK(argc == 1);
}

TEST_CASE(
    parse_cli_color_always, "--color=always is consumed and sets opts.color"
)
{
    char const* argv[] = {"prog", "--color=always"};
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv);
    CHECK(opts.color == eggs::test::color_when::always);
    CHECK(argc == 1);
}

TEST_CASE(
    parse_cli_color_never, "--color=never is consumed and sets opts.color"
)
{
    char const* argv[] = {"prog", "--color=never"};
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv);
    CHECK(opts.color == eggs::test::color_when::never);
    CHECK(argc == 1);
}

TEST_CASE(
    parse_cli_ns_color, "--ns:color=never is consumed and sets opts.color"
)
{
    char const* argv[] = {"prog", "--ns:color=never"};
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv, "ns");
    CHECK(opts.color == eggs::test::color_when::never);
    CHECK(argc == 1);
}

TEST_CASE(
    parse_cli_color_invalid,
    "--color=bogus (unrecognized value) is left in argv"
)
{
    char const* argv[] = {"prog", "--color=bogus"};
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv);
    CHECK(opts.color == eggs::test::color_when::auto_);
    CHECK(argc == 2);
    CHECK(std::string_view{argv[1]} == "--color=bogus");
}

TEST_CASE(parse_cli_mutable_argv, "non-const char* overload forwards correctly")
{
    char prog[] = "prog";
    char flag[] = "--list";
    char* argv[] = {prog, flag};
    int argc = countof(argv);
    auto opts = eggs::test::parse_cli(argc, argv);
    CHECK(opts.list == true);
    CHECK(argc == 1);
}
