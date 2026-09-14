// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test/detail/warning.hpp>

#include <string_view>
#include <vector>

namespace eggs::test {

EGGS_TEST_WARNING_NO_PADDED_PUSH

// Options passed to run()
struct run_options
{
    // ordered test case names; empty = run all
    std::vector<std::string_view> run;
    // list matching test case names instead of running them
    bool list = false;
    // report successful assertions
    bool verbose = false;
};

EGGS_TEST_WARNING_NO_PADDED_POP

// Public entry point - call this from main().
int run(run_options opts = {});

} // namespace eggs::test
