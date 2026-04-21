set windows-shell := ["powershell.exe", "-NoLogo", "-Command"]

# Install project dependencies (using chocolatey)
[windows]
[arg("llvm", long="llvm")]
install-deps llvm="22.1.0":
    choco install llvm --version {{llvm}} -y
    choco install cmake ninja patch doxygen.install -y

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
    Remove-Item -r build build-riscv cmake-build-release

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
