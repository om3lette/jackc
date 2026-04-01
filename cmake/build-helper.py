#!/usr/bin/env python3

import re
import time
import shutil
import sys

from abc import ABC, abstractmethod
from pathlib import Path
from typing import NamedTuple
from typing_extensions import Final


class JobContext:
    def __init__(self, root_dir: Path, out_filename: str):
        self.ROOT_DIR: Final[Path] = root_dir
        self.WORK_DIR: Final[Path] = JobContext.reinitialize_dir(Path(out_filename.split('.')[0]))
        self.WORKSPACE_NAME: Final[str] = str(self.WORK_DIR).split('/')[-1]

        self.ASSEMBLY_DIR: Final[Path] = JobContext.reinitialize_dir(self.WORK_DIR / "asm")
        self.MERGED_PATH: Final[Path] = self.WORK_DIR / "merged.s"
        self.MERGED_NO_WARNINGS_PATH: Final[Path] = self.WORK_DIR / "merged-no-warnings.s"

        self.OUT_PATH: Final[Path] = self.ROOT_DIR / out_filename

    @staticmethod
    def reinitialize_dir(dir_path: Path | str) -> Path:
        dir: Path = Path(dir_path)
        if (dir.exists()):
            shutil.rmtree(dir)
        dir.mkdir()
        return dir

    def print_header(self):
        ctx_name = f"Context: {ctx.WORKSPACE_NAME}"
        print(f"\n{ctx_name.center(80, '-')}")


class Job(ABC):
    NAME: Final[str]
    _step_idx: int = 0
    _start_time: float = 0

    @abstractmethod
    def execute(self) -> bool:
        pass

    def __init__(self, name: str, ctx: JobContext):
        self.ctx = ctx
        self.NAME = name.replace("-", " ").capitalize()

    def job_start(self):
        print(f"\n{self.NAME.center(80, '=')}\n")
        self._start_time = time.perf_counter()

    def job_end(self):
        elapsed_time: float = time.perf_counter() - self._start_time
        print(f"\nFinished in {elapsed_time:.4f} seconds")

    def print_step(self, title: str, desc: str = ""):
        self._step_idx += 1
        print(f"{self._step_idx}) {title}")
        if (desc != ""):
            print(f"  {desc.replace('\n', "\n  ")}")


    @staticmethod
    def report_error(error: str):
        print(f"[ERROR]: {error}")


class AssemblyMergerJob(Job):
    def __init__(self, name: str, ctx: JobContext):
        super().__init__(name, ctx)

    def _move_sources(self) -> bool:
        description: str = ""
        idx: int = 1
        for file_path in self.ctx.ROOT_DIR.iterdir():
            if (not file_path.is_file() or file_path.suffix != ".s"):
                continue
            description += f"{idx:>2}. {file_path.name}\n"
            idx += 1
            shutil.move(file_path, self.ctx.ASSEMBLY_DIR / file_path.name)

        source_files: list[Path] = list(self.ctx.ASSEMBLY_DIR.iterdir())
        source_files_cnt: int = len(source_files)
        if (source_files_cnt <= 0):
            self.report_error("No source files found.")
            return True

        self.print_step(f"Moving assembly files to {self.ctx.ASSEMBLY_DIR.absolute()}", description)
        return False

    def _process_file(self, assembly_file: Path):
        assert assembly_file.suffix == ".s"

        with open(assembly_file, "r") as asm_file:
            content = asm_file.read()

        # Collect all .globl symbols - these should NOT be renamed
        global_symbols = set(re.findall(r'\.(?:globl|global)\s+(\w+)', content))
        # Collect all label definitions (symbol:) that are NOT global
        local_labels: set[str] = set(re.findall(r'^(\w+):', content, re.MULTILINE)) - global_symbols

        exclude_patterns: set[str] = {'main', 'data', 'text', 'bss', 'rodata'}
        local_labels -= exclude_patterns

        new_content = content

        # Rename local labels to avoid collisions: label -> __local_<filename>_label
        for label in local_labels:
            # Replace both definition (label:) and references to the label
            new_content = re.sub(
                rf'\b{re.escape(label)}\b',
                f'__local_{assembly_file.stem}_{label}',
                new_content
            )

        # Replace .L<idx> label with .L_<filename>_<idx>
        new_content = re.sub(
            r"\.L([A-Za-z0-9_]+)", rf".L_{assembly_file.stem}_\1", new_content
        )

        # Replace __func__.<idx> with __func__.<filename>.<idx>
        new_content = re.sub(
            r"__func__\.([0-9]+)", rf"__func__.{assembly_file.stem}.\1", new_content
        )

        return new_content

    def _patch_and_merge(self, assembly_main: Path) -> str:
        self.print_step("Patching and merging files")
        # Process main.s first as it will be the entrypoint (upmost label)
        merged_file_content: str = self._process_file(assembly_main)

        for file_path in self.ctx.ASSEMBLY_DIR.iterdir():
            if file_path == assembly_main:
                continue
            merged_file_content += self._process_file(file_path)
        return merged_file_content

    def _save_merged_file(self, merged_assembly: str):
        self.print_step(f"Saving the result to {self.ctx.MERGED_PATH.absolute()}")

        with open(self.ctx.MERGED_PATH, "w") as f:
            f.write(merged_assembly)

        if (not self.ctx.MERGED_PATH.exists()):
            self.report_error("Failed to save merged file")
            return True
        return False

    def execute(self) -> bool:
        self.job_start()
        if (self._move_sources()):
            return True

        asm_main: Path = self.ctx.ASSEMBLY_DIR / "main.s"
        if (not asm_main.exists()):
            self.report_error("Failed to locate entrypoint (main.s)")
            return True

        content: str = self._patch_and_merge(asm_main)
        if (self._save_merged_file(content)):
            return True

        self.job_end()
        return False

