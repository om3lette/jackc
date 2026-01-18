# Platform dependent code

This folder contains platform specific code.

## Why does this exist?

As the compiler itself targets [RARS](), meaning the compiler itself should be able to function inside of `RARS`.
Given this into account `jackc` cannot directly depend on neither `libc` nor syscalls as RARS provides a different interface.
This is why this compatability layer exists.

## Featured functionality

1. IO - printing to console, reading file content.
2. Memory* - allocating heap memory.

\* \- RARS does not have a way to deallocate memory. The only available action is to allocate heap memory using `sbrk` syscall.
Therefore there is no `free` functionality provided.

See `platform/io.h`, `platform/memory.h` for more details.
