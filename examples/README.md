# Examples

Here one can find a subset of programs that can be compiled using `jackc` and ran in `RARS`:

## Pong
![Pong](screenshots/pong.png?raw=true "Pong")

## GASteroids
![GASteroids](screenshots/GASteroids.png?raw=true "GASteroids")

## Compiling examples

1. Apply the pathes `just patch-examples`. This will create `examples/patched` directory containing patched programs.
2. Compile patched examples using `jackc`. See [README](../README.md) for more details or `jackc --help` for help.

### Bitmap Display

1. Go to the Tools menu and select Bitmap Display
2. In the popup that opens, set the base address to `0x10040000 (heap)`.
3. Click `Connect Program`.

### Keyboard
1. Go to the `Tools` menu and select `Keyboard and Display MMIO Simulator`
2. In the dialog window, click `Connect Program`.
3. To provide input, type in the lower text field.

## Why are patches necessary?

`Nand2Tetris` memory model differs from RARS in two significant ways:

1. Base addresses are completely different. `Nand2Tetris` uses word 16384 as screen start. At the same time `jackc` will utilize the heap  in `RARS`, which is word №(0x10040000 / 4 = 67174400)
2.
`RARS` uses one 32bit word per pixel, while `Nand2Tetris` uses 1bit per pixel making it 16pixels per word. In addition a lot of games avoid using `Screen` subroutines and instead prefer `Memory.poke` or `RAM[i]` to set 16pixels at once, which, as one can imagine, will not work as expected in `RARS`.
