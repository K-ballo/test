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

// The registration below aborts during static initialization, before main()
// runs. Left at its defaults, the Debug CRT's abort() calls _CALL_REPORTFAULT
// (a blocking Watson/WER "stopped working" prompt), and a failed assert()
// pops its own "Debug Assertion Failed!" dialog; disable both, and route the
// assert report to stderr instead. Declared first so its constructor runs
// before the REGISTER_P initializers below (guaranteed by intra-TU ordering).
struct disable_crash_dialogs
{
    disable_crash_dialogs()
    {
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
        SetErrorMode(SEM_NOGPFAULTERRORBOX | SEM_FAILCRITICALERRORS);
    }
} const disable_crash_dialogs_;

} // namespace
#endif

// Two REGISTER_P instances with the same name collide in the registry ("name/
// instance" is identical for both); registry::add() aborts on registration,
// before main() ever runs.
TEST_CASE(duplicate_case, "desc", int const& a)
{
    (void)a;
}

REGISTER_P(duplicate_case, "same", 1);
REGISTER_P(duplicate_case, "same", 2);
