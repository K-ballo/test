// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstdlib>

// EGGS_ABORT
//
// Terminates the program upon encountering an internal invariant violation.
// Defined as ::std::abort() by default; may be overridden from the command
// line (e.g. -DEGGS_ABORT=::std::exit(0)) so that coverage builds can exit
// cleanly instead of aborting, allowing profiling data to be flushed.
#ifndef EGGS_ABORT
#    define EGGS_ABORT ::std::abort()
#endif
