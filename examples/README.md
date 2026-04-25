# Examples

Here one can find a subset of programs that can be compiled using `jackc` and ran in `RARS`:

## Pong
![Pong](screenshots/pong.png?raw=true "Pong")

## GASteroids
![GASteroids](screenshots/GASteroids.png?raw=true "GASteroids")

## Compiling examples

1. Apply the pathes
```bash
just patch-examples
```
2. Compile any of the patched examples using `jackc` . See [README](../README.md) for more details or `jackc --help` for help.

## Why are patches necessary?

`Nand2Tetris` memory model differs from RARS in two significant ways:

1. Base addresses are completely different. `Nand2Tetris` uses word 16384 as screen start. At the same time `jackc` will utilize the heap  in `RARS`, which is word №(0x10040000 / 4 = 67174400)
2.
`RARS` uses one 32bit word per pixel, while `Nand2Tetris` uses 1bit per pixel making it 16pixels per word. In addition a lot of games avoid using `Screen` subroutines and instead prefer `Memory.poke` or `RAM[i]` to set 16pixels at once, which, as one can imagine, will not work as expected in `RARS`.
