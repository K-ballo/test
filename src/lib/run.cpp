// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test/detail/print.hpp>
#include <eggs/test/detail/registry.hpp>
#include <eggs/test/detail/run_state.hpp>
#include <eggs/test/detail/unwind.hpp>
#include <eggs/test/run.hpp>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <format>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include <stdio.h> // ::fileno / ::_fileno, ::tmpfile_s (not guaranteed via <cstdio>)

#if defined(_WIN32)
#    include <io.h>
#else
#    include <unistd.h>
#endif

eggs::test::detail::run_state*&
eggs::test::detail::run_state::_current_ptr() noexcept
{
    static thread_local run_state* tl_current_state = nullptr;
    return tl_current_state;
}

eggs::test::detail::registry::cases_type& eggs::test::detail::registry::cases()
{
    static registry::cases_type cases_;
    return cases_;
}

namespace eggs::test {
namespace detail {
namespace {

#if defined(_WIN32)
int dup_fd(int fd) noexcept
{
    return ::_dup(fd);
}

int dup2_fd(int src, int dst) noexcept
{
    return ::_dup2(src, dst);
}

int close_fd(int fd) noexcept
{
    return ::_close(fd);
}

int fileno_of(std::FILE* f) noexcept
{
    return ::_fileno(f);
}
#else
int dup_fd(int fd) noexcept
{
    return ::dup(fd);
}

int dup2_fd(int src, int dst) noexcept
{
    return ::dup2(src, dst);
}

int close_fd(int fd) noexcept
{
    return ::close(fd);
}

int fileno_of(std::FILE* f) noexcept
{
    return ::fileno(f);
}
#endif

// std::tmpfile() is flagged deprecated (C4996) by MSVC's CRT in favor of the
// bounds-checked tmpfile_s(); use it under the MSVC CRT (also targeted by
// clang-cl), std::tmpfile() everywhere else.
#if defined(_MSC_VER)
std::FILE* open_tmpfile() noexcept
{
    std::FILE* f = nullptr;
    return ::tmpfile_s(&f) == 0 ? f : nullptr;
}
#else
std::FILE* open_tmpfile() noexcept
{
    return std::tmpfile();
}
#endif

// Redirects stdout+stderr into a single temp file for the duration of the
// capture. `enabled=false`, or any setup failure (open_tmpfile()/dup()/dup2()
// failing, e.g. a sandboxed environment with no writable temp directory),
// both leave the capture inactive: stdout/stderr are left untouched, exactly
// as if the feature were off. Never throws, never aborts the run.
class output_capture
{
  public:
    explicit output_capture(bool enabled)
    {
        if (!enabled) return;

        std::fflush(stdout);
        std::fflush(stderr);

        tmp_ = open_tmpfile();
        if (!tmp_) return;

        stdout_ = dup_fd(fileno_of(stdout));
        if (stdout_ == -1) {
            std::fclose(tmp_);
            tmp_ = nullptr;
            return;
        }

        stderr_ = dup_fd(fileno_of(stderr));
        if (stderr_ == -1) {
            close_fd(stdout_);
            stdout_ = -1;
            std::fclose(tmp_);
            tmp_ = nullptr;
            return;
        }

        int const tmp_fd = fileno_of(tmp_);
        if (dup2_fd(tmp_fd, fileno_of(stdout)) == -1 ||
            dup2_fd(tmp_fd, fileno_of(stderr)) == -1) {
            dup2_fd(stdout_, fileno_of(stdout));
            dup2_fd(stderr_, fileno_of(stderr));
            close_fd(stdout_);
            close_fd(stderr_);
            stdout_ = stderr_ = -1;
            std::fclose(tmp_);
            tmp_ = nullptr;
        }
    }

    output_capture(output_capture const&) = delete;
    output_capture& operator=(output_capture const&) = delete;

    ~output_capture() { stop(/*replay:*/ false); }

    // Restores the real stdout/stderr. If `replay` is true and the capture
    // was active, copies the captured bytes to the (now-restored) stdout
    // first. No-op if the capture was never active. Safe to call more than
    // once.
    void stop(bool replay)
    {
        if (!tmp_) return;

        std::fflush(stdout);
        std::fflush(stderr);

        dup2_fd(stdout_, fileno_of(stdout));
        dup2_fd(stderr_, fileno_of(stderr));
        close_fd(stdout_);
        close_fd(stderr_);
        stdout_ = stderr_ = -1;

        if (replay) {
            std::fflush(tmp_);
            std::rewind(tmp_);

            char buf[4096];
            std::size_t n;
            while ((n = std::fread(buf, 1, sizeof buf, tmp_)) > 0) {
                std::fwrite(buf, 1, n, stdout);
            }
            std::fflush(stdout);
        }

        std::fclose(tmp_); // tmpfile() content is removed on close
        tmp_ = nullptr;
    }