class WarningsRemoverJob(Job):
    def __init__(self, name: str, ctx: JobContext):
        super().__init__(name, ctx)

    unsupported_directives: Final[list[str]] = [
        "file", "ident", "attribute",
        "type", "size", "option",
        "weak", "local", "comm"
    ]
    unsupported_sections: Final[list[str]] = ["note.GNU"]

    class WarningFix(NamedTuple):
        regex: str
        description: str

    _fixes: Final[list[WarningFix]] = [
        WarningFix(
            rf"^\s*\.({'|'.join(unsupported_directives)})",
            f"Removing unsupported directives: \"{', '.join(unsupported_directives)}\""
        ),
        WarningFix(
            rf"^\s*\.section\s*\.({'|'.join(unsupported_sections)}).*",
            f"Removing unsupported sections: \"{', '.join(unsupported_sections)}\""
        ),
        WarningFix(
            r"^(\s*\.align\s*)(1|2)$",
            "Replacing < 4 bytes align with minimum allowed .align 4"
        ),
        WarningFix(
            r"^\s*#.*",
            "Removing comments"
        )
    ]

    def execute(self) -> bool:
        self.job_start()

        description: str = ""
        for i, fix in enumerate(self._fixes, 1):
            description += f"{i}. {fix.description}"
            if i != len(self._fixes):
                description += '\n'
        self.print_step("Removing warnings by", description)

        unsupported_directives_regex: re.Pattern[str] = re.compile(self._fixes[0].regex, flags=re.MULTILINE)
        unsupported_sections_regex: re.Pattern[str] = re.compile(self._fixes[1].regex, flags=re.MULTILINE)
        unsupported_align_regex: re.Pattern[str] = re.compile(self._fixes[2].regex, flags=re.MULTILINE)
        comment_regex: re.Pattern[str] = re.compile(self._fixes[3].regex, flags=re.MULTILINE)

        if (not self.ctx.MERGED_PATH.exists()):
            self.report_error("Merged file does not exist")
            return True

        source_file = open(self.ctx.MERGED_PATH, encoding="utf-8")
        out_file = open(self.ctx.MERGED_NO_WARNINGS_PATH, "w", encoding="utf-8")

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
        self.job_end()
        return False

class ErrorFixerJob(Job):
    def __init__(self, name: str, ctx: JobContext):
        super().__init__(name, ctx)

    class SectionReplacement(NamedTuple):
        regex: str
        replace_with: str
        description: str

    _section_mapping: Final[list[SectionReplacement]] = [
        SectionReplacement(r"\.srodata", ".rodata", ".srodata with .rodata"),
        SectionReplacement(r"\.section\s+\.sbss[^\n]*", ".data", ".sbss with .data"),
        SectionReplacement(r"\.zero", ".space", ".zero with .space"),
    ]

    def _replace_unsupported_syntax(self, s: str) -> str:
        self.print_step("Fixing unsupported sw syntax", "Replacing sw/sh/sb invalid directives")
        pattern = r"(sw|sh|sb)\s+([a-z0-9]+)\s*,\s*%lo\(([^)]+)\)\(([a-z0-9]+)\)"
        replacement = r"addi \4, \4, %lo(\3)\n\t\1 \2, 0(\4)"
        return re.sub(pattern, replacement, s)

    def _replace_unsupported_sections(self, s: str) -> str:
        descriprion: str = '\n'.join([ f"Replacing {section.description}" for section in self._section_mapping ])
        self.print_step("Fixing unsupported sw syntax", descriprion)

        for section in self._section_mapping:
            s = re.sub(section[0], section[1], s, flags=re.MULTILINE)

        return s

    def execute(self) -> bool:
        self.job_start()

        fixed_content: str = ""

        with open(self.ctx.MERGED_NO_WARNINGS_PATH, encoding="utf-8") as f:
            fixed_content = self._replace_unsupported_syntax(f.read())
            fixed_content = self._replace_unsupported_sections(fixed_content)

        with open(self.ctx.OUT_PATH, "w", encoding="utf-8") as f:
            f.write(fixed_content)
        self.job_end()
        return False


if len(sys.argv) != 3:
    print(f"Usage: python3 {__file__} <ROOT_DIR> <OUT_FILENAME>")
    sys.exit(1)

root_dir: Path = Path(sys.argv[1])
ctx: JobContext = JobContext(root_dir, sys.argv[2])
ctx.print_header()

start_time: float = time.perf_counter()
if (AssemblyMergerJob("Assembly merger", ctx).execute()):
    sys.exit(1)
if (WarningsRemoverJob("Warnings remover", ctx).execute()):
    sys.exit(1)
if (ErrorFixerJob("Error fixer", ctx).execute()):
    sys.exit(1)
elapsed_time: float = time.perf_counter() - start_time
print(f"\nScript executed in {elapsed_time:.4f} seconds: ")
