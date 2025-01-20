#!/bin/bash
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
