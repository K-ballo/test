// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test/detail/noinline.hpp>
#include <eggs/test/detail/registry.hpp>
#include <eggs/test/detail/run_state.hpp>

#include <cstddef>
#include <exception>
#include <source_location>

namespace eggs::test::detail {

template <typename Fn>
std::exception_ptr invoke_catch(Fn&& fn)
{
    try {
        fn();
    } catch (require_failed const&) {
        throw;
    } catch (...) {
        return std::current_exception();
    }
    return nullptr;
}

template <typename ExcType>
bool holds_exception(std::exception_ptr const& ep) noexcept
{
    try {
        std::rethrow_exception(ep);
    } catch (ExcType const&) {
        return true;
    } catch (...) {
        return false;
    }
}

template <typename Fn>
std::exception_ptr check_throws(Fn fn)
{
    try {
        fn();
    } catch (require_failed const&) {
        throw;
    } catch (...) {
        run_state::current().last_threw = std::current_exception();
        return std::current_exception();
    }
    return nullptr;
}

template <typename ExcType, typename Fn>
std::exception_ptr check_throws_as(Fn fn)
{
    try {
        fn();
    } catch (require_failed const&) {
        throw;
    } catch (ExcType) {
        run_state::current().last_threw = std::current_exception();
        return std::current_exception();
    } catch (...) {
        run_state::current().last_threw = std::current_exception();
        return nullptr;
    }
    return nullptr;
}

EGGS_TEST_NOINLINE void check_failed(
    const char* expr, std::source_location const& loc, std::size_t entry_depth
);

EGGS_TEST_NOINLINE void check_throws_failed(
    char const* expr, std::source_location const& loc, std::size_t entry_depth
);

EGGS_TEST_NOINLINE void check_throws_as_failed(
    char const* expr, char const* exc_type, std::source_location const& loc,
    std::size_t entry_depth
);

EGGS_TEST_NOINLINE void check_nothrow_failed(
    char const* expr, std::source_location const& loc, std::size_t entry_depth
);

} // namespace eggs::test::detail
