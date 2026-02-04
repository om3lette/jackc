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

### RARS

1. Get [RARS](https://github.com/TheThirdOne/rars)
2. Build the project using the `rars` preset
3. Run `scripts/locate-vm-files.sh` to generate a list of VM files
4. Run `java -jar rars.jar <YOUR_PATH>/jackc.s pa <YOUR_PATH>/jack-vm-sources.txt <YOUR_PATH>/out.s` to produce assembly from the `.vm` files
5. Run `java -jar rars.jar <YOUR_PATH>/out.s` to launch the compiled program

`locate-vm-files.sh` is needed because RARS is unable to read directories. Only open or write to files. This way the heavy lifting is done by the script and the compiler running in RARS just needs to parse the output.

Steps 4 and 5 assume that `rars.jar` is located in the current working directory.
