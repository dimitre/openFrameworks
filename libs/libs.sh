#!/bin/bash
cd "$(dirname "$0")"
set -eu

VERSION=v0.12.6
OF_FOLDER=..
CHALETVERSION=0.8.15

# wipeDownloads=true
wipeDownloads=false
wipeAddonLibs=true
wipeLibs=true


COLOR='\033[0;32m'
COLOR2='\033[0;34m'
COLOR3='\033[0;95m'
NC='\033[0m' # No Color

section() {
	# printf "⚡️ ${COLOR}%s${NC}\n" "$*"
	printf "${COLOR}%s${NC}\n" "$*"
}

sectionOK() {
	printf "💾 ${COLOR}%s${NC}\n" "$*"
}

executa2() {
	printf "✅ ${COLOR2}%s${NC}\n" "$*"
}

executa() {
	printf "✅ ${COLOR2}%s${NC}\n" "$*"
	"$@"
}

alert() {
	printf "⚠️ ${COLOR2}%s${NC}\n" "$*"
}

sectionOK "ofWorks install ofLibs"

# Determine if we need sudo
if command -v sudo &> /dev/null; then
  SUDO_CMD="sudo"
else
  SUDO_CMD=""
fi

# Parse command line arguments
PLATFORM="${PLATFORM:-}"
while [[ $# -gt 0 ]]; do
	case $1 in
	-p=*|--platform=*)
		PLATFORM="${1#*=}"
		shift
		;;
	-p|--platform)
		PLATFORM="$2"
		shift 2
		;;
	*)
		echo "Unknown option: $1"
		exit 1
		;;
	esac
done

# ============================================
# Platform Detection
# ============================================
if [[ -n "$PLATFORM" ]]; then
	section "Using manually specified platform: $PLATFORM"
else
	# Auto-detect platform
	if [[ "$OSTYPE" == "msys"* ]]; then
		PLATFORM=vs
	elif [[ "$OSTYPE" == "cygwin"* ]]; then
		PLATFORM=msys2
	elif [[ "$OSTYPE" == "darwin"* ]]; then
		PLATFORM=macos
	elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
		# Detect Linux variant
		ARCH="$(uname -m)"

		if [[ "$ARCH" == "x86_64" ]]; then
			PLATFORM=linux64
		# commenting out this for now so I can use ubuntu-22-arm
		# elif [[ "$ARCH" == "aarch64" ]] && [[ -f /etc/rpi-issue ]]; then
		elif [[ "$ARCH" == "aarch64" ]]; then
			PLATFORM=rpi-aarch64
		elif [[ "$ARCH" == "armv6l" ]] || [[ "$ARCH" == "armv7l" ]]; then
			# Check if it's actually a Raspberry Pi
			if [[ -f /proc/device-tree/model ]] && grep -q "Raspberry Pi" /proc/device-tree/model; then
				PLATFORM=rpi-armv6l
			else
				echo "Unsupported ARM architecture: $ARCH (not a Raspberry Pi?)"
				exit 1
			fi
		else
			echo "Unsupported Linux architecture: $ARCH"
			exit 1
		fi
	else
		echo "Unsupported OS: $OSTYPE"
		exit 1
	fi
	section "Platform: $PLATFORM"

fi


# ============================================
# Setup Directories
# ============================================
LIBS_FOLDER="./${PLATFORM}"

if [[ "$wipeLibs" == true && -d "${LIBS_FOLDER}" ]]; then
	executa rm -rf "${LIBS_FOLDER}"
fi

DOWNLOAD="./_download_${VERSION}_${PLATFORM}"

if [[ "$wipeDownloads" == true && -d "${DOWNLOAD}" ]]; then
	echo "Removing Previously Downloaded Libraries"
	rm -rf "${DOWNLOAD}"
fi

echo "Creating Download Folder ${DOWNLOAD}"
mkdir -p "${DOWNLOAD}"

# ============================================
# Platform-Specific Configuration
# ============================================

ADDONLIBS=( assimp opencv libusb pixman cairo )
LIBADDONS=(
	"assimp:ofxAssimp"
	"libusb:ofxKinect"
	"opencv:ofxOpenCv"
	"cairo:ofxCairo"
	"pixman:ofxCairo"
)

	# msys2)
	#	 CORELIBS=( tess2 kissfft videoInput )

	#	 # Install MSYS2 packages
	#	 PACMANLIBS="toolchain openssl python assimp cairo curl freeglut FreeImage glew glfw glm libsndfile libusb libxml2 mpg123 nlohmann-json openal opencv pugixml rtaudio uriparser utf8cpp"
	#	 PACMANPARAMS="pacman -Syyuw --noconfirm"
	#	 for LIBNAME in ${PACMANLIBS}; do
	#		 PACMANPARAMS+=" mingw-w64-x86_64-${LIBNAME}"
	#	 done
	#	 # executa ${PACMANPARAMS}
	#	 ;;

