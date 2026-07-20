// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#include <concepts>

#if defined(REGISTER_P_NONLITERAL_INSTANCE_COMPILE_FAIL)

TEST_CASE(nonliteral_instance, "zero-parameter parameterized test")
{
    CHECK(1 + 1 == 2);
}

// #name_ "/" instance_ relies on adjacent string-literal concatenation, so
// instance_ must be a string-literal token, not a variable holding a string.
char const* const instance = "first";
REGISTER_P(nonliteral_instance, instance);

#elif defined(REGISTER_P_INTEGRAL_AUTO_COMPILE_FAIL)

TEST_CASE(
    integral_auto_reject,
    "std::integral auto only instantiates for integral types",
    std::integral auto n
)
{
    CHECK((n | 0) == n);
}

// std::integral auto rejects arguments that are not integral, such as double.
REGISTER_P(integral_auto_reject, "double", 3.14);

#elif defined(REGISTER_P_WRONG_ARITY_COMPILE_FAIL)

TEST_CASE(wrong_arity, "addition is commutative", int const& a, int const& b)
{
    CHECK(a + b == b + a);
}

// REGISTER_P's arguments are forwarded directly to run(); passing fewer
// arguments than run() requires is a call-site arity mismatch.
REGISTER_P(wrong_arity, "missing", 1);

#endif
