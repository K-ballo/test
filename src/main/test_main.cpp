// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/detail/print.hpp>

#include <charconv>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <optional>
#include <string_view>

namespace eggs::test {
namespace {

std::optional<int> parse_int(std::string_view sv) noexcept
{
    int value = 0;
    auto [end, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
    if (ec != std::errc{} || end != sv.data() + sv.size()) return std::nullopt;
    return value;
}

void print_help(std::filesystem::path const& usage)
{
    static constexpr std::size_t k_desc_col = 29u;

    detail::println(
        stdout, "Usage: {} [options]\n\nOptions:",
        usage.empty() ? "<test-executable>" : usage.string()
    );
    detail::println(
        stdout, "  {:<{}} {}", "-h, --help", k_desc_col - 2,
        "print this help message and exit"
    );
    test::print_options(k_desc_col);
}

int main(int argc, char const* argv[], run_options const& opts)
{
    std::optional<int> exit_code;

    for (int i = 1; i < argc; ++i) {
        std::string_view const arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            std::filesystem::path const path{argv[0] ? argv[0] : ""};

            print_help(path.filename());
            return EXIT_SUCCESS;
        }

        if (arg.starts_with("--exit-code=")) {
            std::string_view const val = arg.substr(12);
            exit_code = parse_int(val);
            if (!exit_code) {
                eggs::test::detail::println(
                    stderr, "error: invalid exit code '{}'", val
                );
                return EXIT_FAILURE;
            }
            continue;
        }

        detail::println(stderr, "error: unknown argument '{}'", arg);
        return EXIT_FAILURE;
    }

    int const r = test::run(opts);
    return exit_code.value_or(r);
}

} // namespace
} // namespace eggs::test

int main(int argc, char const* argv[])
{
    auto const opts = eggs::test::parse_cli(argc, argv);
    return eggs::test::main(argc, argv, opts);
}