# REMOVED brotli freetype libpng
CORELIBS=( mango yaml-cpp freetype glew glfw glm json pugixml rtAudio tess2 uriparser utfcpp zlib-ng )


case "$PLATFORM" in
	vs)
		CORELIBS+=( videoInput )


		if ! command -v wget2 &> /dev/null; then
			section "wget2 not installed, installing scoop and wget2..."
			powershell.exe -ExecutionPolicy Bypass -Command "
				if (!(Get-Command scoop -ErrorAction SilentlyContinue)) {
					irm get.scoop.sh | iex
				}
				\$env:Path = [System.Environment]::GetEnvironmentVariable('Path','Machine') + ';' + [System.Environment]::GetEnvironmentVariable('Path','User')
				scoop install wget2
			"

			# Add scoop shims to PATH for this bash session
			section "Adding scoop to PATH..."
			SCOOP_SHIMS="$HOME/scoop/shims"
			if [[ ":$PATH:" != *":$SCOOP_SHIMS:"* ]]; then
				export PATH="$SCOOP_SHIMS:$PATH"
			fi

			# Verify wget2 is now available
			if command -v wget2 &> /dev/null; then
				section "wget2 successfully installed and available!"
			else
				alert "wget2 installed but may require a new shell session"
			fi
		else
			section "wget2 already installed!"
		fi

		;;

	macos)
		# Install system dependencies (skip in CI)
		if [[ -z "${CI:-}" ]]; then

			if command -v brew &> /dev/null; then

				if ! command -v gh &> /dev/null; then
					# section "no wget2"
					brew install gh
				else
					section "gh already installed"
				fi

				if ! command -v wget2 &> /dev/null; then
					# section "no wget2"
					brew install wget2
				else
					section "wget2 already installed"
				fi

				if ! command -v chalet &> /dev/null; then
					brew tap chalet-org/chalet
					brew install --cask chalet
				else
					section "chalet already installed"
				fi

				if ! command -v cmake &> /dev/null; then
					brew install cmake
				else
					section "cmake already installed"
				fi

				if ! command -v ninja &> /dev/null; then
					brew install ninja
				else
					section "ninja already installed"
				fi

			else
				alert "Brew not installed, won't install gh (github command line)"
			fi
		fi

		;;

	linux64|rpi-aarch64|rpi-armv6l)

		CORELIBS+=( kissfft )

		# Install system dependencies (skip in CI)
		if [[ -z "${CI:-}" ]]; then
			section "Installing system dependencies"

			${SUDO_CMD} apt-get update

			${SUDO_CMD} apt-get -y install \
				ninja-build wget2 cmake \
				libfontconfig1-dev \
				libglu1-mesa-dev libgl1-mesa-dev \
				libxrandr-dev \
				freeglut3-dev libxmu-dev libxxf86vm-dev libudev-dev \
				libxcursor-dev libxi-dev libxinerama-dev \
				libunwind-dev \
				libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
				gstreamer1.0-x gstreamer1.0-plugins-bad gstreamer1.0-alsa \
				gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-libav \
				libopenal-dev libsndfile1-dev
				# make \
				# libssl3 libcairo2-dev libssl-dev libcurl4 libcurl4-openssl-dev \
				# libasound2-dev   \
				#
			if ! command -v chalet &> /dev/null; then
				if [[ ${PLATFORM} == 'linux64' ]]; then
					curl -L -O https://github.com/chalet-org/chalet/releases/download/v${CHALETVERSION}/chalet_${CHALETVERSION}_amd64.deb &&
					sudo dpkg -i chalet*.deb
				else
					# there is arm only also.
					curl -L -O https://github.com/chalet-org/chalet/releases/download/v${CHALETVERSION}/chalet_${CHALETVERSION}_arm64.deb &&
					sudo dpkg -i chalet*.deb
				fi
			else
				section "chalet already installed"
			fi
		fi # end if CI

		;;

	*)
		echo "Unknown platform: $PLATFORM"
		exit 1
		;;
esac

# Build combined library list
ALLLIBS=("${CORELIBS[@]}" "${ADDONLIBS[@]}")

# section "Core libs: ${#CORELIBS[@]}"
# section "Addon libs: ${#ADDONLIBS[@]}"



# ============================================
# Helper Functions
# ============================================
# checkLib() {
#	 for lib in "$@"; do
#		 section "Check $lib"
#		 if ! command -v "$lib" &>/dev/null; then
#			 if [[ "${PLATFORM}" == "macos" ]]; then
#				 echo "$lib not found, installing via brew"
#				 executa brew install "$lib"
#			 elif [[ "${PLATFORM}" == "vs" ]]; then
#				 echo "$lib will not be installed, vs platform"
#			 else
#				 echo "$lib not found, installing via apt"
#				 executa ${SUDO_CMD} apt-get install -y "$lib"
#			 fi
#		 else
#			 echo "$lib ok"
#		 fi
#	 done
# }

