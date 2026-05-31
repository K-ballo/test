// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "cli.hpp"

#include <eggs/test/detail/print.hpp>

#include <cstdio>
#include <format>
#include <initializer_list>
#include <string>
#include <string_view>

namespace eggs::test {
namespace {

// ── Option table ─────────────────────────────────────────────────────────────
//
// flag    — the flag text
// value   - the flag value
// also    — extra form shown alongside in --help:
//             short alias (-x)   → listed before the flag: "-v, --verbose"
// desc    — --help right-column text; empty = hidden from --help (alias or internal flag)

struct opt_spec
{
    std::string_view flag;
    std::string_view value;
    std::string_view also;
    std::initializer_list<std::string_view> desc;
};

constexpr opt_spec k_opts[] = {
    // {flag, value, also, desc}
    {"--list",
     {},
     {},
     {
         "list selected test case names and exit;",
         "respects --run filters",
     }},
    {"--run=",
     "<test_case>",
     {},
     {
         "run only the specified test cases (repeatable).",
         "all test cases will run if omitted",
     }},
};

// ── Help printing ────────────────────────────────────────────────────────────

// description starts at this column (0-based)
static constexpr std::size_t k_desc_col = 29u;

void print_help()
{
    detail::print(
        stdout, "Usage: <test-executable> [options]\n"
                "\n"
                "Options:\n"
    );

    for (opt_spec const& opt : k_opts) {
        if (opt.desc.size() == 0) continue;

        // Build left-column display string from flag + value + also.
        auto disp = std::string(opt.flag) += opt.value;
        if (!opt.also.empty()) {
            // "-h, --help"
            disp = std::format("{}, {}", opt.also, disp);
        }

        bool first = true;
        for (auto const& line : opt.desc) {
            if (first) {
                // Print left column + description.
                if (2 + disp.size() <= k_desc_col) {
                    // Fits on one line. Pad left column to (k_desc_col - 2)
                    detail::println(
                        stdout, "  {:<{}} {}", disp, k_desc_col - 2, line
                    );
                } else {
                    // Too long. Print option name, wrap to next line, pad description
                    detail::println(stdout, "  {}", disp);
                    detail::println(stdout, "{:>{}} {}", "", k_desc_col, line);
                }
                first = false;
            } else {
                // Subsequent lines are always padded to match the description column
                detail::println(stdout, "{:>{}} {}", "", k_desc_col, line);
            }
        }
    }
}

} // namespace

// ── Public interface ─────────────────────────────────────────────────────────

parse_result parse_args(int argc, char const* const argv[])
{
    parse_result pr;
    pr.action = parse_action::run;

    for (int i = 1; i < argc; ++i) {
        std::string_view const arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            print_help();

            pr.action = parse_action::exit_success;
            return pr;
        } else if (arg == "--list") {
            pr.opts.list = true;
        } else if (arg.starts_with("--run=")) {
            auto const name = arg.substr(6);

            pr.opts.run.push_back(name);
        } else {
            detail::println(stderr, "error: unknown argument '{}'", arg);

            pr.action = parse_action::exit_failure;
            return pr;
        }
    }

    return pr;
}

} // namespace eggs::test
