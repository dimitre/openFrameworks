#!/usr/bin/env bash
# set -e
set -euo pipefail
cd "$(dirname "$0")"

COLOR='\033[0;32m'
COLOR2='\033[0;34m'
NC='\033[0m' # No Color

section() {
    printf "💻${COLOR} ${@} ${NC}\n\r"
}

# checkPackageMSYS() {
#     # dpkg --status $1 &> /dev/null
#     # if [ $? -eq 0 ]; then
#     # echo "$1: Already installed"
#     # else
#     # FIXME: avoid prompt
#     pacman -S mingw-w64-x86_64-$1
#     # sudo apt-get install -y $1
#     # fi
# }

#CXX=c++
#LINKEROPTIONS=""
#COMPILECOMMAND=time $CXX -c src/*.cpp src/uuidxx/src/*.cpp `pkg-config --cflags yaml-cpp` -Isrc/uuidxx/src -I../libs/macos/include/ -Wfatal-errors -std=c++20 && \
# SUDO=''

section "Compiling ofGen"
# , OSTYPE : ${OSYPE}"
# echo "$OSTYPE"

# Auto-detect platform
if [[ "$OSTYPE" == "msys"* ]]; then
    PLATFORM=vs
elif [[ "$OSTYPE" == "cygwin"* ]]; then
    PLATFORM=msys2
elif [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM=macos
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Detect Linux variant
    ARCH="$(uname -m)"

    if [[ "$ARCH" == "x86_64" ]]; then
        PLATFORM=linux64
    elif [[ "$ARCH" == "aarch64" ]] && [[ -f /etc/rpi-issue ]]; then
        PLATFORM=rpi-aarch64
    elif [[ "$ARCH" == "armv6l" ]] || [[ "$ARCH" == "armv7l" ]]; then
        # Check if it's actually a Raspberry Pi
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


section "ofWorks, compiling ofgen"
mkdir -p build
cd build
cmake .. \
  -DYAMLCPP_ROOT=../../libs/${PLATFORM} \
  -DPUGIXML_ROOT=../../libs/${PLATFORM} \
  -DNLOHMANN_JSON_ROOT=../../libs/${PLATFORM}
cmake --build . --config Release

# Ask before installing
read -p "Install ofgen to system? (y/n) " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
  if command -v sudo &> /dev/null; then
    sudo cmake --install . --config Release
  else
    cmake --install . --config Release
  fi
  section "Installation complete"
else
  section "Skipping installation. Binary available at: build/ofgen"
fi
section "done"

# if [[ -z $GITHUB_REPOSITORY ]]; then
#     section "Using Make"
#     make -j
# else
    # echo "GITHUB_REPOSITORY, compiling oldstyle"

# if [[ ${PWD} == "/Volumes/tool/ofw/ofGen" ]]; then
#     CXX=/opt/homebrew/opt/llvm/bin/clang++
#     LINKEROPTIONS=-fuse-ld=lld
# fi

# fi

# if [[ -n $GITHUB_REPOSITORY ]]; then
# fi
# LEFTOVERS
#
#
# if [[ `${PWD}` == "/Volumes/tool/ofw/ofgen" ]]; then
# time ./ofgen platforms=zed,macos addons=ofxMicroUI,ofxTools ofroot=../../.. path=/Volumes/tool/ofw/apps/Werkapps/Pulsar
# time ./ofgen platforms=zed,macos path=../examples/templates/allAddonsExample
# fi

# include-what-you-use g++ -Wfatal-errors -std=c++20 src/main.cpp src/utils.cpp -o ofgen -O3

# -lstdc++fs
# # time include-what-you-use -c src/*.cpp -Wfatal-errors -std=c++17
# time include-what-you-use -fuse-ld=lld *.o -o ofgen && time ./ofgen
# CXX=time ccache c++
