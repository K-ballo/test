// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/test.hpp>

#ifdef _WIN32
#    include <cstdlib>

#    include <crtdbg.h>
#    include <windows.h>

namespace {

// Suppress MSVC debug dialogs.
struct disable_debug_dialogs
{
    disable_debug_dialogs()
    {
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
        SetErrorMode(SEM_NOGPFAULTERRORBOX | SEM_FAILCRITICALERRORS);
    }
} const disable_debug_dialogs_;

} // namespace
#endif

TEST_CASE(duplicate_case, "desc", int const& a)
{
    (void)a;
}

// REGISTER_P rejects a duplicate instance name for the same test case.
REGISTER_P(duplicate_case, "same", 1);
REGISTER_P(duplicate_case, "same", 2);
