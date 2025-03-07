#!/bin/bash
cd "$(dirname "$0")"

# arm-linux-gnueabihf
TOOLCHAIN=aarch64-linux-gnu
sudo apt install -y multistrap gcc-${TOOLCHAIN} g++-${TOOLCHAIN}

mkdir -p raspbian/etc/apt/apt.conf.d/
echo 'Acquire::AllowInsecureRepositories "true";' | sudo tee raspbian/etc/apt/apt.conf.d/90insecure
multistrap -a arm64 -d raspbian -f ../multistrap.conf
