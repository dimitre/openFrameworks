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

sectionOK() {
    printf "💾 ${COLOR} ${@} ${NC}\n\r"
}

# this one echoes and invokes a command. it is useful if we want to dry run script.
executa() {
    printf "✅ ${COLOR2} ${@} ${NC}\n\r"
    $@
}

executa2() {
    printf "✅ ${COLOR2} ${@} ${NC}\n\r"
    # $@
}


checkWget2() {
section Check Wget2
if ! command -v wget2 2>&1 >/dev/null
then
    echo "<wget2> not found, installing via brew"
    executa brew install wget2
else
	echo "ok"
fi
}



CORELIBS=( brotli cairo FreeImage freetype glew glfw glm json libpng pugixml tess2 uriparser utfcpp zlib openssl curl )

# FIXME: TODO: add svgtiny to ofLibs and here
ADDONLIBS=( assimp libusb libxml2 opencv svgtiny )
ALLLIBS="${CORELIBS[@]} ${ADDONLIBS[@]}"



# echo ${CORELIBS[@]}
# echo ${ADDONLIBS[@]}
# echo ${ALLLIBS[@]}
# LIBS=( assimp brotli cairo FreeImage freetype glew glfw glm json libpng libusb libxml2 opencv pugixml svgtiny tess2 uriparser utfcpp zlib openssl curl )

getlink() {
    for LIBNAME in ${ALLLIBS[@]}
    do
        DOWNLOAD=oflib_${LIBNAME}_${PLATFORM}.zip
        PARAMS+=" "https://github.com/dimitre/ofLibs/releases/download/v0.12.1/${DOWNLOAD}
    done
    executa wget2 --clobber=off ${PARAMS} -P _download
}

unzipCore() {
	for LIBNAME in ${CORELIBS[@]}
	do
	    filename=_download/oflib_${LIBNAME}_${PLATFORM}.zip
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
	    # printf "%s goes to %s.\n" "$lib" "$addon"
		OUTFOLDER=${OF_FOLDER}/addons/${addon}/libs/${lib}
		executa unzip -qq -o _download/oflib_${lib}_${PLATFORM}.zip -d ${OUTFOLDER}
	done
}


sectionOK OpenFrameworks install ofLibs

checkWget2
executa mkdir -p _download
getlink
unzipCore
unzipAddons

# unzipall() {
#     for filename in _download/*.zip; do
#         echo ${filename}
#         executa unzip -o ${filename} -d _download/macos
#     done
# }

# getlink
# unzipall
# executa mv _download/macos/lib/macos/*.a _download/macos/lib/


# getlink assimp brotli cairo curl FreeImage freetype glew glfw glm json libpng libusb libxml2 opencv pugixml svgtiny tess2 uriparser utfcpp zlib


instala() {
    LIBNAME=$@
    section Installing ${LIBNAME}
    DOWNLOAD=oflib_${LIBNAME}_${PLATFORM}.zip
    # executa ${W} https://github.com/dimitre/ofLibs/releases/download/v0.12.1/${DOWNLOAD} -O ${DOWNLOAD}
    executa ${W} https://github.com/dimitre/ofLibs/releases/download/v0.12.1/${DOWNLOAD} &&
    OUTFOLDER=${LIBS_FOLDER}/${LIBNAME}
    # OUTFOLDER=${LIBS_FOLDER}/macos

    for libaddon in "${LIBADDONS[@]}" ; do
        LIB=${libaddon%%:*}
        ADDON=${libaddon#*:}
        if [[ ${LIBNAME} == ${LIB} ]]
        then
            printf "%s likes to %s.\n" "$LIB" "$ADDON"
            OUTFOLDER=${OF_FOLDER}/addons/${ADDON}/libs/${LIB}
        fi
    done

    rm -rf ${OUTFOLDER}
    executa unzip -o ${DOWNLOAD} -d ${OUTFOLDER}
}



sectionOK Install ofLibs done
