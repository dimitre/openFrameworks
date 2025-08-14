#!/bin/bash
cd "$(dirname "$0")"

COLOR='\033[0;32m'
COLOR2='\033[0;34m'
NC='\033[0m' # No Color
SUDO='sudo'
if [[ "$OSTYPE" == "cygwin"* ]]; then
	SUDO=''
fi

section() {
    printf "💿${COLOR} ${@} ${NC}\n\r"
}

section "OFWorks ofgen (tool to generate projects)"
echo This will install a symlink in /usr/local/bin/ so ofgen can be called from any directory.
echo If you need to uninstall in the future you can use the $ofw/ofgen/uninstall.sh script
    echo It is recommended to install, so you can just invoke ofgen from your project folder
echo and have your project built.
echo You will be asked for user password.
# read -p "Proceed? (y/n) " -n 1 -r

# echo -------
#
if [[ -n $GITHUB_REPOSITORY ]]; then
    echo WOW GITHUB_REPOSITORY
    ${SUDO} ln -sf "$PWD/ofgen" /usr/local/bin/ofgen
	echo All good!
else

read -p "Proceed? (y/n) " -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
# -sf parameters will overwrite old symlink if it is already installed
	if [[ ! -d "/usr/local/bin" ]]; then
  		echo "/usr/local/bin does not exist. creating."
    	sudo mkdir /usr/local/bin
	fi
	sudo ln -sf "$PWD/ofgen" /usr/local/bin/ofgen
	echo All good!
else
	echo OK, ofGen not installed
fi

fi
