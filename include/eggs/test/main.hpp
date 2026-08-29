// Eggs.Test
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2026
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

namespace eggs::test {

// Full default main: parse_cli + --help handling + run.
// Link Eggs::TestMain to get this wired up automatically, or
// call it from your own main() to add setup before tests run.
int main(int argc, char const* argv[]);

inline int main(int argc, char* argv[])
{
    return main(argc, const_cast<char const**>(argv));
}

} // namespace eggs::test
