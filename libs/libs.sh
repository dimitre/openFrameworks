#!/bin/bash
cd "$(dirname "$0")"


COLOR='\033[0;32m'
COLOR2='\033[0;34m'
COLOR3='\033[0;95m'

NC='\033[0m' # No Color

section() {
    printf "⚡️ ${COLOR}$@ ${NC}\r\n"
}
sectionOK() {
	printf "💾 ${COLOR}$@ ${NC}\r\n"
}
executa2() {
    printf "✅ ${COLOR2}$@ ${NC}\r\n"
}
executa() { #echoes and execute. dry run is "executa2"
	printf "✅ ${COLOR2}$@ ${NC}\r\n"
    $@
}


VERSION=v0.12.1
OF_FOLDER=..
# PLATFORM=macos
#rpi-aarch64.zip

for i in "$@"; do
  case $i in
    -p=*|--platform=*)
      PLATFORM="${i#*=}"
      shift # past argument=value
      ;;
    -l=*|--searchpath=*)
      SEARCHPATH="${i#*=}"
      shift # past argument=value
      ;;
  esac
done

# LIBS_FOLDER=${OF_FOLDER}/libs/${PLATFORM}
# echo ${PLATFORM}
# exit

if [[ "$(uname -s)" == "Darwin" ]]; then
    PLATFORM=macos
    CORELIBS=( brotli cairo FreeImage freetype glew glfw glm json libpng pugixml tess2 uriparser utfcpp zlib openssl curl pixman )
    # FIXME: TODO: add svgtiny to ofLibs and here
    ADDONLIBS=( assimp libusb libxml2 opencv )
    ALLLIBS="${CORELIBS[@]} ${ADDONLIBS[@]}"

    LIBADDONS=(
    	# "assimp:ofxAssimpModelLoader"
    	"assimp:ofxAssimp"
        "libusb:ofxKinect"
        "libxml2:ofxSvg"
        "opencv:ofxOpenCv"
        # "svgtiny:ofxSvg"
    )

elif [[ "$(uname -s)" == "Linux" ]]; then
	GSTREAMER_VERSION=1.0
	sudo apt-get -y install libcairo2-dev make libgtk2.0-dev nlohmann-json3-dev libssl3 libcurl4 brotli libcurl4-openssl-dev libjack-jackd2-0 libjack-jackd2-dev freeglut3-dev libasound2-dev libxmu-dev libxxf86vm-dev libgl1-mesa-dev libraw1394-dev libudev-dev libdrm-dev libglew-dev libopenal-dev libsndfile1-dev libfreeimage-dev libcairo2-dev libfreetype6-dev libssl-dev libpulse-dev libusb-1.0-0-dev libopencv-dev libassimp-dev librtaudio-dev liburiparser-dev libpugixml-dev libgtk2.0-0 libxcursor-dev libxi-dev libxinerama-dev libglfw3-dev libxml2-dev libgstreamer${GSTREAMER_VERSION}-dev libgstreamer-plugins-base${GSTREAMER_VERSION}-dev gstreamer${GSTREAMER_VERSION}-ffmpeg gstreamer${GSTREAMER_VERSION}-pulseaudio gstreamer${GSTREAMER_VERSION}-x gstreamer${GSTREAMER_VERSION}-plugins-bad gstreamer${GSTREAMER_VERSION}-alsa gstreamer${GSTREAMER_VERSION}-plugins-base gstreamer${GSTREAMER_VERSION}-plugins-good

	CORELIBS=( kissfft glm tess2 utfcpp )
	ADDONLIBS=(  )
	ALLLIBS="${CORELIBS[@]} ${ADDONLIBS[@]}"
	section Linux
	uname -m

	if [[ "$(uname -m)" == "x86_64" ]]; then
		PLATFORM=linux64
	elif [ -f /etc/rpi-issue ]; then
		PLATFORM=rpi-aarch64
    fi
fi

# PLATFORM="${PLATFORM:-macos}"


# unameOut="$(uname -s)"
# case "${unameOut}" in
#     Linux*)     machine=Linux;;
#     Darwin*)    machine=macos;;
#     CYGWIN*)    machine=Cygwin;;
#     MINGW*)     machine=MinGw;;
#     MSYS_NT*)   machine=MSys;;
#     *)          machine="UNKNOWN:${unameOut}"
# esac
# echo ${machine}
# exit


LIBS_FOLDER=./${PLATFORM}

DOWNLOAD="./_download_${VERSION}"
# echo ${DOWNLOAD}

checkLib() {
	for lib in "$@"
	do
		section Check $lib
		if ! command -v $lib 2>&1 >/dev/null
		then
		    echo "<$lib> not found, installing via brew"
			if [[ ${PLATFORM} == "macos" ]]; then
    		    executa brew install $lib
            else
                executa sudo apt install -y $lib
			fi
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




# exit 1

getlink() {
    for LIBNAME in ${ALLLIBS[@]}
    do
        PARAMS+=" "https://github.com/dimitre/ofLibs/releases/download/${VERSION}/oflib_${LIBNAME}_${PLATFORM}.zip
    done
    executa "wget2 --clobber=off ${PARAMS} -P ${DOWNLOAD}"
}

unzipCore() {
	for LIBNAME in ${CORELIBS[@]}
	do
	    filename="${DOWNLOAD}/oflib_${LIBNAME}_${PLATFORM}.zip"
		# executa unzip -o ${filename} -d ${LIBS_FOLDER}
		# -q = quiet -qq = quieter
		executa "unzip -qq -o ${filename} -d ${LIBS_FOLDER}"
	done

	executa "rm -rf ${LIBS_FOLDER}/*.{txt,md,MIT}"
	executa "rm -rf ${LIBS_FOLDER}/{LICENSE,COPYING}"
	executa "rm -rf ${LIBS_FOLDER}/LICENSES"

	# remover no futuro
	executa "mv ${LIBS_FOLDER}/lib/${PLATFORM}/* ${LIBS_FOLDER}/lib/"
	executa "rm -rf ${LIBS_FOLDER}/lib/${PLATFORM}"
}



unzipAddons() {
	for libaddon in "${LIBADDONS[@]}" ; do
		lib=${libaddon%%:*}
		addon=${libaddon#*:}
		OUTFOLDER=${OF_FOLDER}/addons/${addon}/libs/${lib}
		executa "mkdir -p ${OUTFOLDER}"
		executa "unzip -qq -o -d ${OUTFOLDER} ${DOWNLOAD}/oflib_${lib}_${PLATFORM}.zip"
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
