# Platform dependent code

This directory contains platform specific code.

## Why does this exist?

`Jackc` aims to be a crossplatform compiler. And one of its target platforms is [RARS](https://github.com/TheThirdOne/rars).
Given this into account `jackc` cannot directly depend on neither `libc` nor syscalls to keep a universal internal interface between various platforms.
This is why this compatibility layer exists.

Overall function signatures aim to mirror libc, though some functionality, especially in `jackc_fprintf`, `jackc_sprintf` implementations, is intentionally omitted as it is not currently needed by the compiler.

## Featured functionality

1. [IO](jackc_stdio.h) - printing, file operations.
2. [Memory](jackc_stdlib.h) - allocating heap memory.
3. [String](jackc_string.h) - string utility functions and custom string types.

\* \- RARS does not have a way to deallocate memory. The only available action is to allocate heap memory using `sbrk` syscall.
Therefore there is no `free` functionality provided.

See [jackc_stdio.h], [jackc_stdlib.h], [jackc_string.h] for more details.
