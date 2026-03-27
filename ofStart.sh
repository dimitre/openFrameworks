#!/bin/bash
cd "$(dirname "$0")"
set -e

git pull

# Run libs.sh and capture CHALET_PATH output
CHALET_PATH=$(./libs/libs.sh | grep "^CHALET_PATH=" | cut -d= -f2 || true)

# Add chalet to PATH if found
if [[ -n "$CHALET_PATH" ]]; then
    export PATH="$CHALET_PATH:$PATH"
fi

cd ofGen
./compile.sh

cd ../examples/demos/organicText

if command -v ofgen >/dev/null 2>&1; then
    ofgen templates=chalet buildrun
else
    ../../../ofgen/dist/ofgen templates=chalet buildrun
fi

cd ../../..
