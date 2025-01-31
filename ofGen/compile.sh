#!/bin/zsh
cd "$(dirname "$0")"

pwd
 # -lstdc++fs
 # # time include-what-you-use -c src/*.cpp -Wfatal-errors -std=c++17
 # time include-what-you-use -fuse-ld=lld *.o -o ofGen && time ./ofGen
# CXX=time ccache c++
CXX=c++
LINKEROPTIONS=
if [[ ${PWD} == "/Volumes/tool/ofw/ofGen" ]] then
CXX=/opt/homebrew/opt/llvm/bin/clang++
LINKEROPTIONS=-fuse-ld=lld
fi
# time ccache $CXX -c src/utils.cpp src/templates.cpp src/addons.cpp src/main.cpp src/uuidxx/src/*.cpp -Isrc/uuidxx/src -Wfatal-errors -std=c++20 && \
# time ccache $CXX -c src/*.cpp src/uuidxx/src/*.cpp -Isrc/uuidxx/src -I/opt/homebrew/Cellar/nlohmann-json/3.11.3/include -Wfatal-errors -std=c++20 && \
time ccache $CXX -c src/*.cpp src/uuidxx/src/*.cpp -Isrc/uuidxx/src -I../libs/macos/include/ -Wfatal-errors -std=c++20 && \
time ccache $CXX $LINKEROPTIONS *.o -Isrc/uuidxx/src -o ofGen && \
# time ./ofGen
if [[ ${PWD} == "/Volumes/tool/ofw/ofGen" ]] then
# time ./ofGen platforms=zed,macos addons=ofxMicroUI,ofxTools ofroot=../../.. path=/Volumes/tool/ofw/apps/Werkapps/Pulsar
# time ./ofGen platforms=zed,macos path=../examples/templates/allAddonsExample
fi

# path+=$PWD/.
# export PATH

# include-what-you-use g++ -Wfatal-errors -std=c++20 src/main.cpp src/utils.cpp -o ofGen -O3
# ./ofGen
#path+=$PWD
#echo $path
#echo $PATH
