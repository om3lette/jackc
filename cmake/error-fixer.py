#!/usr/bin/env python3
# This script adjusts produces assembly to run under RARS

import re
import sys
from pathlib import Path

if len(sys.argv) != 2:
    print(f"Usage: python3 {__file__} <PATH_TO_FILE>")
    sys.exit(1)

file_path: Path = Path(sys.argv[1])
file_content: str = file_path.read_text()

PROGRAM_NAME: str = Path(__file__).stem.replace("-", " ").capitalize()
print(f"\n{PROGRAM_NAME.center(80, '=')}")


def replace_unsupported_syntax(s: str) -> str:
    print("Fixing unsupported sw syntax...")
    pattern = r"(sw|sh|sb)\s+([a-z0-9]+)\s*,\s*%lo\(([^)]+)\)\(([a-z0-9]+)\)"
    replacement = r"addi \4, \4, %lo(\3)\n\t\1 \2, 0(\4)"
    return re.sub(pattern, replacement, s)

def replace_unsupported_sections(s: str) -> str:
    section_mapping: list[tuple[str, str]] = [
        (r"\.srodata", ".rodata"),
        (r"\.section\s+\.sbss[^\n]*", ".data"),
        (r"\.zero", ".space"),
    ]

    for section in section_mapping:
        print(f"Replacing {section[0]} with {section[1]}...")
        s = re.sub(section[0], section[1], s, flags=re.MULTILINE)
    return s


file_content = replace_unsupported_syntax(file_content)
file_content = replace_unsupported_sections(file_content)

file_path.write_text(file_content)
