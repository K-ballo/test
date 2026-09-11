// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <chrono>
#include <thread>

TEST_CASE(
    duration_seconds,
    "a test case running past 1 second reports its duration in seconds"
)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1050));
    CHECK(true);
}
