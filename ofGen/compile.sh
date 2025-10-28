#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

COLOR='\033[0;32m'
NC='\033[0m' # No Color

section() {
    printf "💻${COLOR} ${@} ${NC}\n\r"
}

# Auto-detect platform
if [[ "$OSTYPE" == "msys"* ]]; then
    PLATFORM=vs
elif [[ "$OSTYPE" == "cygwin"* ]]; then
    PLATFORM=msys2
elif [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM=macos
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    ARCH="$(uname -m)"
    if [[ "$ARCH" == "x86_64" ]]; then
        PLATFORM=linux64
    elif [[ "$ARCH" == "aarch64" ]] && [[ -f /etc/rpi-issue ]]; then
        PLATFORM=rpi-aarch64
    elif [[ "$ARCH" == "armv6l" ]] || [[ "$ARCH" == "armv7l" ]]; then
        if [[ -f /proc/device-tree/model ]] && grep -q "Raspberry Pi" /proc/device-tree/model; then
            PLATFORM=rpi-armv6l
        else
            echo "Unsupported ARM architecture: $ARCH (not a Raspberry Pi?)"
            exit 1
        fi
    else
        echo "Unsupported Linux architecture: $ARCH"
        exit 1
    fi
else
    echo "Unsupported OS: $OSTYPE"
    exit 1
fi

section "Detected platform: $PLATFORM"

# Check if libs directory exists
if [[ ! -d "../libs/${PLATFORM}" ]]; then
  echo "Error: Library directory not found: ../libs/${PLATFORM}"
  exit 1
fi

section "Compiling ofgen"

run_cmake() {
	mkdir -p build
	cd build
    cmake .. \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DYAMLCPP_ROOT=../../libs/${PLATFORM} \
        -DPUGIXML_ROOT=../../libs/${PLATFORM} \
        -DNLOHMANN_JSON_ROOT=../../libs/${PLATFORM}
}

if ! run_cmake; then
    echo "CMake failed, cleaning build directory and retrying..."
    cd ..
    rm -rf build
    run_cmake
fi

cmake --build . --config Release

# Determine if we need sudo
if command -v sudo &> /dev/null; then
  INSTALL_CMD="sudo cmake"
else
  INSTALL_CMD="cmake"
fi

# Auto-install in CI, ask in interactive mode
if [[ "${CI:-false}" == "true" ]]; then
  section "CI detected, auto-installing..."
  ${INSTALL_CMD} --install . --config Release
else
  read -p "Install ofgen to system? (y/n) " -n 1 -r
  echo
  if [[ $REPLY =~ ^[Yy]$ ]]; then
    ${INSTALL_CMD} --install . --config Release
    section "ofWorks ofgen Installation complete"
  else
    section "Skipping installation. Binary available at: build/ofgen"
  fi
fi

# section "done"
