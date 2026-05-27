// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <vector>

namespace eggs::test::detail {

struct test_entry
{
    const char* name;
    const char* desc;
    void (*run)();
};

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
