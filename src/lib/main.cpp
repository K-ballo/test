// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/detail/print.hpp>

#include <cstdlib>
#include <filesystem>
#include <string_view>

namespace eggs::test {

int main(int argc, char const* argv[])
{
    run_options const opts = parse_cli(argc, argv);

    for (int i = 1; i < argc; ++i) {
        std::string_view const arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            std::filesystem::path const path{argv[0] ? argv[0] : ""};
            auto const& usage = path.filename().string();

            print_help(usage);
            return EXIT_SUCCESS;
        }

        detail::println(stderr, "error: unknown argument '{}'", arg);
        return EXIT_FAILURE;
    }

    return run(opts);
}

} // namespace eggs::test
