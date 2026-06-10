// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <cstdlib>

#include "cli.hpp"

int main(int argc, char const* const argv[])
{
    auto [opts, action] = eggs::test::parse_args(argc, argv);

    using eggs::test::parse_action;
    switch (action) {
        case parse_action::exit_success: {
            return EXIT_SUCCESS;
        }
        case parse_action::exit_failure: {
            return EXIT_FAILURE;
        }
        case parse_action::run:
            break;
    }

    return eggs::test::run(opts);
}
