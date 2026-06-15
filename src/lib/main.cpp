// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/cli.hpp>
#include <eggs/test/detail/print.hpp>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string_view>

namespace eggs::test {
namespace {

void print_help(std::FILE* out, std::string_view const usage)
{
    static constexpr std::size_t k_desc_col = 29u;

    detail::println(
        out, "Usage: {} [options]\n\nOptions:",
        usage.empty() ? "<test-executable>" : usage
    );
    detail::print_option(
        out, "-h, --help", {"print this help message and exit"}, k_desc_col
    );
    test::print_options(out, /*ns:*/ {}, k_desc_col);
}

} // namespace

int main(int argc, char const* argv[])
{
    run_options const opts = parse_cli(argc, argv);

    for (int i = 1; i < argc; ++i) {
        std::string_view const arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            std::filesystem::path const path{argv[0] ? argv[0] : ""};
            auto const& usage = path.filename().string();

            test::print_help(stdout, usage);
            return EXIT_SUCCESS;
        }

        detail::println(stderr, "error: unknown argument '{}'", arg);
        return EXIT_FAILURE;
    }

    return test::run(opts);
}

} // namespace eggs::test
