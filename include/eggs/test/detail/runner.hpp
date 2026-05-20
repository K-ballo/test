// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test/detail/checks.hpp>
#include <eggs/test/detail/registry.hpp>

#include <exception>
#include <print>
#include <stacktrace>
#include <string_view>
#include <vector>

namespace eggs::test::detail {

// Out-of-class inline definition of registry::run_all().
// Runs every registered test case in registration order, catches all
// exceptions, and prints a final summary.  Returns 0 if all cases passed,
// 1 if any failed.
inline int registry::run_all()
{
    std::size_t const entry_depth = std::stacktrace::current().size();

    std::size_t cases_passed = 0;
    std::vector<std::string_view> cases_failed;

    for (test_entry& e : cases()) {
        std::println("[ RUN  ] {} -- {}", e.name, e.desc);

        e.state.entry_depth = entry_depth;

        tl_current_state = &e.state;
        bool passed = false;
        try {
            e.run();
            passed = !e.state.assertions_failed;
        } catch (require_failed const&) {
        } catch (std::exception const& ex) {
            std::println("  EXCEPTION: {}", ex.what());
        } catch (...) {
            std::println("  UNKNOWN EXCEPTION");
        }
        tl_current_state = nullptr;

        if (passed) {
            std::println("[ PASS ] {}", e.name);
            ++cases_passed;
        } else {
            std::println("[ FAIL ] {}", e.name);
            cases_failed.push_back(e.name);
        }

        auto const assertions_total =
            e.state.assertions_passed + e.state.assertions_failed;
        std::println(
            "{} of {} assertions passed", e.state.assertions_passed,
            assertions_total
        );
    }

    auto const cases_total = cases_passed + cases_failed.size();
    std::println("\n{} of {} test cases passed", cases_passed, cases_total);
    if (!cases_failed.empty()) {
        std::println("Failed test cases:");
        for (const auto& e : cases_failed) {
            std::println("- {}", e);
        }
    }

    return cases_failed.empty() ? 0 : 1;
}

} // namespace eggs::test::detail
