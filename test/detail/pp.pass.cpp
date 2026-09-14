// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test/detail/pp.hpp>

#include "support.hpp"

#define EGGS_TEST_DETAIL_PP_TEST_FOO foo
#define EGGS_TEST_DETAIL_PP_TEST_BAR bar

// EGGS_TEST_PP_CAT expands its arguments before pasting them together: a
// plain `a##b` would instead paste the macro names themselves together.
#define foobar 42
#if EGGS_TEST_PP_CAT(                                          \
    EGGS_TEST_DETAIL_PP_TEST_FOO, EGGS_TEST_DETAIL_PP_TEST_BAR \
) != 42
#    error "EGGS_TEST_PP_CAT did not expand its arguments before pasting"
#endif
#undef foobar

// Non-macro arguments still paste directly, same as a plain ## would.
#define baz1 7
#if EGGS_TEST_PP_CAT(baz, 1) != 7
#    error "EGGS_TEST_PP_CAT did not paste non-macro arguments"
#endif
#undef baz1

int main(int argc, char**)
{
    // argc is always 1, but not a compile-time constant, unlike the checks
    // above: avoids MSVC's C4127 warning.
    EGGS_TEST_DETAIL_CHECK(argc == 1);

    return eggs::test_support::report();
}
