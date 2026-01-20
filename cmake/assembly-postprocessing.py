#!/usr/bin/env python3
import re
import shutil
import sys
from pathlib import Path

if len(sys.argv) != 4:
    print(f"Usage: python3 {__file__} <BIN_DIR> <ASM_DIR> <MERGED_FILE_PATH>")
    sys.exit(1)

bin_dir: Path = Path(sys.argv[1])
asm_dir: Path = Path(sys.argv[2])
merged_path: Path = Path(sys.argv[3])
asm_dir.mkdir(exist_ok=True)

PROGRAM_NAME: str = Path(__file__).stem.replace("-", " ").capitalize()
print(f"\n{PROGRAM_NAME.center(80, '=')}")
print(f"Moving asm files to {asm_dir.absolute()}")
for file_path in bin_dir.iterdir():
    if (
        not file_path.is_file()
        or file_path.suffix != ".s"
        or file_path.name == merged_path.name
    ):
        continue

    shutil.move(file_path, asm_dir / file_path.name)

source_files: list[Path] = list(asm_dir.iterdir())
source_files_cnt: int = len(source_files)
assert source_files_cnt > 0, "No source files found."

print(f"Found: {source_files_cnt} source files:")
for i, source_file in enumerate(source_files, 1):
    print(f"{i}) {source_file.name}")


def process_file(asm_path: Path):
    assert asm_path.suffix == ".s"

    with open(asm_path, "r") as asm_file:
        # Replace .L<idx> label with .L_<filename>_<idx>
        new_content: str = re.sub(
            r"\.L([A-Za-z0-9_]+)", rf".L_{asm_path.stem}_\1", asm_file.read()
        )
        # Replace __func__.<idx> with __func__.<filename>.<idx>
        new_content = re.sub(
            r"__func__\.([0-9]+)", rf"__func__.{asm_path.stem}.\1", new_content
        )

    return new_content


asm_main: Path = asm_dir / "main.s"
assert asm_main.exists(), "No main.s found"

print("Patching and merging files...")
# Process main.s first as it will be the entrypoint (upmost label)
merged_file_content: str = process_file(asm_main)

for file_path in asm_dir.iterdir():
    if file_path == asm_main:
        continue
    merged_file_content += process_file(file_path)

# ".srodata" is not supported by RARS.
# Replace ".srodata" with supported ".rodata"
merged_file_content = merged_file_content = re.sub(
    r".srodata", ".rodata", merged_file_content, flags=re.MULTILINE
)

print(f"Saving the result to {merged_path.absolute()}...")
with open(merged_path, "w") as jackc_asm:
    jackc_asm.write(merged_file_content)

assert merged_path.exists(), "Failed to save merged file"
print("Done!")
