// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/detail/registry.hpp>

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

TEST_CASE(
    is_parameterized_false_for_no_arg_run,
    "a run() callable with no arguments is not parameterized"
)
{
    CHECK(!eggs::test::detail::is_parameterized<no_args>);
}

TEST_CASE(
    is_parameterized_true_for_one_required_arg,
    "a run() requiring one argument is parameterized"
)
{
    CHECK(eggs::test::detail::is_parameterized<one_required_arg>);
}

TEST_CASE(
    is_parameterized_true_for_multiple_required_args,
    "a run() requiring multiple arguments is parameterized"
)
{
    CHECK(eggs::test::detail::is_parameterized<two_required_args>);
}

TEST_CASE(
    is_parameterized_false_for_defaulted_arg,
    "a run() with a defaulted argument is callable with none, so it is not "
    "parameterized"
)
{
    CHECK(!eggs::test::detail::is_parameterized<one_defaulted_arg>);
}
