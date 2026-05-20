// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <vector>

namespace eggs::test::detail {

struct run_state
{
    std::size_t entry_depth = 0;
    std::size_t assertions_passed = 0;
    std::size_t assertions_failed = 0;
};

struct test_entry
{
    const char* name;
    const char* desc;
    void (*run)();
    run_state state; // populated by run_all()
};

// Points at the run_state of the currently-executing test on this thread.
// nullptr between test cases.
inline thread_local run_state* tl_current_state = nullptr;

inline run_state& current_state()
{
    assert(tl_current_state && "CHECK/REQUIRE called outside of a test case");
    if (!tl_current_state) std::abort();

    return *tl_current_state;
}

// Exception thrown by REQUIRE to unwind the current test case without
// terminating the process.  Caught by the runner per-test.
struct require_failed
{};

struct registry
{
    static auto cases() -> std::vector<test_entry>&
    {
        // Meyers singleton — guaranteed to be constructed before first use,
        // which avoids the static-initialisation-order fiasco when test_entry
        // instances in different translation units register before main().
        static std::vector<test_entry> v;
        return v;
    }

    static void add(test_entry e) { cases().push_back(e); }

    static int run_all(); // defined in runner.hpp
};

} // namespace eggs::test::detail
