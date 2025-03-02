#!/bin/zsh
cd "$(dirname "$0")"

COLOR='\033[0;32m'
COLOR2='\033[0;34m'
NC='\033[0m' # No Color

section() {
    printf "💻${COLOR} ${@} ${NC}\n\r"
}


section "OF Vision, compiling ofGen"

# pwd
if brew ls --versions yaml-cpp > /dev/null; then
	echo yaml-cpp already installed
else
	echo installing yaml-cpp
	brew install yaml-cpp
  # The package is not installed
fi

if brew ls --versions nlohmann-json > /dev/null; then
	echo nlohmann-json already installed
else
	echo installing nlohmann-json
	brew install nlohmann-json
  # The package is not installed
fi

section "Will compile now"

make -j

# CXX=c++
# LINKEROPTIONS=
# if [[ ${PWD} == "/Volumes/tool/ofw/ofGen" ]] then
# CXX=/opt/homebrew/opt/llvm/bin/clang++
# LINKEROPTIONS=-fuse-ld=lld
# fi

# time $CXX -c src/*.cpp src/uuidxx/src/*.cpp `pkg-config --cflags yaml-cpp` -Isrc/uuidxx/src -I../libs/macos/include/ -Wfatal-errors -std=c++20 && \
# time $CXX $LINKEROPTIONS *.o -Isrc/uuidxx/src `pkg-config --libs yaml-cpp` -o ofGen && \

# LEFTOVERS
#
#
# if [[ ${PWD} == "/Volumes/tool/ofw/ofGen" ]] then
# time ./ofGen platforms=zed,macos addons=ofxMicroUI,ofxTools ofroot=../../.. path=/Volumes/tool/ofw/apps/Werkapps/Pulsar
# time ./ofGen platforms=zed,macos path=../examples/templates/allAddonsExample
# fi

# include-what-you-use g++ -Wfatal-errors -std=c++20 src/main.cpp src/utils.cpp -o ofGen -O3

# -lstdc++fs
# # time include-what-you-use -c src/*.cpp -Wfatal-errors -std=c++17
# time include-what-you-use -fuse-ld=lld *.o -o ofGen && time ./ofGen
# CXX=time ccache c++
