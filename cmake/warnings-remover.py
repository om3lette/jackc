#!/usr/bin/env python3
# This script removes unsupported directives, sections, and aligns from a source file
# to clear RARS's console from hundreds of warnings.

import re
import sys
from io import TextIOWrapper
from pathlib import Path

if len(sys.argv) != 3:
    print(f"Usage: python3 {__file__} <SOURCE_FILE_PATH> <OUT_PATH>")
    sys.exit(1)

source_path: Path = Path(sys.argv[1])
source_file: TextIOWrapper = open(source_path, "r", encoding="utf-8")

out_path: Path = Path(sys.argv[2])
out_file: TextIOWrapper = open(out_path, "w", encoding="utf-8")

PROGRAM_NAME: str = Path(__file__).stem.replace("-", " ").capitalize()
print(f"\n{PROGRAM_NAME.center(80, '=')}")

print(f"Working on {source_path.absolute()}")
print(f"Save path: {out_path.absolute()}")
unsupported_directives: list[str] = [
    "file",
    "ident",
    "attribute",
    "type",
    "size",
    "option",
    "weak",
]
unsupported_directives_regex: re.Pattern[str] = re.compile(
    rf"^\s*\.({'|'.join(unsupported_directives)})", flags=re.MULTILINE
)

unsupported_sections: list[str] = ["note.GNU"]
unsupported_sections_regex: re.Pattern[str] = re.compile(
    rf"^\s*\.section\s*\.({'|'.join(unsupported_sections)}).*", flags=re.MULTILINE
)
unsupported_align_regex: re.Pattern[str] = re.compile(
    r"^(\s*\.align\s*)(1|2)$", flags=re.MULTILINE
)
comment_regex: re.Pattern[str] = re.compile(r"^\s*#.*", flags=re.MULTILINE)

print("""
Removing:
1. Unsupported directives
2. Unsupported sections
Replacing:
1. < 4 bytes allignments with minimum allowed .align 4
""")

print(f"Unsupported directives: {', '.join(unsupported_directives)}")
print(f"Unsupported sections: {', '.join(unsupported_sections)}")

for line in source_file.readlines():
    if unsupported_directives_regex.match(line):
        continue
    if unsupported_sections_regex.match(line):
        continue
    if comment_regex.match(line):
        continue
    patched_line: str = unsupported_align_regex.sub(r"\g<1>4", line)
    out_file.write(patched_line)

source_file.close()
out_file.close()
print("\nDone!")
