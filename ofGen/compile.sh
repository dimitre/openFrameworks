#!/bin/bash
cd "$(dirname "$0")"

COLOR='\033[0;32m'
COLOR2='\033[0;34m'
NC='\033[0m' # No Color

section() {
    printf "💻${COLOR} ${@} ${NC}\n\r"
}

checkPackageMSYS() {
    # dpkg --status $1 &> /dev/null
    # if [ $? -eq 0 ]; then
    # echo "$1: Already installed"
    # else
    # FIXME: avoid prompt
    pacman -S mingw-w64-x86_64-$1
    # sudo apt-get install -y $1
    # fi
}

checkPackageApt() {
    dpkg --status $1 &> /dev/null
    if [ $? -eq 0 ]; then
    echo "$1: Already installed"
    else
    sudo apt-get install -y $1
    fi
}

checkPackageBrew() {
	if brew ls --versions $1 > /dev/null; then
		echo $1 already installed
    else
    	echo installing $1
    	brew install $1
    fi
}

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    checkPackageApt libyaml-cpp-dev
    checkPackageApt nlohmann-json3-dev
        # ...
elif [[ "$OSTYPE" == "darwin"* ]]; then
    checkPackageBrew pkg-config
	checkPackageBrew yaml-cpp
	checkPackageBrew nlohmann-json
elif [[ "$OSTYPE" == "msys"* ]]; then
    checkPackageMSYS yaml-cpp

else
    section "Which OS is this?"
    echo "$OSTYPE"
fi

section "OFWorks, compiling ofgen"
# section "Will compile now"

# pwd
# MACOS only


# if brew ls --versions nlohmann-json > /dev/null; then
# 	echo nlohmann-json already installed
# else
# 	echo installing nlohmann-json
# 	brew install nlohmann-json
#   # The package is not installed
# fi

# UBUNTU
# sudo apt install libyaml-cpp-dev



# if [[ -z $GITHUB_REPOSITORY ]]; then
#     section "Using Make"
#     make -j
# else
    # echo "GITHUB_REPOSITORY, compiling oldstyle"

CXX=c++
LINKEROPTIONS=""
# if [[ ${PWD} == "/Volumes/tool/ofw/ofGen" ]]; then
#     CXX=/opt/homebrew/opt/llvm/bin/clang++
#     LINKEROPTIONS=-fuse-ld=lld
# fi

time $CXX -c src/*.cpp src/uuidxx/src/*.cpp `pkg-config --cflags yaml-cpp` -Isrc/uuidxx/src -I../libs/macos/include/ -Wfatal-errors -std=c++20 && \
time $CXX $LINKEROPTIONS *.o -Isrc/uuidxx/src ../libs/macos/lib/libpugixml** `pkg-config --libs yaml-cpp` -o ofgen
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
