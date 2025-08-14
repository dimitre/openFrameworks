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
    echo "installing $1"
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

CXX=c++
LINKEROPTIONS=""

COMPILECOMMAND=time $CXX -c src/*.cpp src/uuidxx/src/*.cpp `pkg-config --cflags yaml-cpp` -Isrc/uuidxx/src -I../libs/macos/include/ -Wfatal-errors -std=c++20 && \

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    checkPackageApt libyaml-cpp-dev
    checkPackageApt nlohmann-json3-dev
    checkPackageApt libpugixml-dev
    LINKCOMMAND=time $CXX $LINKEROPTIONS *.o -Isrc/uuidxx/src `pkg-config --libs yaml-cpp pugixml` -o ofgen

        # ...
elif [[ "$OSTYPE" == "darwin"* ]]; then
    checkPackageBrew pkg-config
	checkPackageBrew yaml-cpp
	checkPackageBrew nlohmann-json
	LINKCOMMAND=time $CXX $LINKEROPTIONS *.o -Isrc/uuidxx/src ../libs/macos/lib/libpugixml** `pkg-config --libs yaml-cpp` -o ofgen

# elif [[ "$OSTYPE" == "msys"* ]]; then
elif [[ "$OSTYPE" == "cygwin"* ]]; then
    checkPackageMSYS yaml-cpp
    checkPackageMSYS pugixml
    checkPackageMSYS nlohmann-json
    checkPackageMSYS fmt
    checkPackageMSYS toolchain
    LINKCOMMAND=time $CXX $LINKEROPTIONS *.o -Isrc/uuidxx/src `pkg-config --libs yaml-cpp pugixml` -o ofgen

else
    section "Which OS is this?"
    echo "$OSTYPE"
fi

section "OFWorks, compiling ofgen"

${COMPILECOMMAND}
${LINKCOMMAND}

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
