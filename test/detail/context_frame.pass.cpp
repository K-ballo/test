// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test/detail/context.hpp>
#include <eggs/test/detail/run_state.hpp>

#include <source_location>

#include "support.hpp"

namespace {

namespace detail = eggs::test::detail;

void context_frame_sets_and_restores_context_top()
{
    detail::run_state s;
    detail::run_state::set_current(&s);

    EGGS_TEST_DETAIL_CHECK(s.context_top == nullptr);
    {
        detail::context_frame const cf(
            "outer", std::source_location::current()
        );
        EGGS_TEST_DETAIL_CHECK(s.context_top == &cf);
    }
    EGGS_TEST_DETAIL_CHECK(s.context_top == nullptr);

    detail::run_state::set_current(nullptr);
}

void context_frame_stores_message_and_loc()
{
    detail::run_state s;
    detail::run_state::set_current(&s);

    auto const loc = std::source_location::current();
    {
        detail::context_frame const cf("some message", loc);

        EGGS_TEST_DETAIL_CHECK(cf.message == "some message");
        EGGS_TEST_DETAIL_CHECK(cf.loc.line() == loc.line());
        EGGS_TEST_DETAIL_CHECK(cf.prev == nullptr);
    }

    detail::run_state::set_current(nullptr);
}

void context_frame_nests()
{
    detail::run_state s;
    detail::run_state::set_current(&s);

    {
        detail::context_frame const outer(
            "outer", std::source_location::current()
        );
        EGGS_TEST_DETAIL_CHECK(s.context_top == &outer);
        EGGS_TEST_DETAIL_CHECK(outer.prev == nullptr);

        {
            detail::context_frame const inner(
                "inner", std::source_location::current()
            );
            EGGS_TEST_DETAIL_CHECK(s.context_top == &inner);
            EGGS_TEST_DETAIL_CHECK(inner.prev == &outer);
        }

        EGGS_TEST_DETAIL_CHECK(s.context_top == &outer);
    }

    detail::run_state::set_current(nullptr);
}

} // namespace

int main()
{
    context_frame_sets_and_restores_context_top();
    context_frame_stores_message_and_loc();
    context_frame_nests();

    return eggs::test_support::report();
}
