// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <eggs/test/detail/checks.hpp>
#include <eggs/test/detail/print.hpp>
#include <eggs/test/detail/registry.hpp>
#include <eggs/test/detail/run_state.hpp>
#include <eggs/test/detail/stacktrace.hpp>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string_view>
#include <vector>

namespace eggs::test::detail {

// Out-of-class inline definition of registry::run().
// Runs test cases in the order given by `run`.
// Returns EXIT_SUCCESS if all cases passed, EXIT_FAILURE if any failed.
inline int registry::run(std::vector<test_entry> const& run)
{
    std::size_t const entry_depth = stacktrace::current().size();

    std::size_t cases_passed = 0;
    std::vector<std::string_view> cases_failed;

    for (test_entry const& e : run) {
        detail::println(stdout, "[ RUN  ] {} -- {}", e.name, e.desc);

        run_state state;
        state.entry_depth = entry_depth;

        run_state::set_current(&state);
        bool passed = false;
        try {
            e.run();
            passed = !state.assertions_failed;
        } catch (require_failed const&) {
        } catch (std::exception const& ex) {
            detail::println(stdout, "  EXCEPTION: {}", ex.what());
        } catch (...) {
            detail::println(stdout, "  UNKNOWN EXCEPTION");
        }
        run_state::set_current(nullptr);

        if (passed) {
            detail::println(stdout, "[ PASS ] {}", e.name);
            ++cases_passed;
        } else {
            detail::println(stdout, "[ FAIL ] {}", e.name);
            cases_failed.push_back(e.name);
        }

        auto const assertions_total =
            state.assertions_passed + state.assertions_failed;
        detail::println(
            stdout, "{} of {} assertions passed", state.assertions_passed,
            assertions_total
        );
    }

    auto const cases_total = cases_passed + cases_failed.size();
    detail::println(
        stdout, "\n{} of {} test cases passed", cases_passed, cases_total
    );
    if (!cases_failed.empty()) {
        detail::println(stdout, "Failed test cases:");
        for (auto const& e : cases_failed) {
            detail::println(stdout, "- {}", e);
        }
    }

    return cases_failed.empty() ? EXIT_SUCCESS : EXIT_FAILURE;
}

} // namespace eggs::test::detail
