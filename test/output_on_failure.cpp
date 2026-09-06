// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <cstdio>

TEST_CASE(
    output_on_failure_pass,
    "a passing case that writes directly to stdout and stderr"
)
{
    std::printf("MARKER_STDOUT_PASS_CASE\n");
    std::fprintf(stderr, "MARKER_STDERR_PASS_CASE\n");

    CHECK(1 + 1 == 2);
}

TEST_CASE(
    output_on_failure_fail,
    "a failing case that writes directly to stdout and stderr"
)
{
    std::printf("MARKER_STDOUT_FAIL_CASE\n");
    std::fprintf(stderr, "MARKER_STDERR_FAIL_CASE\n");

    CHECK(1 + 1 == 3);
}
