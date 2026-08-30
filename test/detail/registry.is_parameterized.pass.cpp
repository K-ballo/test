// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test/detail/registry.hpp>

#include "support.hpp"

namespace {

struct no_args
{
    static void run() {}
};

struct one_required_arg
{
    static void run(int) {}
};

struct two_required_args
{
    static void run(int, int) {}
};

struct one_defaulted_arg
{
    static void run(int = 0) {}
};

} // namespace

int main()
{
    // A run() callable with no arguments is not parameterized.
    EGGS_TEST_DETAIL_ASSERT(!eggs::test::detail::is_parameterized<no_args>);

    // A run() requiring one or more arguments is parameterized.
    EGGS_TEST_DETAIL_ASSERT(
        eggs::test::detail::is_parameterized<one_required_arg>
    );
    EGGS_TEST_DETAIL_ASSERT(
        eggs::test::detail::is_parameterized<two_required_args>
    );

    // A run() with a defaulted argument is callable with none, so it is not
    // parameterized.
    EGGS_TEST_DETAIL_ASSERT(
        !eggs::test::detail::is_parameterized<one_defaulted_arg>
    );

    return 0;
}
