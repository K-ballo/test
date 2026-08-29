// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/cli.hpp>
#include <eggs/test/detail/print.hpp>

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <format>
#include <initializer_list>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

#include "opts.hpp"

namespace eggs::test {
namespace detail {

void print_option(
    std::FILE* out, std::string_view const disp,
    std::initializer_list<std::string_view> const desc,
    std::size_t const desc_col
)
{
    // FIXME(C++26): !desc.empty()
    assert(desc.size() != 0);

    auto it = desc.begin();

    // First line: print display alongside description if it fits, else wrap.
    if (2 + disp.size() <= desc_col && it != desc.end()) {
        detail::println(out, "  {:<{}} {}", disp, desc_col - 2, *it);
        ++it;
    } else {
        detail::println(out, "  {}", disp);
    }

    // Remaining lines: indent to description column.
    for (; it != desc.end(); ++it) {
        detail::println(out, "{:>{}} {}", "", desc_col, *it);
    }
}

} // namespace detail

void print_options(
    std::FILE* out, std::string_view const ns, std::size_t const desc_col
)
{
    for (opt_spec const& opt : k_opts) {
        // --[ns:]flag[=<value>]
        auto const disp = std::format(
            "--{}{}{}{}", ns, ns.empty() ? "" : ":", opt.flag, opt.value
        );

        detail::print_option(out, disp, opt.desc, desc_col);
    }
}

namespace {

std::string_view extract_stem(std::string_view arg, std::string_view const ns)
{
    // Remove '--'
    if (!arg.starts_with("--")) return {};
    arg.remove_prefix(2);

    if (!ns.empty()) {
        // Remove '<ns>:'
        if (!arg.starts_with(ns)) return {};
        if (arg.size() <= ns.size() || arg[ns.size()] != ':') return {};
        arg.remove_prefix(ns.size() + 1);
    }

    return arg;
}

} // namespace

parse_result
parse_cli(std::span<char const* const> const args, std::string_view ns)
{
    run_options opts;

    std::vector<char const*> unknown;
    unknown.reserve(args.size());

    for (char const* const arg : args) {
        auto const stem = extract_stem(arg, ns);

        if (stem == "list") {
            opts.list = true;
        } else if (stem.starts_with("run")) {
            auto const value =
                stem == "run" ? std::string_view{} : stem.substr(4);
            if (!value.empty()) {
                opts.run.push_back(value);
            } else {
                return parse_result{
                    .options = {},
                    .unknown = {},
                    .error = std::format(
                        "missing value for --{}{}run", ns, ns.empty() ? "" : ":"
                    ),
                };
            }
        } else if (stem == "verbose") {
            opts.verbose = true;
        } else {
            unknown.push_back(arg);
        }
    }

    return parse_result{
        .options = std::move(opts),
        .unknown = std::move(unknown),
        .error = {},
    };
}

} // namespace eggs::test
