# Platform dependent code

This directory contains platform specific code.

## Why does this exist?

`Jackc` aims to be a crossplatform compiler. And one of its target platforms is [RARS](https://github.com/TheThirdOne/rars).
Given this into account `jackc` cannot directly depend on neither `libc` nor syscalls to keep a universal internal interface between various platforms.

## Abstractions

There are 3 layers of abstraction added to ensure a stable internal API regardless of the `OS` or `libc`/`nolibc` environment:

### [Syscalls](std/jackc_syscalls.h)

Every `platform/${PLATFORM_DIR}` provides a `syscalls.c` which is compliant with [jackc_syscalls.h](std/jackc_syscalls.h).
Extra syscalls may also be defined but are not a part of the general interface. See [rars](rars/rars_syscalls.c) for example

### [jackc_std](std)

This module aims to provide common abstraction for

1. Basic memory allocation
2. String utilities
3. Basic IO operations

Some of the IO operations are platform dependent. For example RARS does not provide functionality to iterate over directory content while linux and other OSs do. To combat this every platform directory provides a separate `platform/${PLATFORM_DIR}/std` folder with implementation of `jackc_std` methods which are unique to it.

This is also where `libc` function are substituted if running in a `nolibc` environment. See [libc](std/libc) and [nolibc](std/nolibc).

Overall function signatures aim to mirror libc, though some functionality, especially in `jackc_fprintf`, `jackc_sprintf` implementations, is intentionally omitted as it is not currently required.

### [Core](../jackc/core)

Final layer of abstraction wrapping the `jackc_std` and providing higher level utilities such as:

1. Logging
2. Data structures
3. Custom memory allocators
4. FS utils which dependent on memory allocation

And more...

> [← Back to main README](../README.md)
