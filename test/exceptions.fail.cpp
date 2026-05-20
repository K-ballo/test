// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <stdexcept>

// This executable exits 1.

TEST_CASE(
    leaked_std_exception,
    "std::exception leaked from a test case is caught and reported"
)
{
    throw std::runtime_error{"deliberate failure"};
}

TEST_CASE(
    leaked_unknown_exception,
    "unknown exception leaked from a test case is caught and reported"
)
{
    throw 42;
}
