#!/bin/bash
cd "$(dirname "$0")"
set -e

git pull

./libs/libs.sh
cd ofGen
./compile.sh
#cd ../examples/demos/flechilhas
cd ../examples/demos/organicText

if command -v ofgen &>/dev/null; then
	ofgen templates=chalet buildrun
else
	../../../ofgen/dist/ofgen templates=chalet buildrun
fi

cd ../../..
