#!/bin/bash
cd "$(dirname "$0")"

VERSION=v0.12.1
OF_FOLDER=..
PLATFORM=macos
# LIBS_FOLDER=${OF_FOLDER}/libs/${PLATFORM}
LIBS_FOLDER=./${PLATFORM}

DOWNLOAD="./_download_${VERSION}"
# echo ${DOWNLOAD}

COLOR='\033[0;32m'
COLOR2='\033[0;34m'
NC='\033[0m' # No Color

section() {
    printf "⚡️${COLOR} ${@} ${NC}\n\r"
}
sectionOK() {
    printf "💾${COLOR} ${@} ${NC}\n\r"
}
executa2() {
    printf "✅${COLOR2} ${@} ${NC}\n\r"
}
executa() { #echoes and execute. dry run is "executa2"
    printf "✅${COLOR2} ${@} ${NC}\n\r"
    $@
}

checkLib() {
	for lib in "$@"
	do
		section Check $lib
		if ! command -v $lib 2>&1 >/dev/null
		then
		    echo "<$lib> not found, installing via brew"
		    executa brew install $lib
	    else
			echo "$lib ok"
		fi
	done
}


# checkWget2() {
# section Check Wget2
# if ! command -v wget2 2>&1 >/dev/null
# then
#     echo "<wget2> not found, installing via brew"
#     executa brew install wget2
# else
# 	echo "ok"
# fi
# }

CORELIBS=( brotli cairo FreeImage freetype glew glfw glm json libpng pugixml tess2 uriparser utfcpp zlib openssl curl pixman )
# FIXME: TODO: add svgtiny to ofLibs and here
ADDONLIBS=( assimp libusb libxml2 opencv )
#svgtiny
ALLLIBS="${CORELIBS[@]} ${ADDONLIBS[@]}"


# exit 1

getlink() {
    for LIBNAME in ${ALLLIBS[@]}
    do
        PARAMS+=" "https://github.com/dimitre/ofLibs/releases/download/${VERSION}/oflib_${LIBNAME}_${PLATFORM}.zip
    done
    executa wget2 --clobber=off ${PARAMS} -P ${DOWNLOAD}
}

unzipCore() {
	for LIBNAME in ${CORELIBS[@]}
	do
	    filename="${DOWNLOAD}/oflib_${LIBNAME}_${PLATFORM}.zip"
		# executa unzip -o ${filename} -d ${LIBS_FOLDER}
		# -q = quiet -qq = quieter
		executa unzip -qq -o ${filename} -d ${LIBS_FOLDER}
	done

	executa rm -rf ${LIBS_FOLDER}/*.{txt,md,MIT}
	executa rm -rf ${LIBS_FOLDER}/{LICENSE,COPYING}
	executa rm -rf ${LIBS_FOLDER}/LICENSES
	executa mv ${LIBS_FOLDER}/lib/${PLATFORM}/* ${LIBS_FOLDER}/lib/
	executa rm -rf ${LIBS_FOLDER}/lib/${PLATFORM}
}

LIBADDONS=(
	"assimp:ofxAssimpModelLoader"
	"assimp:ofxAssimp"
    "libusb:ofxKinect"
    "libxml2:ofxSvg"
    "opencv:ofxOpenCv"
    # "svgtiny:ofxSvg"
)

unzipAddons() {
	for libaddon in "${LIBADDONS[@]}" ; do
		lib=${libaddon%%:*}
		addon=${libaddon#*:}
		OUTFOLDER=${OF_FOLDER}/addons/${addon}/libs/${lib}
		executa mkdir -p ${OUTFOLDER}
		executa unzip -qq -o -d ${OUTFOLDER} ${DOWNLOAD}/oflib_${lib}_${PLATFORM}.zip
	done
}


sectionOK OpenFrameworks install ofLibs
# ccache
checkLib wget2
#  fmt yaml-cpp
# checkWget2
executa mkdir -p ${DOWNLOAD}
getlink
unzipCore
unzipAddons
sectionOK Install ofLibs done
