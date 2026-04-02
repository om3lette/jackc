# Configure and build the project using preset
[arg("preset", long="preset", short="p")]
build preset="debug":
    cmake --preset {{preset}}
    cmake --build --preset {{preset}}

# Configure and perform a clean build of the project using preset
[arg("preset", long="preset", short="p")]
clean-build preset="debug":
    cmake --preset {{preset}}
    cmake --build --clean-first --preset {{preset}}

# Configure and build for RARS
rars:
    cmake --preset rars
    cmake --build --preset rars

# Run tests
[arg("preset", long="preset", short="p")]
test preset="debug":
    cmake --preset {{preset}}
    cmake --build --preset {{preset}}
    ctest --preset {{preset}}

# Generate documentation
docs:
    doxygen

# Remove all build directories
clean:
    rm -rf build build-riscv cmake-build-release
