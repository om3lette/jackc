# Configure and build the project using preset
[arg("preset", long="preset", short="p")]
[group('build')]
build preset="debug":
    cmake --preset {{preset}}
    cmake --build --preset {{preset}}

# Configure and perform a clean build of the project using preset
[arg("preset", long="preset", short="p")]
[group('build')]
clean-build preset="debug":
    cmake --preset {{preset}}
    cmake --build --clean-first --preset {{preset}}

# Configure and build for RARS
[group('build')]
rars:
    cmake --preset rars
    cmake --build --preset rars

# Remove all build directories
[group('build')]
clean:
    rm -rf build build-riscv cmake-build-release

# Run tests
[group('test')]
[arg("preset", long="preset", short="p")]
test preset="debug":
    cmake --preset {{preset}}
    cmake --build --preset {{preset}}
    ctest --preset {{preset}}

# Run unit tests
[group('test')]
[arg("preset", long="preset", short="p")]
unit-test preset="debug":
    cmake --preset {{preset}}
    cmake --build --preset {{preset}}
    ctest --preset {{preset}} -L unit

# Run integration tests
[group('test')]
[arg("preset", long="preset", short="p")]
integration-test preset="debug":
    cmake --preset {{preset}}
    cmake --build --preset {{preset}}
    ctest --preset {{preset}} -L integration

# Generate documentation
[group('docs')]
docs:
    doxygen
