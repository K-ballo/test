#!/usr/bin/env python3
# Copyright Agustin K-ballo Berge, Fusion Fenix 2026
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

"""Turn AddressSanitizer/UndefinedBehaviorSanitizer diagnostics found in a
ctest log into GitHub Actions error annotations.
"""

import re
import sys
from pathlib import Path

# UBSan prints a single self-contained line for each finding.
UBSAN_RE = re.compile(
    r"^(?P<file>\S+):(?P<line>\d+):(?P<col>\d+): runtime error: (?P<message>.*)$"
)
# ASan (and friends) print a multi-line report; the "==PID==ERROR:" line
# carries the message, and the "SUMMARY:" line carries the location.
SANITIZER_ERROR_RE = re.compile(r"^==\d+==ERROR: (?P<sanitizer>\w+): (?P<message>.*)$")
SUMMARY_RE = re.compile(
    r"^SUMMARY: (?P<sanitizer>\w+): (?P<kind>\S+) (?P<file>.+):(?P<line>\d+):(?P<col>\d+)(?: in .*)?$"
)


def relativize(path: str, repo_root: Path) -> str | None:
    try:
        return str(Path(path).resolve().relative_to(repo_root))
    except ValueError:
        return None


def main() -> int:
    if len(sys.argv) != 3:
        sys.exit(f"usage: {sys.argv[0]} <ctest-log> <repo-root>")

    log_path, repo_root = sys.argv[1], Path(sys.argv[2]).resolve()
    lines = Path(log_path).read_text(encoding="utf-8", errors="replace").splitlines()

    findings = []  # (file, line, col, sanitizer, message)
    seen = set()
    pending_message = None

    for text_line in lines:
        if match := UBSAN_RE.match(text_line):
            key = (match["file"], match["line"], match["col"])
            if key not in seen:
                seen.add(key)
                findings.append(
                    (
                        match["file"],
                        int(match["line"]),
                        int(match["col"]),
                        "UndefinedBehaviorSanitizer",
                        match["message"],
                    )
                )
            continue

        if match := SANITIZER_ERROR_RE.match(text_line):
            pending_message = match["message"]
            continue

        if match := SUMMARY_RE.match(text_line):
            key = (match["file"], match["line"], match["col"])
            message = pending_message or match["kind"]
            pending_message = None
            if key not in seen:
                seen.add(key)
                findings.append(
                    (
                        match["file"],
                        int(match["line"]),
                        int(match["col"]),
                        match["sanitizer"],
                        message,
                    )
                )

    annotated = 0
    for file, line, col, sanitizer, message in findings:
        rel = relativize(file, repo_root)
        if rel is None:
            continue  # outside the checkout (e.g. a system header); nothing to annotate
        annotated += 1
        print(f"::error file={rel},line={line},col={col},title={sanitizer}::{message}")

    if findings:
        print(
            f"{len(findings)} sanitizer diagnostic(s) found, {annotated} annotated.",
            file=sys.stderr,
        )

    return 0


if __name__ == "__main__":
    sys.exit(main())
