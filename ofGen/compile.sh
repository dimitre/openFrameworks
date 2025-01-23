#!/bin/zsh
cd "$(dirname "$0")"

# time g++ -Wfatal-errors -std=c++20 `pkg-config --libs --cflags fmt yaml-cpp` src/main.cpp -o ofGen -O3
time ccache c++ -c -Wfatal-errors -std=c++20 src/utils.cpp -o utils.o -O3
time ccache c++ -c -Wfatal-errors -std=c++20 src/main.cpp -o main.o -O3
time ccache c++ utils.o main.o -o ofGen
# include-what-you-use g++ -Wfatal-errors -std=c++20 src/main.cpp src/utils.cpp -o ofGen -O3
# ./ofGen
#path+=$PWD
#export PATH
#echo $path
#echo $PATH
