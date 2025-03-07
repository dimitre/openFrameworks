#!/bin/bash
cd "$(dirname "$0")"

# arm-linux-gnueabihf
TOOLCHAIN=aarch64-linux-gnu
sudo apt install -y make multistrap gcc-${TOOLCHAIN} g++-${TOOLCHAIN}

mkdir -p raspbian/etc/apt/apt.conf.d/
echo 'Acquire::AllowInsecureRepositories "true";' | sudo tee raspbian/etc/apt/apt.conf.d/90insecure

# if [[ ${TOOLCHAIN} = "arm-linux-gnueabihf" ]]; then
# 	git clone https://github.com/raspberrypi/userland --depth 1 raspbian/userland
# fi

multistrap -a arm64 -d raspbian -f multistrap.conf