getlink() {
	if command -v gh &>/dev/null; then
		section "Downloading with GH (github command line)"
		gh release download ${VERSION} -R dimitre/ofLibs --pattern "ofLibs_*_${PLATFORM}.zip" -D "${DOWNLOAD}"


	elif command -v wget2 &>/dev/null; then
		# wget2 is available - use it for parallel downloads
		PARAMS=""
		for LIBNAME in "${ALLLIBS[@]}"; do
			PARAMS+=" https://github.com/dimitre/ofLibs/releases/download/${VERSION}/ofLibs_${LIBNAME}_${PLATFORM}.zip"
		done
		section "Downloading with wget2 (parallel downloads)"
		#--clobber=off (skips download if file exists at all)
		# --progress=bar:force
		# echo "${PARAMS}"
		# -N --no-verbose
		# wget2 "${PARAMS}" -P "${DOWNLOAD}"

		# Run wget2 and capture exit code
		set +e  # Temporarily disable exit on error
		wget2 ${PARAMS} -P "${DOWNLOAD}"
		WGET_EXIT=$?
		set -e  # Re-enable exit on error

		if [[ $WGET_EXIT -ne 0 ]]; then
			alert "wget2 returned exit code: $WGET_EXIT (this may be normal if files are up-to-date)"
		fi

	elif command -v wget &>/dev/null; then
		# Fallback to wget - sequential downloads with timestamp checking
		section "Downloading with wget (sequential)"
		for LIBNAME in "${ALLLIBS[@]}"; do
			local filepath="${DOWNLOAD}/ofLibs_${LIBNAME}_${PLATFORM}.zip"
			executa wget -N --no-verbose --show-progress \
				"https://github.com/dimitre/ofLibs/releases/download/${VERSION}/ofLibs_${LIBNAME}_${PLATFORM}.zip" \
				-P "${DOWNLOAD}"
		done
	elif command -v powershell.exe &>/dev/null; then
		# Fallback to PowerShell on Windows
		section "Downloading with PowerShell (sequential)"
		for LIBNAME in "${ALLLIBS[@]}"; do
			local filepath="${DOWNLOAD}/ofLibs_${LIBNAME}_${PLATFORM}.zip"
			local url="https://github.com/dimitre/ofLibs/releases/download/${VERSION}/ofLibs_${LIBNAME}_${PLATFORM}.zip"
			executa powershell.exe -Command "Invoke-WebRequest -Uri '${url}' -OutFile '${filepath}'"
		done
	else
		# Fallback to curl - sequential downloads
		section "Downloading with curl (sequential)"
		for LIBNAME in "${ALLLIBS[@]}"; do
			local filepath="${DOWNLOAD}/ofLibs_${LIBNAME}_${PLATFORM}.zip"
			# Only use -z if file exists, otherwise just download
			if [[ -f "${filepath}" ]]; then
				executa curl -L -z "${filepath}" -o "${filepath}" \
				"https://github.com/dimitre/ofLibs/releases/download/${VERSION}/ofLibs_${LIBNAME}_${PLATFORM}.zip"
			else
				executa curl -L -o "${filepath}" \
				"https://github.com/dimitre/ofLibs/releases/download/${VERSION}/ofLibs_${LIBNAME}_${PLATFORM}.zip"
			fi
		done
	fi
}

unzipCore() {
	echo "unzipCore"
	for LIBNAME in "${CORELIBS[@]}"; do
		filename="${DOWNLOAD}/ofLibs_${LIBNAME}_${PLATFORM}.zip"
		executa unzip -qq -o "${filename}" -d "${LIBS_FOLDER}"
	done

	rm -rf "${LIBS_FOLDER}"/*.{txt,md,MIT}
	rm -rf "${LIBS_FOLDER}"/{LICENSE,COPYING,LICENSES}
}

unzipAddons() {
	echo "unzipAddons"

	for libaddon in "${LIBADDONS[@]}"; do
		lib=${libaddon%%:*}
		addon=${libaddon#*:}
		OUTFOLDER="${OF_FOLDER}/addons/${addon}/libs/${lib}"

		if [[ "$wipeAddonLibs" == true ]]; then
			executa rm -rf "${OUTFOLDER}"
		fi
		executa mkdir -p "${OUTFOLDER}"
		executa unzip -qq -o -d "${OUTFOLDER}" "${DOWNLOAD}/ofLibs_${lib}_${PLATFORM}.zip"
	done
}

# ============================================
# Main Execution
# ============================================

executa mkdir -p "${DOWNLOAD}"
getlink
unzipCore
unzipAddons

sectionOK "Install ofLibs done"

trap 'printf "${NC}"' EXIT
