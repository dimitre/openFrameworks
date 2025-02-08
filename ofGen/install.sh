#!/bin/sh
cd "$(dirname "$0")"

COLOR='\033[0;32m'
COLOR2='\033[0;34m'
NC='\033[0m' # No Color

section() {
    printf "💿${COLOR} ${@} ${NC}\n\r"
}

section "OpenFrameworks ofGen (tool to generate projects)"
echo This will install a symlink in /usr/local/bin/ so ofGen can be called from any directory.
echo If you need to uninstall in the future you can use the $ofw/ofGen/uninstall.sh script
echo It is recommended to install ofGen, skip if you already have it installed.
echo You will be asked for user password
# echo -------
read -p "Proceed? (y/n) " -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
# -sf parameters will overwrite old symlink if it is already installed
	sudo ln -sf "$PWD/ofGen" /usr/local/bin/ofGen
	echo All good!
else
	echo OK, ofGen not installed
fi
