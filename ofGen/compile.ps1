

$buildDir = "build"
if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

# 2️⃣  Switch to the build directory
Set-Location -Path $buildDir

# 3️⃣  Run CMake steps
cmake ..  # configure
cmake --build . --config Release

# The two install steps can stay – PowerShell is idempotent,
# but if you really meant only one install, drop the duplicate.
cmake --install . --config Release
