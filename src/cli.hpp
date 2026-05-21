// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test.hpp>

#include <cstdint>

namespace eggs::test {

enum class parse_action : std::uint8_t
{
    exit_success,
    exit_failure,
    run,
};

struct parse_result
{
    run_options opts;
    parse_action action = parse_action::exit_success;
};

parse_result parse_args(int argc, char const* const argv[]);

} // namespace eggs::test
