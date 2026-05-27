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
#include <string>
#include <string_view>

namespace eggs::test {
namespace {

// ── Option table ─────────────────────────────────────────────────────────────
//
// flag    — the flag text
// also    — extra form shown alongside in --help:
//             short alias (-x)   → listed before the flag: "-v, --verbose"
// desc    — --help right-column text; empty = hidden from --help (alias or internal flag)

struct opt_spec
{
    std::string_view flag;
    std::string_view also;
    std::string_view desc;
};

constexpr opt_spec k_opts[] = {
    // {flag, also, desc}
    {"--list", {}, "list all registered test case names and exit"},
    {"--help", "-h", "show this help"},
    {"-h", {}, {}},
};

// ── Help printing ────────────────────────────────────────────────────────────

// description starts at this column (0-based)
static constexpr int k_desc_col = 29;

void print_help()
{
    detail::print(
        stdout, "Usage: <test-executable> [options]\n"
                "\n"
                "Options:\n"
    );

    for (opt_spec const& opt : k_opts) {
        if (opt.desc.empty()) continue;

        // Build left-column display string from flag + also.
        std::string disp;
        if (opt.also.empty()) {
            disp = opt.flag;
        } else {
            // "-h, --help"
            disp = std::format("{}, {}", opt.also, opt.flag);
        }

        // Print left column + description.
        if (2 + static_cast<int>(disp.size()) <= k_desc_col) {
            detail::print(
                stdout, "  {:<{}}{}\n", disp, k_desc_col - 2, opt.desc
            );
        } else {
            detail::print(
                stdout, "  {}\n{:>{}}{}\n", disp, "", k_desc_col, opt.desc
            );
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
            auto const& all_cases = detail::registry::cases();
            for (auto const& e : all_cases) {
                detail::println(stdout, "{}", e.name);
            }

            pr.action = parse_action::exit_success;
            return pr;
        } else {
            detail::println(stderr, "error: unknown argument '{}'", arg);

            pr.action = parse_action::exit_failure;
            return pr;
        }
    }

    return pr;
}

} // namespace eggs::test
