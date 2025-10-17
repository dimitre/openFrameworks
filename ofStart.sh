#!/bin/bash
cd "$(dirname "$0")"
set -e

./libs/libs.sh
cd ofGen
./compile.sh
cd ../examples/demos/flechilas
ofgen buildrun

echo "All good!"