  private:
    std::FILE* tmp_ = nullptr;
    int stdout_ = -1;
    int stderr_ = -1;
};

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

int run(
    std::vector<test_entry const*> const& run, bool verbose,
    bool output_on_failure, bool capture_output
)
{
    std::size_t cases_passed = 0;
    std::vector<std::string_view> cases_failed;

    for (test_entry const* e : run) {
        detail::println(
            stdout, "[ RUN  ] {} -- {}  [{}:{}]", e->name, e->desc,
            e->loc.file_name(), e->loc.line()
        );

        run_state state;
        state.verbose = verbose;

        output_capture capture{output_on_failure || capture_output};

        run_state::set_current(&state);
        bool passed = false;
        try {
            e->run(state);
            passed = !state.assertions_failed;
        } catch (detail::unwind const&) {
        } catch (std::exception const& ex) {
            detail::println(stdout, "  EXCEPTION: {}", ex.what());
        } catch (...) {
            detail::println(stdout, "  UNKNOWN EXCEPTION");
        }
        run_state::set_current(nullptr);

        // capture_output always replays (it exists to prove the capture
        // round-trip is transparent); output_on_failure only replays when
        // the test case failed.
        capture.stop(
            /*replay:*/ capture_output || (output_on_failure && !passed)
        );

        auto const assertions_total =
            state.assertions_passed + state.assertions_failed;
        if (assertions_total == 0) {
            detail::println(
                stdout, "[ {} ] {} -- 0 assertions\n", passed ? "PASS" : "FAIL",
                e->name
            );
        } else {
            detail::println(
                stdout, "[ {} ] {} -- {} assertions: {}\n",
                passed ? "PASS" : "FAIL", e->name, assertions_total,
                detail::format_summary(
                    state.assertions_passed, state.assertions_failed
                )
            );
        }

        if (passed) {
            ++cases_passed;
        } else {
            cases_failed.push_back(e->name);
        }
    }

    // Omit summary if only one test-case.
    auto const cases_total = cases_passed + cases_failed.size();
    if (cases_total != 1) {
        detail::println(
            stdout, "{} test cases: {}{}", cases_total,
            detail::format_summary(cases_passed, cases_failed.size()),
            cases_failed.empty() ? "" : ":"
        );
        for (auto const& e : cases_failed) {
            detail::println(stdout, "- {}", e);
        }
    }

    return cases_failed.empty() ? EXIT_SUCCESS : EXIT_FAILURE;
}

} // namespace
} // namespace detail

int run(run_options opts)
{
    auto const& all_cases = detail::registry::cases();

    std::vector<detail::test_entry const*> selected_cases;
    if (opts.run.empty()) {
        selected_cases.resize(all_cases.size()); // for overwrite
        auto it = selected_cases.begin();
        for (auto const& e : all_cases) {
            *it++ = &e;
        }
    } else {
        selected_cases.reserve(opts.run.size());

        bool any_unknown = false;

        std::unordered_set<std::string_view> seen;
        seen.reserve(opts.run.size());
        for (auto const& name : opts.run) {
            auto const it = all_cases.find(name);
            if (it == all_cases.end()) {
                detail::println(stderr, "error: unknown test case '{}'", name);
                any_unknown = true;

                if (!detail::is_valid_instance_name(name)) {
                    detail::println(
                        stderr, "warning: '{}' is not a valid test case name",
                        name
                    );
                }
            } else if (!seen.insert(name).second) {
                detail::println(
                    stderr, "warning: duplicate test case '{}'", name
                );
            } else {
                selected_cases.push_back(&*it);
            }
        }

        // TODO: consider executing known test cases instead of failing
        if (any_unknown) return EXIT_FAILURE;
    }

    if (opts.list) {
        for (auto const* e : selected_cases) {
            detail::println(stdout, "{}", e->name);
        }
        return EXIT_SUCCESS;
    }

    return detail::run(
        selected_cases, opts.verbose, opts.output_on_failure,
        opts.capture_output
    );
}

} // namespace eggs::test
