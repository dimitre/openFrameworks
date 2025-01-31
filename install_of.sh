#!/bin/bash
cd "$(dirname "$0")"

COLOR='\033[0;32m'
COLOR2='\033[0;34m'
NC='\033[0m' # No Color

section() {
    printf "💿${COLOR} ${@} ${NC}\n\r"
}

# echo "parameter"
echo $@
section "OF Vision (OpenFrameworks Fork)"
read -p "Installing to ${PWD}/ofvision Proceed? (y/n) " -n 1 -r

set -e

echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
	echo "YES"
	time git clone --branch tuningfork https://github.com/dimitre/openframeworks --depth 1 ofvision
	cd ofvision
	time ./libs.sh
	cd ofGen
	./compile.sh
	./install.sh
	cd ..
else
	echo "ok, exit without installing."
    # do dangerous stuff
fi


## BREW ALIKE
# /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/dimitre/openFrameworks/refs/heads/tuningfork/install_of.sh)" pwd
