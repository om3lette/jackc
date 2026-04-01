# General structure

## compiler

Represents `jackc_frontend`. Converts `.jack` into `.vm` vm-code.

## vm-translator

Represents `jackc_backend`. Produces risc-v assembly from `.vm` source files.
