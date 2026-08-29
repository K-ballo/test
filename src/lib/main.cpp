// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test/cli.hpp>
#include <eggs/test/detail/print.hpp>
#include <eggs/test/main.hpp>
#include <eggs/test/run.hpp>

#include <charconv>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#if defined(_WIN32)
#    define WIN32_LEAN_AND_MEAN
#    include <crtdbg.h>
#    include <windows.h>
#endif

namespace eggs::test {
namespace {

// The parsed main-only options, an error message, and any unknown arguments.
struct main_result
{
    bool help = false;
    std::optional<int> exit_code;
    bool non_interactive = false;
    std::vector<char const*> unknown;
    std::string error;
};

std::optional<int> parse_int(std::string_view sv) noexcept
{
    int value = 0;
    auto [end, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
    if (ec != std::errc{} || end != sv.data() + sv.size()) return std::nullopt;
    return value;
}

main_result parse_main_cli(std::span<char const* const> args)
{
    main_result opts;

    for (std::string_view const arg : args) {
        if (arg == "--help" || arg == "-h") {
            opts.help = true;
        } else if (arg.starts_with("--exit-code=")) {
            std::string_view const val = arg.substr(12);

            if (opts.exit_code = test::parse_int(val); !opts.exit_code) {
                opts.error = std::format("invalid exit code '{}'", val);
                return opts;
            }
        } else if (arg == "--non-interactive") {
            opts.non_interactive = true;
        } else {
            opts.unknown.push_back(arg.data());
        }
    }

    return opts;
}

void print_help(std::FILE* out, std::string_view const usage)
{
    static constexpr std::size_t k_desc_col = 29u;

    detail::println(
        out, "Usage: {} [options]\n\nOptions:",
        usage.empty() ? "<test-executable>" : usage
    );
    test::print_option(
        out, "-h, --help", {"print this help message and exit"}, k_desc_col
    );
    test::print_options(out, /*ns:*/ {}, k_desc_col);
}

void suppress_debug_dialogs() noexcept
{
#if defined(_WIN32)
    SetErrorMode(
        SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX
    );
    _set_abort_behavior(0, _CALL_REPORTFAULT);

    for (int const report_type : {_CRT_WARN, _CRT_ERROR, _CRT_ASSERT}) {
        _CrtSetReportMode(report_type, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(report_type, _CRTDBG_FILE_STDERR);
        (void)report_type; // NDEBUG
    }
#endif
}

} // namespace

int main(int argc, char const* argv[])
{
    std::span<char const*> const args{argv, static_cast<std::size_t>(argc)};

    auto const parsed = test::parse_cli(args.subspan(1));
    if (!parsed.error.empty()) {
        detail::println(stderr, "error: {}", parsed.error);
        return EXIT_FAILURE;
    }

    auto const main_opts = test::parse_main_cli(parsed.unknown);
    if (!main_opts.error.empty()) {
        detail::println(stderr, "error: {}", main_opts.error);
        return EXIT_FAILURE;
    }
    if (!main_opts.unknown.empty()) {
        for (std::string_view const arg : main_opts.unknown) {
            detail::println(stderr, "error: unknown argument '{}'", arg);
        }
        return EXIT_FAILURE;
    }

    if (main_opts.help) {
        std::filesystem::path const path{args.front() ? args.front() : ""};
        auto const& usage = path.filename().string();

        test::print_help(stdout, usage);
        return EXIT_SUCCESS;
    }

    if (main_opts.non_interactive) {
        test::suppress_debug_dialogs();
    }

    int const result = test::run(parsed.options);
    return main_opts.exit_code.value_or(result);
}

} // namespace eggs::test
