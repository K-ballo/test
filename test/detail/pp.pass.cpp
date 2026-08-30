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

int main()
{
    // EGGS_TEST_PP_CAT expands its arguments before pasting them together: a
    // plain `a##b` would instead paste the macro names themselves, producing
    // the literal token
    // EGGS_TEST_DETAIL_PP_TEST_FOOEGGS_TEST_DETAIL_PP_TEST_BAR.
    int const EGGS_TEST_PP_CAT(
        EGGS_TEST_DETAIL_PP_TEST_FOO, EGGS_TEST_DETAIL_PP_TEST_BAR
    ) = 42;
    EGGS_TEST_DETAIL_ASSERT(foobar == 42);

    // Non-macro arguments still paste directly, same as a plain ## would.
    int const EGGS_TEST_PP_CAT(baz, 1) = 7;
    EGGS_TEST_DETAIL_ASSERT(baz1 == 7);

    return 0;
}
