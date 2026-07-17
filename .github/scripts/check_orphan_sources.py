#!/usr/bin/env python3
# Copyright Agustin K-ballo Berge, Fusion Fenix 2026
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

"""Fail if a tracked header/source file is not referenced by any CMake target.

Reads the CMake File API codemodel-v2 reply (see the "Configure" step run
before this script) so target sources are read exactly as CMake resolved
them: FILE_SET headers, add_lib_test()/add_example() helper wrappers, etc.
"""

import json
import pathlib
import subprocess
import sys

SOURCE_EXTENSIONS = {".hpp", ".cpp"}

# Standalone consumer projects: each is its own CMake project, configured and
# built on its own by the "test" workflow's dedicated consumer steps. They
# are never part of the main project's target graph, so their sources are
# excluded here rather than reported as orphans.
EXCLUDED_DIRS = (
    "test/cmake-fetch_content/",
    "test/cmake-find_package/",
)


def tracked_source_files(repo_root: pathlib.Path) -> set[str]:
    out = subprocess.run(
        ["git", "ls-files"], cwd=repo_root, check=True, capture_output=True, text=True
    ).stdout

    files = set()
    for line in out.splitlines():
        if pathlib.PurePosixPath(line).suffix not in SOURCE_EXTENSIONS:
            continue
        if any(line.startswith(prefix) for prefix in EXCLUDED_DIRS):
            continue
        files.add(line)
    return files


def referenced_source_files(build_dir: pathlib.Path) -> set[str]:
    reply_dir = build_dir / ".cmake" / "api" / "v1" / "reply"
    index_files = sorted(reply_dir.glob("index-*.json"))
    if not index_files:
        sys.exit(f"error: no CMake File API reply found in {reply_dir}")

    index = json.loads(index_files[-1].read_text())
    codemodel_file = reply_dir / index["reply"]["codemodel-v2"]["jsonFile"]
    codemodel = json.loads(codemodel_file.read_text())

    referenced = set()
    for target_ref in codemodel["configurations"][0]["targets"]:
        target = json.loads((reply_dir / target_ref["jsonFile"]).read_text())
        for source in target.get("sources", []):
            if not source.get("isGenerated", False):
                referenced.add(source["path"])
    return referenced


def main() -> int:
    if len(sys.argv) != 2:
        sys.exit(f"usage: {sys.argv[0]} <build-dir>")

    build_dir = pathlib.Path(sys.argv[1]).resolve()
    repo_root = pathlib.Path(
        subprocess.run(
            ["git", "rev-parse", "--show-toplevel"],
            check=True,
            capture_output=True,
            text=True,
        ).stdout.strip()
    )

    tracked = tracked_source_files(repo_root)
    referenced = referenced_source_files(build_dir)

    orphans = sorted(tracked - referenced)
    if orphans:
        print("error: files not referenced by any CMake target:")
        for path in orphans:
            print(f"  {path}")
        return 1

    print(
        f"OK: all {len(tracked)} tracked header/source files "
        "are referenced by a CMake target."
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
