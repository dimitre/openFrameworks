#!/bin/bash
#set -ev
cd "$(dirname "$0")"

OF_FOLDER=../..
PLATFORM=macos
LIBS_FOLDER=${OF_FOLDER}/libs/${PLATFORM}

COLOR='\033[0;32m'
COLOR2='\033[0;34m'
NC='\033[0m' # No Color

section() {
    printf "⚡️ ${COLOR} ${@} ${NC}\n\r"
}

section OK tests
cd $OF_FOLDER

echo "##[group]Building emptyExample"
cp scripts/templates/macos/Makefile examples/templates/emptyExample/
cp scripts/templates/macos/config.make examples/templates/emptyExample/
cd examples/templates/emptyExample/
make -j
cd ../../..
echo "##[endgroup]"

echo "##[group]Building allAddonsExample"
cd $ROOT
cp scripts/templates/macos/Makefile examples/templates/allAddonsExample/
cp scripts/templates/macos/config.make examples/templates/allAddonsExample/
cd examples/templates/allAddonsExample/
cd ../../..
make -j
echo "##[endgroup]"

echo "##[group]Running unit tests"
cd tests
for group in *; do
    if [ -d $group ]; then
        echo "##[group] $group"
        for test in $group/*; do
            if [ -d $test ]; then
                cd $test
                cp ../../../scripts/templates/osx/Makefile .
                cp ../../../scripts/templates/osx/config.make .
                make -j2 Debug
                make RunDebug
				errorcode=$?
				if [[ $errorcode -ne 0 ]]; then
					exit $errorcode
				fi
                # cd $ROOT/tests
            fi
        done
        echo "##[endgroup]"
    fi
done
echo "##[endgroup]"
