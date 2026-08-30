// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test/detail/checks.hpp>
#include <eggs/test/detail/run_state.hpp>
#include <eggs/test/detail/unwind.hpp>

#include <stdexcept>

#include "support.hpp"

namespace {

namespace detail = eggs::test::detail;

void check_pass()
{
    detail::run_state s;
    bool const r = detail::check(true, "expr", s);

    EGGS_TEST_DETAIL_ASSERT(r);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_passed == 1);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_failed == 0);
}

void check_fail()
{
    detail::run_state s;
    bool const r = detail::check(false, "expr", s);

    EGGS_TEST_DETAIL_ASSERT(!r);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_passed == 0);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_failed == 1);
}

void check_throws_pass()
{
    detail::run_state s;
    auto const ep = detail::check_throws([] { throw 42; }, "throw 42", s);

    EGGS_TEST_DETAIL_ASSERT(ep != nullptr);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_passed == 1);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_failed == 0);
}

void check_throws_fail_no_throw()
{
    detail::run_state s;
    auto const ep = detail::check_throws([] {}, "nothing", s);

    EGGS_TEST_DETAIL_ASSERT(ep == nullptr);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_passed == 0);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_failed == 1);
}

void check_throws_propagates_unwind()
{
    detail::run_state s;
    bool caught = false;
    try {
        detail::check_throws([] { throw detail::unwind{}; }, "unwind", s);
    } catch (detail::unwind const&) {
        caught = true;
    }

    EGGS_TEST_DETAIL_ASSERT(caught);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_passed == 0);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_failed == 0);
}

void check_throws_as_pass_exact_type()
{
    detail::run_state s;
    auto const ep = detail::check_throws_as<std::runtime_error>(
        [] { throw std::runtime_error("x"); }, "expr", "std::runtime_error", s
    );

    EGGS_TEST_DETAIL_ASSERT(ep != nullptr);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_passed == 1);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_failed == 0);
}

void check_throws_as_pass_derived_type()
{
    detail::run_state s;
    auto const ep = detail::check_throws_as<std::logic_error>(
        [] { throw std::out_of_range("x"); }, "expr", "std::logic_error", s
    );

    EGGS_TEST_DETAIL_ASSERT(ep != nullptr);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_passed == 1);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_failed == 0);
}

void check_throws_as_fail_wrong_type()
{
    detail::run_state s;
    auto const ep = detail::check_throws_as<std::logic_error>(
        [] { throw std::runtime_error("x"); }, "expr", "std::logic_error", s
    );

    EGGS_TEST_DETAIL_ASSERT(ep == nullptr);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_passed == 0);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_failed == 1);
}

void check_throws_as_fail_no_throw()
{
    detail::run_state s;
    auto const ep = detail::check_throws_as<std::runtime_error>(
        [] {}, "expr", "std::runtime_error", s
    );

    EGGS_TEST_DETAIL_ASSERT(ep == nullptr);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_passed == 0);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_failed == 1);
}

void check_throws_as_propagates_unwind()
{
    detail::run_state s;
    bool caught = false;
    try {
        detail::check_throws_as<std::runtime_error>(
            [] { throw detail::unwind{}; }, "unwind", "std::runtime_error", s
        );
    } catch (detail::unwind const&) {
        caught = true;
    }

    EGGS_TEST_DETAIL_ASSERT(caught);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_passed == 0);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_failed == 0);
}

void check_nothrow_pass()
{
    detail::run_state s;
    bool const r = detail::check_nothrow([] {}, "expr", s);

    EGGS_TEST_DETAIL_ASSERT(r);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_passed == 1);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_failed == 0);
}

void check_nothrow_fail()
{
    detail::run_state s;
    bool const r = detail::check_nothrow([] { throw 1; }, "expr", s);

    EGGS_TEST_DETAIL_ASSERT(!r);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_passed == 0);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_failed == 1);
}

void check_nothrow_propagates_unwind()
{
    detail::run_state s;
    bool caught = false;
    try {
        detail::check_nothrow([] { throw detail::unwind{}; }, "unwind", s);
    } catch (detail::unwind const&) {
        caught = true;
    }

    EGGS_TEST_DETAIL_ASSERT(caught);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_passed == 0);
    EGGS_TEST_DETAIL_ASSERT(s.assertions_failed == 0);
}

} // namespace

int main()
{
    check_pass();
    check_fail();

    check_throws_pass();
    check_throws_fail_no_throw();
    check_throws_propagates_unwind();

    check_throws_as_pass_exact_type();
    check_throws_as_pass_derived_type();
    check_throws_as_fail_wrong_type();
    check_throws_as_fail_no_throw();
    check_throws_as_propagates_unwind();

    check_nothrow_pass();
    check_nothrow_fail();
    check_nothrow_propagates_unwind();

    return 0;
}
