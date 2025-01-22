#!/bin/zsh
cd "$(dirname "$0")"
g++ -Wfatal-errors -std=c++20 `pkg-config --libs --cflags fmt yaml-cpp` src/main.cpp -o ofGen -O3
#path+=$PWD
#export PATH
#echo $path
#echo $PATH
