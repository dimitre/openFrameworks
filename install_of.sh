#!/bin/bash
cd "$(dirname "$0")"
echo "parameter"
echo $@
read -p "Installing OFVision to ${PWD} Are you sure? " -n 1 -r

set -e

echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
	echo "YES"
	time git clone --branch tuningfork https://github.com/dimitre/openframeworks --depth 1 ofvision
	cd ofvision
	cd ofGen
	time ./compile.sh
	cd ..
	time ./libs.sh
else
	echo "ok!"
    # do dangerous stuff
fi


## BREW ALIKE
# /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/dimitre/openFrameworks/refs/heads/tuningfork/install_of.sh)" pwd
