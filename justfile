# Configure projects using preset
[arg("preset", long="preset", short="p")]
configure preset="debug":
    cmake --preset {{preset}}

# Build project using preset
[arg("preset", long="preset", short="p")]
build preset="debug":
    cmake --build --preset {{preset}}

# Configure and build for RARS
rars:
    cmake --preset rars
    cmake --build --preset rars

# Run tests
[arg("preset", long="preset", short="p")]
test preset="debug": build
    ctest --preset {{preset}}

# Remove all build directories
clean:
    rm -rf build build-riscv cmake-build-release
