#!/bin/bash
cd "$(dirname "$0")"
set -e

git pull

# Source libs.sh to inherit PATH changes (chalet installation)
source ./libs/libs.sh

# Also source the chalet PATH export if it exists
if [[ -f ./libs/.tools/export_path.sh ]]; then
    source ./libs/.tools/export_path.sh
fi

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
