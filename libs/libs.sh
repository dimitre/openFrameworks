#!/bin/bash
cd "$(dirname "$0")"

wipeDownloads=true
wipeAddonLibs=true
wipeLibs=true

COLOR='\033[0;32m'
COLOR2='\033[0;34m'
COLOR3='\033[0;95m'

NC='\033[0m' # No Color

section() {
	printf "⚡️ ${COLOR}$@${NC}\n"
}
sectionOK() {
	printf "💾 ${COLOR}$@${NC}\n"
}
executa2() {
	printf "✅ ${COLOR2}$@${NC}\n"
}
executa() { #echoes and execute. dry run is "executa2"
	printf "✅ ${COLOR2}$@${NC}\n"
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

# if [[ "$OSTYPE" == "msys"* ]]; then
# This is MSYS / MING Windows

# echo "ostype = ${OSTYPE}"

# VS - Visual Studio
if [[ "$OSTYPE" == "msys"* ]]; then
    PLATFORM=vs
   	CORELIBS=( mango libjpeg libtiff videoInput yaml-cpp brotli FreeImage freetype glew glfw glm json libpng pixman pugixml rtAudio tess2 uriparser utfcpp zlib )
	# FIXME: TODO: add svgtiny to ofLibs and here
	# TODO: curl, openssl
	ADDONLIBS=( assimp cairo libusb opencv )
	ALLLIBS="${CORELIBS[@]} ${ADDONLIBS[@]}"

	LIBADDONS=(
		# "assimp:ofxAssimpModelLoader"
		"assimp:ofxAssimp"
		"cairo:ofxCairo"
		"libusb:ofxKinect"
		"opencv:ofxOpenCv"
	)

# Windows MSYS2
elif [[ "$OSTYPE" == "cygwin"* ]]; then
	PLATFORM=msys2
	CORELIBS=( tess2 kissfft videoInput )
	# CORELIBS=( brotli cairo FreeImage freetype glew glfw glm json libpng pugixml rtAudio tess2 uriparser utfcpp zlib openssl curl pixman )
	ADDONLIBS=( )
	ALLLIBS="${CORELIBS[@]} ${ADDONLIBS[@]}"
	# LIBADDONS=(
	# 	# "assimp:ofxAssimpModelLoader"
	# 	"assimp:ofxAssimp"
	#	 "libusb:ofxKinect"
	#	 "libxml2:ofxSvg"
	#	 "opencv:ofxOpenCv"
	#	 # "svgtiny:ofxSvg"
	# )
	# gcc
	PACMANLIBS="toolchain openssl python assimp cairo curl freeglut FreeImage glew glfw glm libsndfile libusb libxml2 mpg123 nlohmann-json openal opencv pugixml rtaudio uriparser utf8cpp"

	PACMANPARAMS="pacman -Syyuw --noconfirm"
	for LIBNAME in ${PACMANLIBS[@]}
	do
		PACMANPARAMS+=" mingw-w64-x86_64-${LIBNAME}"
	done

	# FIXME VOLTAR
	# executa "${PACMANPARAMS}"

# macOS
elif [[ "$(uname -s)" == "Darwin" ]]; then
	PLATFORM=macos
	CORELIBS=( mango yaml-cpp lzma libtiff libjpeg brotli FreeImage freetype glew glfw glm json libpng pixman pugixml rtAudio tess2 uriparser utfcpp zlib  )
	# FIXME: TODO: add svgtiny to ofLibs and here
	ADDONLIBS=( assimp cairo libusb opencv openssl curl )
	ALLLIBS="${CORELIBS[@]} ${ADDONLIBS[@]}"

	LIBADDONS=(
		# "assimp:ofxAssimpModelLoader"
		"assimp:ofxAssimp"
		"cairo:ofxCairo"
		"libusb:ofxKinect"
		# "libxml2:ofxSvg"
		"opencv:ofxOpenCv"
		"openssl:ofxURL"
		"curl:ofxURL"
		# "svgtiny:ofxSvg"
	)


elif [[ "$(uname -s)" == "Linux" ]]; then # UBUNTU
	# libxml2-dev
	# sudo apt-get -y install libcairo2-dev make libgtk2.0-dev nlohmann-json3-dev libssl3 libcurl4 brotli libcurl4-openssl-dev libjack-jackd2-0 libjack-jackd2-dev freeglut3-dev libasound2-dev libxmu-dev libxxf86vm-dev libgl1-mesa-dev libraw1394-dev libudev-dev libdrm-dev libglew-dev libopenal-dev libsndfile1-dev libfreeimage-dev libcairo2-dev libfreetype6-dev libssl-dev libpulse-dev libusb-1.0-0-dev libopencv-dev libassimp-dev librtaudio-dev liburiparser-dev libpugixml-dev libgtk2.0-0 libxcursor-dev libxi-dev libxinerama-dev libglfw3-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev gstreamer1.0-pulseaudio gstreamer1.0-x gstreamer1.0-plugins-bad gstreamer1.0-alsa gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-libav
	# CORELIBS=( kissfft glm tess2 utfcpp )
	# ADDONLIBS=(	)

	if [[ -z "$CI" ]]; then
	    #libgtk2.0-dev libgtk2.0-0
					# libraw1394-dev # firewire support, legacy
					# libpulse-dev gstreamer1.0-pulseaudio libjack-jackd2-0 libjack-jackd2-dev
					#
					# libdrm-dev
					#
					# libraw1394-dev libdrm-dev libopenal-dev libpulse-dev gstreamer1.0-pulseaudio libjack-jackd2-0 libjack-jackd2-dev
		sudo apt-get -y install make \
		libssl3 libcairo2-dev libssl-dev libcurl4 libcurl4-openssl-dev \
		libasound2-dev libsndfile1-dev \
		libopenal-dev \
		freeglut3-dev libxmu-dev libxxf86vm-dev libgl1-mesa-dev libudev-dev libxcursor-dev libxi-dev libxinerama-dev \
		libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
		gstreamer1.0-x gstreamer1.0-plugins-bad gstreamer1.0-alsa gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-libav
	fi
	# Fixme. Brotli will have to be re-added when issues are fixed # brotli
	CORELIBS=( mango yaml-cpp kissfft libtiff libjpeg FreeImage freetype glew glfw glm json libpng pixman pugixml rtAudio tess2 uriparser utfcpp zlib  )

	ADDONLIBS=(	assimp libusb opencv )
	LIBADDONS=(
		# "assimp:ofxAssimpModelLoader"
		"assimp:ofxAssimp"
		# "cairo:ofxCairo"
		"libusb:ofxKinect"
		# "libxml2:ofxSvg"
		"opencv:ofxOpenCv"
		# "openssl:ofxURL"
		# "curl:ofxURL"
		# "svgtiny:ofxSvg"
	)


	ALLLIBS="${CORELIBS[@]} ${ADDONLIBS[@]}"
	section Linux
	uname -m

	if [[ "$(uname -m)" == "x86_64" ]]; then
		PLATFORM=linux64
	elif [ -f /etc/rpi-issue ]; then
		PLATFORM=rpi-aarch64
	fi
fi


# FIXME: Remove, convert to function
if [[ $1 == 'vs' ]]; then
    PLATFORM=$1
   	CORELIBS=( yaml-cpp brotli FreeImage freetype glew glfw glm json libpng pixman pugixml rtAudio tess2 uriparser utfcpp zlib )
	# FIXME: TODO: add svgtiny to ofLibs and here
	# TODO: curl, openssl
	ADDONLIBS=( assimp cairo libusb opencv )
	ALLLIBS="${CORELIBS[@]} ${ADDONLIBS[@]}"

	LIBADDONS=(
		# "assimp:ofxAssimpModelLoader"
		"assimp:ofxAssimp"
		"cairo:ofxCairo"
		"libusb:ofxKinect"
		"opencv:ofxOpenCv"
	)
fi
# PLATFORM="${PLATFORM:-macos}"


# unameOut="$(uname -s)"
# case "${unameOut}" in
#	 Linux*)	 machine=Linux;;
#	 Darwin*)	machine=macos;;
#	 CYGWIN*)	machine=Cygwin;;
#	 MINGW*)	 machine=MinGw;;
#	 MSYS_NT*)	 machine=MSys;;
#	 *)			machine="UNKNOWN:${unameOut}"
# esac
# echo ${machine}
# exit

LIBS_FOLDER=./${PLATFORM}

if [[ "$wipeLibs" == true && -d ${LIBS_FOLDER} ]]; then
    executa rm -rf ${LIBS_FOLDER}
fi

DOWNLOAD="./_download_${VERSION}_${PLATFORM}"

# wipe folder to re-download libs.
if [[ "$wipeDownloads" == true && -d "${DOWNLOAD}" ]]; then
    echo "Removing Previously Downloaded Libraries"
    rm -rf ${DOWNLOAD}
fi

echo "Creating Download Folder ${DOWNLOAD}"
mkdir ${DOWNLOAD}


# echo ${DOWNLOAD}

checkLib() {
	for lib in "$@"
	do
		section Check $lib
		if ! command -v $lib 2>&1 >/dev/null
		then
			if [[ ${PLATFORM} == "macos" ]]; then
				echo "$lib not found, installing via brew"
				executa brew install $lib
			# FIXME: msys2 doesn't want sudo, check via other stuff.
			else
				echo "$lib not found, installing via apt"
				executa sudo apt-get install -y $lib
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
#	 echo "<wget2> not found, installing via brew"
#	 executa brew install wget2
# else
# 	echo "ok"
# fi
# }

# exit 1

getlink() {
    if [[ "$OSTYPE" == "cygwin"* || "$OSTYPE" == "msys"* ]]; then

		for LIBNAME in ${ALLLIBS[@]}
		do
			# github uses redirect, so it is needed -L parameter in curl.
			executa "curl -L -o ${DOWNLOAD}/oflib_${LIBNAME}_${PLATFORM}.zip https://github.com/dimitre/ofLibs/releases/download/${VERSION}/oflib_${LIBNAME}_${PLATFORM}.zip"
		done
	else

		for LIBNAME in ${ALLLIBS[@]}
		do
			PARAMS+=" "https://github.com/dimitre/ofLibs/releases/download/${VERSION}/oflib_${LIBNAME}_${PLATFORM}.zip
		done

		executa "wget2 --clobber=off ${PARAMS} -P ${DOWNLOAD}"
	fi
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
	# executa "mv ${LIBS_FOLDER}/lib/${PLATFORM}/* ${LIBS_FOLDER}/lib/"
	# executa "rm -rf ${LIBS_FOLDER}/lib/${PLATFORM}"
}



unzipAddons() {
	for libaddon in "${LIBADDONS[@]}" ; do
		lib=${libaddon%%:*}
		addon=${libaddon#*:}
		OUTFOLDER=${OF_FOLDER}/addons/${addon}/libs/${lib}
		wipeAddonLibs=true
        if [ "$wipeAddonLibs" == true ]; then
            executa "rm -rf ${OUTFOLDER}"
        fi
		executa "mkdir -p ${OUTFOLDER}"
		executa "unzip -qq -o -d ${OUTFOLDER} ${DOWNLOAD}/oflib_${lib}_${PLATFORM}.zip"
	done
}


sectionOK OpenFrameworks install ofLibs
# ccache
checkLib wget2
#	fmt yaml-cpp
# checkWget2
executa mkdir -p ${DOWNLOAD}
getlink
unzipCore
unzipAddons
sectionOK Install ofLibs done

trap 'printf "${NC}"' EXIT
