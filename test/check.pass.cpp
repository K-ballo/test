// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <type_traits>

TEST_CASE(check_bool_true, "CHECK passes when expression is true")
{
    CHECK(true);
    CHECK(1 == 1);
    CHECK(1 + 1 == 2);
}

TEST_CASE(check_pointer_nonnull, "CHECK works with pointer comparisons")
{
    int x = 42;
    int* p = &x;
    CHECK(p != nullptr);
    CHECK(*p == 42);
}

TEST_CASE(
    check_explicit_bool,
    "CHECK works with types that have explicit operator bool"
)
{
    struct flag
    {
        explicit operator bool() const { return true; }
    };

    CHECK(flag{});
}

TEST_CASE(check_expression, "CHECK's result can be captured")
{
    if (auto r = CHECK(1 + 1 == 2)) {
        static_assert(std::is_same_v<decltype(r), bool>);

        CHECK(r == true);
    } else {
        CHECK(false); // must not be reached
    }
}
