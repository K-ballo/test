// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test-export.h>

#include <cassert>
#include <cstddef>
#include <cstdlib>

namespace eggs::test::detail {

class run_state
{
  public:
    std::size_t entry_depth = 0;
    std::size_t assertions_passed = 0;
    std::size_t assertions_failed = 0;

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
    EGGS_TEST_EXPORT [[nodiscard]] static run_state*& _current_ptr() noexcept;
};

} // namespace eggs::test::detail
