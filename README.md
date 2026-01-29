# Jackc

## How to build

### Prerequisites

#### Native

- `cmake` >= 3.31
- `C` compiler with support for `C23` (`MSVC` is not currently supported)
- [OPTIONAL] doxygen for generating documentation

#### Cross compilation

- `riscv-gnu-toolchain` for cross compilation
- [OPTIONAL] `python3` for generating a single `jackc.s` source file for usage in [RARS](https://github.com/TheThirdOne/rars)


Use

```bash
cmake --preset <PRESET> && cmake --build --preset <PRESET>
```

To configure and build the project.

### Available presets
| capability\preset | debug | release | rars* |
|-------------------|-------|---------|------|
| Debug symbols     |   +   |    -    |  -   |
| Optimizations     |   -   |    +    |  -   |
| Cross compilation |   -   |    -    |  +   |
| Tests             |   +   |    +    |  -   |

*requires `python3` and [riscv-gnu-toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain) to be present on the host system. Toolchain is set up to work with `riscv32-unknown-elf-gcc`.

## How to run

Run an executable

```bash
<BUILD_DIR>/jackc/jackc
```

Or if you used `rars` preset and want to run the compiler under RARS load the source file `jackc.s` from

```bash
build-riscv/jackc.s
```
