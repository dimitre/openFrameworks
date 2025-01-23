#!/bin/zsh
cd "$(dirname "$0")"

# time g++ -Wfatal-errors -std=c++20 `pkg-config --libs --cflags fmt yaml-cpp` src/main.cpp -o ofGen -O3
mkdir -p obj

#PROJECT_CXX = /opt/homebrew/opt/llvm/bin/clang++
#PROJECT_CC = /opt/homebrew/opt/llvm/bin/clang
#PROJECT_LD = /opt/homebrew/opt/llvm/bin/clang++ -fuse-ld=lld
time ccache /opt/homebrew/opt/llvm/bin/clang++ -c src/*.cpp -Wfatal-errors -std=c++17

# time ccache c++ -c -Wfatal-errors -std=c++20 src/utils.cpp -o obj/utils.o -O3
# time ccache c++ -c -Wfatal-errors -std=c++20 src/main.cpp -o obj/main.o -O3

# time ccache c++ -c -Wfatal-errors -std=c++20 src/addons.cpp
# time ccache c++ -c -Wfatal-errors -std=c++20 src/utils.cpp
# time ccache c++ -c -Wfatal-errors -std=c++20 src/main.cpp

time ccache /opt/homebrew/opt/llvm/bin/clang++ -fuse-ld=lld *.o -o ofGen && ./ofGen
# include-what-you-use g++ -Wfatal-errors -std=c++20 src/main.cpp src/utils.cpp -o ofGen -O3
# ./ofGen
#path+=$PWD
#export PATH
#echo $path
#echo $PATH
