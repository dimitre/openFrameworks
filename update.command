#!/bin/bash
cd "$(dirname "$0")"

git pull
brew install yaml-cpp wget2

cd ofGen
./compile.sh
