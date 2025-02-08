#!/bin/bash
cd "$(dirname "$0")"

git pull

cd ofGen
./compile.sh
