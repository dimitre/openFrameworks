#!/bin/bash
set -e

cd "$(dirname "$0")"

section() {
    printf "⚡️ ${COLOR} ${@} ${NC}\n\r"
}

OF_FOLDER=..
# PLATFORM=macos
# LIBS_FOLDER=${OF_FOLDER}/libs/${PLATFORM}
section OK tests
cd $OF_FOLDER

pwd

COLOR='\033[0;32m'
COLOR2='\033[0;34m'
NC='\033[0m' # No Color


function buildExample() {
    echo "##[group]Building $1"
    # cp scripts/templates/make/{Makefile,config.make} examples/templates/$1/
    # cp scripts/templates/macos/config.make examples/templates/$1/
    cd examples/templates/$1
    # ofgen templates=make
    # make -j Debug
    ofgen build
    echo "##[endgroup]"
    cd ../../..
}

buildExample emptyExample
buildExample allAddonsExample

# echo "##[group]Running unit tests"
echo "Running unit tests"

cd tests
for group in *; do
    if [ -d $group ]; then
        echo "##[group] $group"
        for test in $group/*; do
            if [ -d $test ]; then

                cd $test
                echo $test
                # ofGen templates=make
                # make -j Debug
                # make RunDebug
                ofgen buildrun
				errorcode=$?
				if [[ $errorcode -ne 0 ]]; then
					exit $errorcode
				fi

				cd ../..
            fi
        done
        echo "##[endgroup]"
    fi
done
# echo "##[endgroup]"
