#!/bin/bash
cd "$(dirname "$0")"

read -p "Installing OFVision to ${PWD} Are you sure? " -n 1 -r

set -e

echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
	echo "YES"
else
	echo "ok!"
    # do dangerous stuff
fi
