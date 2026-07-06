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
#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace eggs::test {
namespace detail {

namespace {

// "<passed> passed (<percent>%)", plus " | <failed> failed (<percent>%)"
// when failed != 0. The two percentages always add up to 100.
std::string format_summary(std::size_t passed, std::size_t failed)
{
    auto const total = passed + failed;
    auto const percent_passed = total == 0 ? 100 : passed * 100 / total;

    if (failed == 0) {
        return std::format("{} passed ({}%)", passed, percent_passed);
    }

    return std::format(
        "{} passed ({}%) | {} failed ({}%)", passed, percent_passed, failed,
        100 - percent_passed
    );
}

} // namespace

run_state*& run_state::_current_ptr() noexcept
{
    static thread_local run_state* tl_current_state = nullptr;
    return tl_current_state;
}

registry::cases_type& registry::cases()
{
    // Meyers singleton - guaranteed to be constructed before first use,
    // which avoids the static-initialisation-order fiasco when test_entry
    // instances in different translation units register before main().
    static registry::cases_type v;
    return v;
}

namespace {

enum class outcome
{
    passed,
    failed,
    skipped
};

struct skipped_case
{
    std::string_view name;
    std::string reason;
};

} // namespace

int registry::run(std::vector<test_entry> const& run)
{
    std::size_t const entry_depth = detail::stacktrace::current().size();

    std::size_t cases_passed = 0;
    std::vector<std::string_view> cases_failed;
    std::vector<skipped_case> cases_skipped;

    for (test_entry const& e : run) {
        detail::println(
            stdout, "[ RUN  ] {} -- {}  [{}:{}]", e.name, e.desc,
            e.loc.file_name(), e.loc.line()
        );

        run_state state;
        state.entry_depth = entry_depth;

        run_state::set_current(&state);
        outcome result = outcome::failed;
        std::optional<skip_requested> skip_info;
        try {
            e.run();
            result =
                state.assertions_failed ? outcome::failed : outcome::passed;
        } catch (detail::skip_requested const& sk) {
            // An earlier CHECK/REQUIRE failure makes the outcome failed.
            result =
                state.assertions_failed ? outcome::failed : outcome::skipped;
            skip_info = sk;
        } catch (detail::unwind const&) {
            result = outcome::failed;
        } catch (std::exception const& ex) {
            detail::println(stdout, "  EXCEPTION: {}", ex.what());
        } catch (...) {
            detail::println(stdout, "  UNKNOWN EXCEPTION");
        }
        run_state::set_current(nullptr);

        switch (result) {
            case outcome::passed:
            case outcome::failed: {
                bool const passed = result == outcome::passed;
                auto const assertions_total =
                    state.assertions_passed + state.assertions_failed;
                if (assertions_total == 0) {
                    detail::println(
                        stdout, "[ {} ] {} -- 0 assertions\n",
                        passed ? "PASS" : "FAIL", e.name
                    );
                } else {
                    detail::println(
                        stdout, "[ {} ] {} -- {} assertions: {}\n",
                        passed ? "PASS" : "FAIL", e.name, assertions_total,
                        format_summary(
                            state.assertions_passed, state.assertions_failed
                        )
                    );
                }
                if (passed) {
                    ++cases_passed;
                } else {
                    cases_failed.push_back(e.name);
                }
                break;
            }
            case outcome::skipped: {
                detail::println(
                    stdout, "[ SKIP ] {} -- {}  [{}:{}]", e.name,
                    skip_info->reason, skip_info->loc.file_name(),
                    skip_info->loc.line()
                );
                cases_skipped.push_back({e.name, skip_info->reason});
                break;
            }
        }
    }

    // Omit summary if only one test-case.
    auto const cases_total =
        cases_passed + cases_failed.size() + cases_skipped.size();
    if (cases_total != 1) {
        detail::println(
            stdout, "{} test cases: {}{}", cases_total,
            format_summary(cases_passed, cases_failed.size()),
            cases_failed.empty() ? "" : ":"
        );
        for (auto const& e : cases_failed) {
            detail::println(stdout, "- {}", e);
        }
        if (!cases_skipped.empty()) {
            detail::println(stdout, "Skipped test cases:");
            for (auto const& e : cases_skipped) {
                detail::println(stdout, "- {} -- {}", e.name, e.reason);
            }
        }
    }

    return cases_failed.empty() ? EXIT_SUCCESS : EXIT_FAILURE;
}

} // namespace detail

int run(run_options opts)
{
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
