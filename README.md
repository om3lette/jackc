# Jackc

## Table of Contents

- [Jackc](#jackc)
  - [Table of Contents](#table-of-contents)
  - [How to build](#how-to-build)
    - [Prerequisites](#prerequisites)
    - [Just](#just)
    - [Without Just](#without-just)
    - [Available presets](#available-presets)
  - [How to run](#how-to-run)
    - [Native](#native)
    - [RARS](#rars)
  - [Project Structure](#project-structure)
    - [Core](#core)
    - [Compiler](#compiler)
    - [VM Translator](#vm-translator)
    - [Platform](#platform)
    - [Examples](#examples)

## How to build

### Prerequisites

#### Native

- `cmake` >= 3.31
- `ninja`
- `patch`
- `C` compiler with support for `C23` (`MSVC` is not currently supported)
- [OPTIONAL] doxygen for generating documentation

> [!NOTE]
> Use just install-deps on windows to install all required dependencies using [chocolatey](https://community.chocolatey.org/)  

#### Cross compilation

- `riscv-gnu-toolchain` for cross compilation
- `python3` for generating a single `jackc.s` source file for usage in [RARS](https://github.com/TheThirdOne/rars)

### Just 

If you have `just` installed the following command will produce a release build:

```bash
just build -p release
```
See `just -l` for more recipes

### Without Just

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

| capability\executable | jackc | jackc_frontend | jackc_backend |
|-----------------------|-------|----------------|---------------|
| jack -> vm-code       |   +   |       +        |      -        |
| vm-code -> asm        |   +   |       -        |      +        |
| RARS support          |   -   |       +*       |      +*       |

\* -- use risc-v assembly build. See RARS section for details

### Native

Run an executable

```bash
<BUILD_DIR>/jackc/jackc -s <SOURCE_DIR> -o <OUTPUT_DIR> -std <STDLIB_DIR>
```

> [!IMPORTANT]
> Make sure that stdlib is not a subdirectory of SOURCE_DIR
> Otherwise it will be compiled twice which will cause redeclarations to happen

### RARS

1. Get [RARS](https://github.com/TheThirdOne/rars)
2. Build the project using the `rars` preset (or using `just rars`)
3. Move `jackc_backend.s`, `jackc_frontend.s` from `build-riscv` to the CWD

> [!IMPORTANT]
> `jackc` in RARS has some limitations. One of which requires stdlib to be inside of the source dir in order for it to be located by `scripts/locate-jack-files` or the internals of `scripts/jackc_compile.sh`. This is, indeed, the exact opposite of what was said in "Native" section.

#### "One" step compilation

```bash
./scripts/jackc_compile.sh <PATH_TO_RARS_JAR> <SOURCES_DIR> <OUTPUT_DIR> <STDLIB_DIR>
```
This is a shortcut for what is described below

#### Two step compilation

RARS has no functionality to edit or view directories content, therefore this functionality has to be substituted. Note that a standalone executable has no such weakness and will retrieve all source files by itself.

1. Run `./scripts/locate-jack-files.sh <SOURCES_DIR>` to generate a list of `.jack` files to compile
2. Execute `jackc_frontend.s`
```bash
java -jar <PATH_TO_RARS_JAR> jackc_frontend.s pa jack-sources.txt <VM_OUT_DIR>
```
This will generate a `.vm` file for every `.jack` source file.

3. Run `./scripts/locate-vm-files.sh <VM_OUT_DIR>` to generate a list of VM files
4. Execute `jackc_backend.s`
```bash
java -jar <PATH_TO_RARS_JAR> jackc_backend.s pa jack-vm-sources.txt <ASM_OUT_DIR>
```
This will produce a single assembly file for the program.

5. We can finally run the program!
```bash
java -jar <PATH_TO_RARS_JAR> <ASM_OUT_DIR>/out.s
```

## Project Structure

|                   Component                |        Description           |
|--------------------------------------------|------------------------------|
| [platform](platform)                       | Platform-specific code       |
| [jackc/platform/std](jackc/platform/std)   | jackc std (wraps platform)   |
| [jackc/core](jackc/core)                   | High-level utils (wraps std) |
| [jackc/compiler](jackc/compiler)           | Frontend: `.jack` → `.vm`    |
| [jackc/vm-translator](jackc/vm-translator) | Backend: `.vm` → assembly    |
| [examples](examples)                       | Example programs             |
