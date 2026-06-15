// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test.hpp>

#include <cstddef>
#include <cstdio>
#include <initializer_list>
#include <string_view>

namespace eggs::test {
namespace detail {

void print_option(
    std::FILE* out, std::string_view const disp,
    std::initializer_list<std::string_view> const desc,
    std::size_t const desc_col
);

} // namespace detail

// Print the built-in options table to stdout, aligning descriptions at desc_col.
// If ns is non-empty, flags are shown in --<ns>:<flag> form.
void print_options(
    std::FILE* out, std::string_view ns = {}, std::size_t desc_col = 29u
);

// Parse known test-runner flags from argv, removing them in-place.
// Unrecognized tokens remain in argv[1..argc-1]; argc is updated accordingly.
// If ns is non-empty, only flags of the form --<ns>:<flag> are processed.
run_options parse_cli(int& argc, char const* argv[], std::string_view ns = {});

inline run_options parse_cli(int& argc, char* argv[], std::string_view ns = {})
{
    return parse_cli(argc, const_cast<char const**>(argv), ns);
}

} // namespace eggs::test
