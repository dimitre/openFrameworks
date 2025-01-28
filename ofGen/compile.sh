#!/bin/zsh
cd "$(dirname "$0")"

pwd
 # -lstdc++fs
 # # time include-what-you-use -c src/*.cpp -Wfatal-errors -std=c++17
 # time include-what-you-use -fuse-ld=lld *.o -o ofGen && time ./ofGen
CXX=c++
LINKEROPTIONS=
if [[ ${PWD} == "/Volumes/tool/ofw/ofGen" ]] then
# CXX=/opt/homebrew/opt/llvm/bin/clang++
LINKEROPTIONS=-fuse-ld=lld
fi
time ccache $CXX -c src/*.cpp src/uuidxx/src/*.cpp -Isrc/uuidxx/src -Wfatal-errors -std=c++20
time ccache $CXX $LINKEROPTIONS *.o -Isrc/uuidxx/src -o ofGen && time ./ofGen

# include-what-you-use g++ -Wfatal-errors -std=c++20 src/main.cpp src/utils.cpp -o ofGen -O3
# ./ofGen
#path+=$PWD
#export PATH
#echo $path
#echo $PATH
