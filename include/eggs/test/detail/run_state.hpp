// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test/detail/stacktrace.hpp>

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <utility>

namespace eggs::test::detail {

struct context_frame;

struct run_state
{
    bool verbose = false;
    std::size_t entry_depth = 0;
    context_frame const* context_top = nullptr;
    std::size_t assertions_passed = 0;
    std::size_t assertions_failed = 0;

    void mark_entry() { entry_depth = stacktrace::current().size(); }

    // Installs `frame` as the current context, returning the previous one.
    context_frame const* exchange_context(context_frame const* frame) noexcept
    {
        return std::exchange(context_top, frame);
    }

    // Points at the run_state of the currently-executing test on this thread.
    // nullptr between test cases.
    [[nodiscard]] static run_state& current()
    {
        run_state* ptr = _current_ptr();
        assert(ptr && "CHECK/REQUIRE called outside of a test case");
        if (!ptr) [[unlikely]]
            std::abort();

        return *ptr;
    }

    static void set_current(run_state* state) noexcept
    {
        _current_ptr() = state;
    }

  private:
    [[nodiscard]] static run_state*& _current_ptr() noexcept;
};

} // namespace eggs::test::detail
