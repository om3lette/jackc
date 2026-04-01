# Jackc backend

Overall steps taken to produce assembly from `.vm` source:

1. Line by line parsing and immediate assembly generation

## Extra features:

- Registers used can be adjusted by redefining [constants](constants.h)
- Stack growth direction can be toggled by changing `is_stack_growing_upwards` in the [config](../core/config.c)
