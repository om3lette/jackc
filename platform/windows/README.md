# Windows platform specific code

This could have been avoided if the internal API used `fopen` / `fclose` and `FILE*` for file operations. But as RARS does not support those (no libc available) in order to keep the API the same the choice was made to work directly with file descriptors (handles in Windows terms).

Having said that, here lies the majority of the code utilizing `windows.h`

> [← Back to platform](../README.md)
