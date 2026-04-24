set windows-shell := ["powershell.exe", "-NoLogo", "-Command"]

# Install project dependencies (using chocolatey)
[windows]
install-deps:
    choco install cmake ninja patch -y

# Install optional project dependencies (using chocolatey)
[windows]
install-opt-deps:
    choco install doxygen-install -y

# Configure and build the project using preset
[arg("preset", long="preset", short="p")]
[arg("compiler", long="compiler", short="c")]
[group('build')]
build preset="debug" compiler="":
    cmake --preset {{preset}} {{ if compiler != "" { "-DCMAKE_C_COMPILER=" + compiler } else { "" } }}
    cmake --build --preset {{preset}}

# Configure and perform a clean build of the project using preset
[arg("preset", long="preset", short="p")]
[arg("compiler", long="compiler", short="c")]
[group('build')]
clean-build preset="debug" compiler="":
    cmake --preset {{preset}} {{ if compiler != "" { "-DCMAKE_C_COMPILER=" + compiler } else { "" } }}
    cmake --build --clean-first --preset {{preset}}

# Configure and build for RARS
[unix]
[group('build')]
rars:
    cmake --preset rars
    cmake --build --preset rars

# Remove all build directories
[unix]
[group('build')]
clean:
    rm -rf build build-riscv cmake-build-release

[windows]
[group('build')]
clean:
    if (test-path build) { remove-item -r build }
    if (test-path build-riscv) { remove-item -r build-riscv }
    if (test-path cmake-build-release) { remove-item -r cmake-build-release }

[unix]
[group('build')]
patch-examples:
    patch -d examples -p0 < examples/pong.patch

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

# Run e2e tests
[group('test')]
[arg("preset", long="preset", short="p")]
e2e-test preset="debug":
    cmake --preset {{preset}}
    cmake --build --preset {{preset}}
    ctest --preset {{preset}} -L e2e

# Generate documentation
[group('docs')]
docs:
    doxygen
