// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test/run.hpp>

#include <cstddef>
#include <cstdio>
#include <initializer_list>
#include <span>
#include <string>
#include <string_view>
#include <vector>

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
    std::FILE* out, std::string_view ns = {}, std::size_t desc_col = 29U
);

// The parsed options and unknown arguments, or an error message.
struct parse_result
{
    run_options options;
    std::vector<char const*> unknown;
    std::string error;
};

// Parse known test-runner flags from args.
// Unrecognized tokens are returned in .unknown.
// If ns is non-empty, only flags of the form --<ns>:<flag> are processed.
// On an invalid value (e.g. --run= with no value), parsing stops and an
// error message describing the problem is returned.
parse_result
parse_cli(std::span<char const* const> args, std::string_view ns = {});

} // namespace eggs::test
