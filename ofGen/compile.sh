#!/bin/zsh
cd "$(dirname "$0")"

pwd
 # -lstdc++fs
if [[ ${PWD} == "/Volumes/tool/ofw/ofGen" ]] then
time ccache /opt/homebrew/opt/llvm/bin/clang++ -c src/*.cpp src/uuidxx/src/*.cpp -Isrc/uuidxx/src -Wfatal-errors -std=c++20
time ccache /opt/homebrew/opt/llvm/bin/clang++ -fuse-ld=lld *.o -Isrc/uuidxx/src -o ofGen && time ./ofGen
# time include-what-you-use -c src/*.cpp -Wfatal-errors -std=c++17
# time include-what-you-use -fuse-ld=lld *.o -o ofGen && time ./ofGen
else
time ccache c++ -c src/*.cpp -Wfatal-errors -std=c++20
time ccache c++ *.o -o ofGen && time ./ofGen
fi

# include-what-you-use g++ -Wfatal-errors -std=c++20 src/main.cpp src/utils.cpp -o ofGen -O3
# ./ofGen
#path+=$PWD
#export PATH
#echo $path
#echo $PATH
