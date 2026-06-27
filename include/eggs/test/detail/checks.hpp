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

EGGS_TEST_NOINLINE void check_failed(
    const char* expr, std::source_location const& loc, std::size_t entry_depth
);

EGGS_TEST_NOINLINE std::exception_ptr check_throws_as(
    std::exception_ptr threw, bool (*holds)(std::exception_ptr const&) noexcept,
    char const* expr, run_state& s, char const* exc_type,
    std::source_location const& loc
);

EGGS_TEST_NOINLINE bool check_nothrow(
    std::exception_ptr threw, const char* expr, run_state& s,
    std::source_location const& loc
);

} // namespace eggs::test::detail
