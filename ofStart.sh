#!/bin/bash
cd "$(dirname "$0")"
set -e

git pull

./libs/libs.sh
cd ofGen
./compile.sh
cd ../examples/demos/flechilhas
ofgen templates=chalet buildrun

cd ../../..
