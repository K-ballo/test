// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/detail/print.hpp>

#include <cstdio>
#include <string>
#include <string_view>

#include "opts.hpp"

namespace eggs::test {

// ── Public interface ─────────────────────────────────────────────────────────

void print_options(std::size_t desc_col)
{
    for (opt_spec const& opt : k_opts) {
        // FIXME(C++26): desc.empty()
        if (opt.desc.size() == 0) continue;

        auto disp = std::string(opt.flag) += opt.value;

        bool first = true;
        for (auto const& line : opt.desc) {
            if (first) {
                // Print left column + description.
                if (2 + disp.size() <= desc_col) {
                    // Fits on one line. Pad left column to (desc_col - 2)
                    detail::println(
                        stdout, "  {:<{}} {}", disp, desc_col - 2, line
                    );
                } else {
                    // Too long. Print option name, wrap to next line, pad description
                    detail::println(stdout, "  {}", disp);
                    detail::println(stdout, "{:>{}} {}", "", desc_col, line);
                }
                first = false;
            } else {
                // Subsequent lines are always padded to match the description column
                detail::println(stdout, "{:>{}} {}", "", desc_col, line);
            }
        }
    }
}

run_options parse_cli(int& argc, char const* argv[])
{
    run_options opts;

    int outc = 1;
    for (int i = 1; i < argc; ++i) {
        std::string_view const arg = argv[i];

        if (arg == "--list") {
            opts.list = true;
        } else if (arg.starts_with("--run=")) {
            opts.run.push_back(arg.substr(6));
        } else {
            argv[outc++] = argv[i];
        }
    }

    argc = outc;
    return opts;
}

} // namespace eggs::test
