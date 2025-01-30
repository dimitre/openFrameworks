#!/bin/zsh
cd "$(dirname "$0")"

COLOR='\033[0;32m'
COLOR2='\033[0;34m'
NC='\033[0m' # No Color

section() {
    printf "💿${COLOR} ${@} ${NC}\n\r"
}

section "OpenFrameworks ofGen (tool to generate projects)"
echo this will install a symlink in /usr/local/bin/ so ofGen can be called from any directory.
echo if you need to uninstall in the future you can use the $ofw/ofGen/uninstall.sh script
echo -------
sudo ln -s "$PWD/ofGen" /usr/local/bin/ofGen
