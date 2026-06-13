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
#include <string_view>

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

void print_options(std::FILE* out, std::size_t const desc_col)
{
    for (opt_spec const& opt : k_opts) {
        // --flag[=<value>]
        auto const disp = std::format("--{}{}", opt.flag, opt.value);

        detail::print_option(out, disp, opt.desc, desc_col);
    }
}

run_options parse_cli(int& argc, char const* argv[])
{
    int outc = 1;
    run_options opts;

    for (int i = 1; i < argc; ++i) {
        std::string_view stem(argv[i]);
        if (stem.starts_with("--")) {
            stem.remove_prefix(2);
        }

        if (stem == "list") {
            opts.list = true;
        } else if (stem.starts_with("run=")) {
            opts.run.push_back(stem.substr(4));
        } else {
            argv[outc++] = argv[i];
        }
    }

    argc = outc;
    return opts;
}

} // namespace eggs::test
