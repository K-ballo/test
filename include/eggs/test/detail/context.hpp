// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test/detail/run_state.hpp>

#include <source_location>
#include <string>
#include <utility>

namespace eggs::test::detail {

// RAII frame for a single CONTEXT(...) call.
struct context_frame
{
    explicit context_frame(
        std::string message_, std::source_location loc_
    ) noexcept
        : message(std::move(message_)),
          loc(loc_),
          prev(run_state::current().exchange_context(this))
    {
    }

    ~context_frame() { run_state::current().exchange_context(prev); }

    context_frame(context_frame const&) = delete;
    context_frame& operator=(context_frame const&) = delete;
    context_frame(context_frame&&) = delete;
    context_frame& operator=(context_frame&&) = delete;

    std::string const message;
    std::source_location const loc;
    context_frame const* const prev;
};

} // namespace eggs::test::detail
