// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test/detail/require.hpp>
#include <eggs/test/detail/run_state.hpp>

#include <source_location>
#include <string>
#include <utility>

namespace eggs::test::detail {

// Exception thrown by SKIP to unwind the current test case and mark it as
// skipped rather than passed or failed.  Caught by the runner per-test.
struct skip_requested final : unwind
{
    std::string reason;
    std::source_location loc;
};

[[noreturn]] inline void skip(
    std::string reason,
    std::source_location loc = ::std::source_location::current()
)
{
    (void)run_state::current(); // aborts if not within a test case
    throw skip_requested{unwind{}, std::move(reason), std::move(loc)};
}

} // namespace eggs::test::detail
