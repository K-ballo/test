// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

// Exercises CONTEXT: an active frame must appear alongside every diagnostic
// (PASSED here, exercised with --verbose) produced while still in scope.

TEST_CASE(context_labeled_pass, "CONTEXT(expr) shows \"expr -> value\"")
{
    int id = 42;
    CONTEXT(id);
    CHECK(id == 42);
}

TEST_CASE(
    context_literal_pass, "CONTEXT(\"literal\") omits the \"expr -> \" prefix"
)
{
    CONTEXT("processing item");
    CHECK(1 + 1 == 2);
}

TEST_CASE(context_nested_pass, "nested CONTEXT frames show innermost first")
{
    CONTEXT("outer");
    {
        CONTEXT("inner");
        CHECK(1 + 1 == 2);
    }
}

TEST_CASE(
    context_scope_ends_pass,
    "a CONTEXT frame stops applying once its scope ends"
)
{
    {
        CONTEXT("scoped out");
    }
    CHECK(1 + 1 == 2);
}

TEST_CASE(
    context_snapshot_pass,
    "CONTEXT captures its value at the point it is called, not later"
)
{
    int counter = 0;
    CONTEXT(counter);
    counter = 7;
    CHECK(counter == 7);
}

TEST_CASE(
    context_numeric_expression_pass,
    "CONTEXT(expr) starting with a digit but not itself a literal still shows "
    "its label"
)
{
    int x = 3;
    CONTEXT(5 + x);
    CHECK(1 + 1 == 2);
}

TEST_CASE(
    context_fmt_pass,
    "CONTEXT(fmt, args...) formats with the given format string and no label"
)
{
    int i = 1;
    int j = 2;
    CONTEXT("i={} j={}", i, j);
    CHECK(1 + 1 == 2);
}

namespace {

template <typename T, typename U>
T first_of(T t, U)
{
    return t;
}

} // namespace

TEST_CASE(
    context_template_comma_pass,
    "CONTEXT(expr) where expr has an unparenthesized template-argument comma "
    "is still treated as one argument, not two"
)
{
    CONTEXT(first_of<int, double>(5, 1.5));
    CHECK(1 + 1 == 2);
}
