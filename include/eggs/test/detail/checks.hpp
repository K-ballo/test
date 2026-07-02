// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test/detail/function_ref.hpp>
#include <eggs/test/detail/noinline.hpp>
#include <eggs/test/detail/registry.hpp>
#include <eggs/test/detail/run_state.hpp>

#include <cstddef>
#include <exception>
#include <source_location>

namespace eggs::test::detail {

EGGS_TEST_NOINLINE bool check(
    bool c, const char* expr,
    run_state& s = ::eggs::test::detail::run_state::current(),
    std::source_location const& loc = std::source_location::current()
);

EGGS_TEST_NOINLINE std::exception_ptr check_throws(
    detail::function_ref<void()> fn, const char* expr,
    run_state& s = ::eggs::test::detail::run_state::current(),
    std::source_location const& loc = std::source_location::current()
);

template <typename ExcType, typename Fn>
struct check_throws_as_wrapper
{
    Fn fn;

    std::exception_ptr
    operator()(run_state& s = ::eggs::test::detail::run_state::current())
    {
        try {
            fn();
        } catch (ExcType const&) {
            ++s.assertions_passed;
            return std::current_exception();
        }
        return nullptr;
    }
};

template <typename ExcType, typename Fn>
auto wrap_throws_as(Fn fn)
{
    return check_throws_as_wrapper<ExcType, Fn>{fn};
}

EGGS_TEST_NOINLINE std::exception_ptr check_throws_as(
    detail::function_ref<std::exception_ptr()> fn, const char* expr,
    const char* exc_type,
    run_state& s = ::eggs::test::detail::run_state::current(),
    std::source_location const& loc = std::source_location::current()
);

EGGS_TEST_NOINLINE bool check_nothrow(
    detail::function_ref<void()> fn, const char* expr,
    run_state& s = ::eggs::test::detail::run_state::current(),
    std::source_location const& loc = std::source_location::current()
);

} // namespace eggs::test::detail
