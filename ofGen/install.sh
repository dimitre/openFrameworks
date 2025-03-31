#!/bin/bash
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
echo It is recommended to install, so you can just invoke ofGen from your project folder
echo and have your project built.
echo You will be asked for user password.
# read -p "Proceed? (y/n) " -n 1 -r

# echo -------
#
if [[ -n $GITHUB_REPOSITORY ]]; then
    echo WOW GITHUB_REPOSITORY
    sudo ln -sf "$PWD/ofGen" /usr/local/bin/ofGen
	echo All good!
else

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

fi
