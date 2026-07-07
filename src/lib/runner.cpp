// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>
#include <eggs/test/detail/print.hpp>
#include <eggs/test/detail/registry.hpp>
#include <eggs/test/detail/stacktrace.hpp>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <format>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace eggs::test {
namespace detail {

run_state*& run_state::_current_ptr() noexcept
{
    static thread_local run_state* tl_current_state = nullptr;
    return tl_current_state;
}

registry::cases_type& registry::cases()
{
    // Meyers singleton — guaranteed to be constructed before first use,
    // which avoids the static-initialisation-order fiasco when test_entry
    // instances in different translation units register before main().
    static registry::cases_type v;
    return v;
}

int registry::run(std::vector<test_entry> const& run)
{
    std::size_t const entry_depth = detail::stacktrace::current().size();

    std::size_t cases_passed = 0;
    std::vector<std::string_view> cases_failed;

    for (test_entry const& e : run) {
        detail::println(
            stdout, "[ {}  ] {} -- {}",
            detail::colored("RUN", detail::ansi::bold_cyan),
            detail::colored(e.name, detail::ansi::yellow),
            detail::colored(e.desc, detail::ansi::gray)
        );

        run_state state;
        state.entry_depth = entry_depth;

        run_state::set_current(&state);
        bool passed = false;
        try {
            e.run();
            passed = !state.assertions_failed;
        } catch (require_failed const&) {
        } catch (std::exception const& ex) {
            detail::println(
                stdout, "  {}: {}",
                detail::colored("EXCEPTION", detail::ansi::bold_red), ex.what()
            );
        } catch (...) {
            detail::println(
                stdout, "  {}",
                detail::colored("UNKNOWN EXCEPTION", detail::ansi::bold_red)
            );
        }
        run_state::set_current(nullptr);

        if (passed) {
            detail::println(
                stdout, "[ {} ] {}",
                detail::colored("PASS", detail::ansi::bold_green),
                detail::colored(e.name, detail::ansi::yellow)
            );
            ++cases_passed;
        } else {
            detail::println(
                stdout, "[ {} ] {}",
                detail::colored("FAIL", detail::ansi::bold_red),
                detail::colored(e.name, detail::ansi::yellow)
            );
            cases_failed.push_back(e.name);
        }

        auto const assertions_total =
            state.assertions_passed + state.assertions_failed;
        auto const summary = std::format(
            "{} of {} assertions passed", state.assertions_passed,
            assertions_total
        );
        detail::println(
            stdout, "{}",
            detail::colored(
                summary,
                passed ? detail::ansi::bold_green : detail::ansi::bold_red
            )
        );
    }

    auto const cases_total = cases_passed + cases_failed.size();
    auto const summary =
        std::format("\n{} of {} test cases passed", cases_passed, cases_total);
    detail::println(
        stdout, "{}",
        detail::colored(
            summary, cases_failed.empty() ? detail::ansi::bold_green
                                          : detail::ansi::bold_red
        )
    );

    if (!cases_failed.empty()) {
        detail::println(
            stdout,
            "{}:", detail::colored("Failed test cases", detail::ansi::bold_red)
        );
        for (auto const& e : cases_failed) {
            detail::println(stdout, "- {}", e);
        }
    }

    return cases_failed.empty() ? EXIT_SUCCESS : EXIT_FAILURE;
}

} // namespace detail

int run(run_options opts)
{
    ::eggs::test::detail::init_color(opts.color);

    auto const& all_cases = detail::registry::cases();

    std::vector<detail::test_entry> selected_cases;
    selected_cases.reserve(opts.run.size());
    if (opts.run.empty()) {
        selected_cases.assign(all_cases.begin(), all_cases.end());
    } else {
        bool any_unknown = false;

        std::unordered_set<std::string_view> seen;
        seen.reserve(opts.run.size());
        for (auto const& name : opts.run) {
            auto const it = all_cases.find(name);
            if (it == all_cases.end()) {
                detail::println(stderr, "error: unknown test case '{}'", name);
                any_unknown = true;
            } else if (!seen.insert(name).second) {
                detail::println(
                    stderr, "warning: duplicate test case '{}'", name
                );
            } else {
                selected_cases.push_back(*it);
            }
        }

        // TODO: consider executing known test cases instead of failing
        if (any_unknown) return EXIT_FAILURE;
    }

    if (opts.list) {
        for (auto const& e : selected_cases) {
            detail::println(stdout, "{}", e.name);
        }
        return EXIT_SUCCESS;
    }

    return detail::registry::run(selected_cases);
}

} // namespace eggs::test
