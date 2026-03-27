#!/bin/bash
cd "$(dirname "$0")"
set -eu

LIBSVERSION=v1.0
OF_FOLDER=..
# CHALETVERSION=0.8.17
OF_LIBS_REPO="ofWorks/ofLibs"
OF_LIBS_COMMIT_FILE=".oflibs"
WGET2VERSION=v2.2.1
# check new versions here : https://github.com/rockdaboot/wget2/releases/

# Function to get the latest Chalet version from GitHub
get_latest_chalet_version() {
	local latest_version=""
	if command -v curl &>/dev/null; then
		latest_version=$(curl -s "https://api.github.com/repos/chalet-org/chalet/releases/latest" 2>/dev/null | grep -o '"tag_name": "[^"]*"' | head -1 | cut -d'"' -f4 | sed 's/^v//')
	fi
	echo "${latest_version:-0.8.18}"
}

CHALETVERSION=$(get_latest_chalet_version)

wipeDownloads=true
wipeDownloadsAfterInstall=true
# wipeDownloads=false
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

sectionOk() {
	# 💾
	printf "[${COLOR}✓${NC}] ${COLOR}%s${NC}\n" "$*"
}

executa2() {
	# ✅
	printf "${COLOR2}%s${NC}\n" "$*"
}

executa() {
	printf "${COLOR2}%s${NC}\n" "$*"
	"$@"
}

alert() {
	printf "⚠️ ${COLOR2}%s${NC}\n" "$*"
}

section "💾 ofWorks will install ofLibs"

# ============================================
# Check Latest Commit from ofWorks/ofLibs
# ============================================

# Function to check if gh CLI is authenticated
gh_is_authenticated() {
	if command -v gh &>/dev/null; then
		gh auth status &>/dev/null 2>&1
		return $?
	fi
	return 1
}

# Function to get the latest commit hash from the remote repository
get_remote_commit() {
	local commit_hash=""

	# Try using gh CLI first if authenticated (most reliable)
	if gh_is_authenticated; then
		commit_hash=$(gh api repos/${OF_LIBS_REPO}/commits/main --jq '.sha' 2>/dev/null || true)
	fi

	# Fallback to curl if gh is not available, not authenticated, or failed
	if [[ -z "$commit_hash" ]]; then
		commit_hash=$(curl -s "https://api.github.com/repos/${OF_LIBS_REPO}/commits/main" 2>/dev/null | grep -o '"sha": "[^"]*"' | head -1 | cut -d'"' -f4)
	fi

	# Truncate to 7 characters (standard short hash)
	if [[ -n "$commit_hash" ]]; then
		echo "${commit_hash:0:7}"
	fi
}

# Read the stored commit hash if file exists
STORED_COMMIT=""
if [[ -f "$OF_LIBS_COMMIT_FILE" ]]; then
	STORED_COMMIT=$(cat "$OF_LIBS_COMMIT_FILE" 2>/dev/null || true)
	sectionOk "Stored commit: ${STORED_COMMIT:-none}"
fi

# Get the currently installed Chalet version
INSTALLED_CHALET_VERSION=""
if command -v chalet &>/dev/null; then
	INSTALLED_CHALET_VERSION=$(chalet --version 2>/dev/null | awk '{print $3}')
fi

# Get the latest commit from remote
section "Checking latest commit from ${OF_LIBS_REPO}..."
REMOTE_COMMIT=$(get_remote_commit)

if [[ -z "$REMOTE_COMMIT" ]]; then
	alert "Could not fetch latest commit from remote. Proceeding with installation..."
else
	sectionOk "Remote commit: $REMOTE_COMMIT"

	# Compare commits and Chalet version, skip only if both are the same
	LIBS_UP_TO_DATE=false
	CHALET_UP_TO_DATE=false

	if [[ -n "$STORED_COMMIT" && "$STORED_COMMIT" == "$REMOTE_COMMIT" ]]; then
		LIBS_UP_TO_DATE=true
	fi

	if [[ -n "$INSTALLED_CHALET_VERSION" && "$INSTALLED_CHALET_VERSION" == "$CHALETVERSION" ]]; then
		CHALET_UP_TO_DATE=true
	fi

	if [[ "$LIBS_UP_TO_DATE" == true && "$CHALET_UP_TO_DATE" == true ]]; then
		sectionOk "Libraries are up-to-date (commit: $REMOTE_COMMIT)"
		sectionOk "Chalet is up-to-date (version: $CHALETVERSION)"
		sectionOk "Skipping installation. Delete $OF_LIBS_COMMIT_FILE to force reinstall."
		exit 0
	fi

	if [[ "$LIBS_UP_TO_DATE" == false && -n "$STORED_COMMIT" ]]; then
		section "New commit detected: $STORED_COMMIT -> $REMOTE_COMMIT"
	fi

	if [[ "$CHALET_UP_TO_DATE" == false && -n "$INSTALLED_CHALET_VERSION" ]]; then
		section "New Chalet version detected: $INSTALLED_CHALET_VERSION -> $CHALETVERSION"
	fi
fi

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

DOWNLOAD="./_ofLibs_${LIBSVERSION}_${PLATFORM}"

if [[ "$wipeDownloads" == true && -d "${DOWNLOAD}" ]]; then
	echo "Removing Previously Downloaded Libraries"
	rm -rf "${DOWNLOAD}"
fi

echo "Creating Download Folder ${DOWNLOAD}"
mkdir -p "${DOWNLOAD}"

# ============================================
# Platform-Specific Configuration
# ============================================

# now removing pixman from the list. it is bundled together with Cairo .zip
#ADDONLIBS=( assimp opencv libusb pixman cairo )
ADDONLIBS=( assimp opencv libusb cairo )
LIBADDONS=(
	"assimp:ofxAssimp"
	"libusb:ofxKinect"
	"opencv:ofxOpenCv"
	"cairo:ofxCairo"
	# "pixman:ofxCairo"
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
CORELIBS=( mango yaml-cpp fmt freetype glew glfw glm json pugixml rtAudio tess2 uriparser utfcpp zlib-ng )


case "$PLATFORM" in
    emscripten)
        # remove glew from list
        # CORELIBS=( "${CORELIBS[@]/glew}" )
        CORELIBS=( mango yaml-cpp fmt freetype glfw glm json pugixml rtAudio tess2 uriparser utfcpp zlib-ng )
        ADDONLIBS=( assimp opencv libusb )
        LIBADDONS=(
	"assimp:ofxAssimp"
	"libusb:ofxKinect"
	"opencv:ofxOpenCv"
        )
    ;;

		vs)
		    CORELIBS+=( videoInput )

			# avoid install tools in github environment. GH will take care of downloading libs.
			if [[ -z "${CI:-}" ]]; then

				# ----- wget2 : install first (needed for chalet download) -----
				# Use local .tools dir to avoid accent issues in Windows usernames
				WGET2_DIR="./.tools/wget2"
				if ! command -v wget2 &>/dev/null; then
					if [[ ! -x "$WGET2_DIR/wget2.exe" ]]; then
						section "Fetching portable wget2.exe …"
						mkdir -p "$WGET2_DIR"
						curl -L -o "$WGET2_DIR/wget2.exe" \
							--ssl-revoke-best-effort \
							https://github.com/rockdaboot/wget2/releases/download/$WGET2VERSION/wget2.exe
						chmod +x "$WGET2_DIR/wget2.exe"
					fi
					# inject into PATH for this session
					WGET2_DIR_ABS="$(cd "$WGET2_DIR" && pwd)"
					[[ ":$PATH:" != *":$WGET2_DIR_ABS:"* ]] && export PATH="$WGET2_DIR_ABS:$PATH"
					sectionOk "wget2 installed"
				else
					sectionOk "wget2 detected"
				fi
				# ------------------------------------------

				# ----- chalet : portable install with version check -----
				# Use local .tools dir to avoid accent issues in Windows usernames
				CHALET_DIR="./.tools/chalet"
				NEED_INSTALL=false

				if [[ -x "$CHALET_DIR/chalet.exe" ]]; then
					version=$("$CHALET_DIR/chalet.exe" --version 2>/dev/null | awk '{print $3}')
					if [ "$CHALETVERSION" != "$version" ]; then
						section "chalet version mismatch ($version != $CHALETVERSION), updating..."
						rm -rf "$CHALET_DIR"
						NEED_INSTALL=true
					else
						sectionOk "chalet already installed ($version)"
					fi
				else
					NEED_INSTALL=true
				fi

				if [ "$NEED_INSTALL" = true ]; then
					section "Fetching chalet ${CHALETVERSION} for Windows..."
					mkdir -p "$CHALET_DIR"

					if ! command -v wget2 &>/dev/null; then
						echo "Error: wget2 not found. Cannot download chalet."
						exit 1
					fi

					if ! wget2 -O "$CHALET_DIR/chalet.zip" \
						"https://github.com/chalet-org/chalet/releases/download/v${CHALETVERSION}/chalet-x86_64-pc-windows-msvc.zip"; then
						echo "Error: Failed to download chalet."
						rm -rf "$CHALET_DIR"
						exit 1
					fi

					# Verify it's a valid zip before extracting
					if ! unzip -t "$CHALET_DIR/chalet.zip" > /dev/null 2>&1; then
						echo "Error: Downloaded file is not a valid zip. Cleaning up..."
						rm -rf "$CHALET_DIR"
						exit 1
					fi

					unzip -o "$CHALET_DIR/chalet.zip" -d "$CHALET_DIR"
					rm -f "$CHALET_DIR/chalet.zip"

					# Verify chalet.exe exists after extraction
					if [[ ! -x "$CHALET_DIR/chalet.exe" ]]; then
						echo "Error: chalet.exe not found after extraction."
						rm -rf "$CHALET_DIR"
						exit 1
					fi
				fi

				# inject into PATH for this session (convert to absolute path)
				CHALET_DIR_ABS="$(cd "$CHALET_DIR" && pwd)"
				[[ ":$PATH:" != *":$CHALET_DIR_ABS:"* ]] && export PATH="$CHALET_DIR_ABS:$PATH"

				# Add to Windows SYSTEM PATH permanently using PowerShell (requires admin)
				# Convert to Windows path format (handle both cygpath and manual conversion)
				if command -v cygpath &>/dev/null; then
					CHALET_DIR_WIN=$(cygpath -w "$CHALET_DIR_ABS")
				else
					# Manual conversion for Git Bash: /c/path -> C:/path
					CHALET_DIR_WIN="$CHALET_DIR_ABS"
					if [[ "$CHALET_DIR_WIN" =~ ^/([a-zA-Z])/(.*) ]]; then
						CHALET_DIR_WIN="${BASH_REMATCH[1]^^}:/${BASH_REMATCH[2]}"
					fi
				fi
				section "Adding chalet to Windows PATH: $CHALET_DIR_WIN"
				powershell.exe -Command "
					\$currentPath = [Environment]::GetEnvironmentVariable('Path', 'Machine')
					if (\$currentPath -notlike '*$CHALET_DIR_WIN*') {
						[Environment]::SetEnvironmentVariable('Path', \$currentPath + ';$CHALET_DIR_WIN', 'Machine')
						Write-Host 'Chalet added to system PATH'
					} else {
						Write-Host 'Chalet already in system PATH'
					}
				" 2>/dev/null || powershell.exe -Command "
					\$currentPath = [Environment]::GetEnvironmentVariable('Path', 'User')
					if (\$currentPath -notlike '*$CHALET_DIR_WIN*') {
						[Environment]::SetEnvironmentVariable('Path', \$currentPath + ';$CHALET_DIR_WIN', 'User')
						Write-Host 'Chalet added to user PATH (no admin rights)'
					} else {
						Write-Host 'Chalet already in user PATH'
					}
				"
				alert "NOTE: You may need to restart your terminal for chalet to be available in new sessions"
				# ------------------------------------------
			fi

    ;;

	macos)
		# temporary test to see if kiss does the job
		CORELIBS+=( kissfft )

		# Install system dependencies (skip in CI)
		if [[ -z "${CI:-}" ]]; then

			if command -v brew &> /dev/null; then
				# Experimental: Uninstall chalet if version is not the same as defined here.
				# it will be installed again soon after.
				if command -v chalet &> /dev/null; then
				    version=$(chalet --version | awk '{print $3}')
				    if [ "$CHALETVERSION" != "$version" ]; then  # Changed [[ to [ and added quotes and space
				        brew uninstall chalet
				    fi
				fi

				if ! command -v wget2 &> /dev/null; then
					# section "no wget2"
					brew install wget2
				else
					sectionOk "wget2 already installed"
				fi

				if ! command -v chalet &> /dev/null; then
					brew tap chalet-org/chalet
					brew install --cask chalet
				else
					sectionOk "chalet already installed"
				fi

				if ! command -v ninja &> /dev/null; then
					brew install ninja
				else
					sectionOk "ninja already installed"
				fi

			else
				alert "Brew not installed, won't install wget2 (github command line)"
			fi
		fi

		;;

	linux64|rpi-aarch64|rpi-armv6l)

		CORELIBS+=( kissfft )

		# Install system dependencies (skip in CI)
		if [[ -z "${CI:-}" ]]; then
			section "Installing system dependencies"

			# Arch Linux
			if [ -f "/etc/arch-release" ]; then
				${SUDO_CMD} pacman -Syu --noconfirm

				${SUDO_CMD} pacman -S --noconfirm --needed \
					gcc unzip ninja wget \
					fontconfig \
					glu mesa \
					libxrandr \
					freeglut libxmu libxxf86vm \
					libxcursor libxi libxinerama \
					libunwind \
					glib2 glib2-devel \
					gstreamer gst-plugins-base gst-plugins-bad gst-plugins-good gst-libav \
					openal libsndfile \
					# Wayland support
					wayland libxkbcommon
			else
				# Check which packages are missing before installing
				UBUNTU_PACKAGES=(
					ninja-build wget2
					libfontconfig1-dev
					libglu1-mesa-dev libgl1-mesa-dev
					libxrandr-dev
					freeglut3-dev libxmu-dev libxxf86vm-dev libudev-dev
					libxcursor-dev libxi-dev libxinerama-dev
					libunwind-dev
					libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev
					gstreamer1.0-x gstreamer1.0-plugins-bad gstreamer1.0-alsa
					gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-libav
					libopenal-dev libsndfile1-dev
					# Wayland support
					libwayland-dev libwayland-egl1-mesa libxkbcommon-dev
				)

				MISSING_PACKAGES=()
				for pkg in "${UBUNTU_PACKAGES[@]}"; do
					if ! dpkg -l "$pkg" 2>/dev/null | grep -q "^ii"; then
						MISSING_PACKAGES+=("$pkg")
					fi
				done

				if [[ ${#MISSING_PACKAGES[@]} -gt 0 ]]; then
					section "Installing missing packages: ${MISSING_PACKAGES[*]}"
					${SUDO_CMD} apt-get update
					${SUDO_CMD} apt-get -y install "${MISSING_PACKAGES[@]}"
				else
					sectionOk "All system dependencies already installed"
				fi
			fi

			# Check if chalet is installed and if version matches
			NEED_INSTALL=false
			if command -v chalet &> /dev/null; then
				version=$(chalet --version | awk '{print $3}')
				if [ "$CHALETVERSION" != "$version" ]; then
					section "chalet version mismatch ($version != $CHALETVERSION), uninstalling..."
					# Arch Linux
					if [[ -f /etc/arch-release ]]; then
						${SUDO_CMD} rm -f /usr/local/bin/chalet
					else
						# UBUNTU
						${SUDO_CMD} dpkg -r chalet 2>/dev/null || true
					fi
					NEED_INSTALL=true
				else
					sectionOk "chalet already installed ($version)"
				fi
			else
				NEED_INSTALL=true
			fi

			if [ "$NEED_INSTALL" = true ]; then
				# Arch Linux
				if [[ -f /etc/arch-release ]]; then
					section "Installing chalet on Arch Linux..."
					if [[ ${PLATFORM} == 'linux64' ]]; then
						CHALET_ARCH="x86_64"
					else
						CHALET_ARCH="aarch64"
					fi

					# Use wget2 if available, otherwise curl
					if command -v wget2 &>/dev/null; then
						wget2 -O chalet-${CHALET_ARCH}-linux-gnu.zip https://github.com/chalet-org/chalet/releases/download/v${CHALETVERSION}/chalet-${CHALET_ARCH}-linux-gnu.zip
					else
						curl -L -O https://github.com/chalet-org/chalet/releases/download/v${CHALETVERSION}/chalet-${CHALET_ARCH}-linux-gnu.zip
					fi
					unzip -o chalet-${CHALET_ARCH}-linux-gnu.zip
					${SUDO_CMD} mv chalet /usr/local/bin/
					${SUDO_CMD} chmod +x /usr/local/bin/chalet
					rm -f chalet-${CHALET_ARCH}-linux-gnu.zip
				else
					# UBUNTU
					section "Installing chalet..."
					if [[ ${PLATFORM} == 'linux64' ]]; then
						wget2 -O chalet_${CHALETVERSION}_amd64.deb https://github.com/chalet-org/chalet/releases/download/v${CHALETVERSION}/chalet_${CHALETVERSION}_amd64.deb &&
						${SUDO_CMD} dpkg -i chalet*.deb
					else
						# there is arm only also.
						wget2 -O chalet_${CHALETVERSION}_arm64.deb https://github.com/chalet-org/chalet/releases/download/v${CHALETVERSION}/chalet_${CHALETVERSION}_arm64.deb &&
						${SUDO_CMD} dpkg -i chalet*.deb
					fi
				fi
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
	if gh_is_authenticated; then
		section "Downloading with GH (github command line)"
		gh release download ${LIBSVERSION} -R ofWorks/ofLibs --pattern "ofLibs_*_${PLATFORM}.zip" -D "${DOWNLOAD}"


	elif command -v wget2 &>/dev/null; then
		# wget2 is available - use it for parallel downloads
		PARAMS=""
		for LIBNAME in "${ALLLIBS[@]}"; do
			PARAMS+=" https://github.com/ofWorks/ofLibs/releases/download/${LIBSVERSION}/ofLibs_${LIBNAME}_${PLATFORM}.zip"

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
		for LIBNAME in "${CORELIBS[@]}"; do
			local filepath="${DOWNLOAD}/ofLibs_${LIBNAME}_${PLATFORM}.zip"
			executa wget -N --no-verbose --show-progress \
				"https://github.com/ofWorks/ofLibs/releases/download/${LIBSVERSION}/ofLibs_${LIBNAME}_${PLATFORM}.zip" \
				-P "${DOWNLOAD}"
		done
	elif command -v powershell.exe &>/dev/null; then
		# Fallback to PowerShell on Windows
		section "Downloading with PowerShell (sequential)"
		for LIBNAME in "${CORELIBS[@]}"; do
			local filepath="${DOWNLOAD}/ofLibs_${LIBNAME}_${PLATFORM}.zip"
			local url="https://github.com/ofWorks/ofLibs/releases/download/${LIBSVERSION}/ofLibs_${LIBNAME}_${PLATFORM}.zip"
			executa powershell.exe -Command "Invoke-WebRequest -Uri '${url}' -OutFile '${filepath}'"
		done
	else
		# Fallback to curl - sequential downloads
		section "Downloading with curl (sequential)"
		for LIBNAME in "${CORELIBS[@]}"; do
			local filepath="${DOWNLOAD}/ofLibs_${LIBNAME}_${PLATFORM}.zip"
			# Only use -z if file exists, otherwise just download
			if [[ -f "${filepath}" ]]; then
				executa curl -L -z "${filepath}" -o "${filepath}" \
				"https://github.com/ofWorks/ofLibs/releases/download/${LIBSVERSION}/ofLibs_${LIBNAME}_${PLATFORM}.zip"
			else
				executa curl -L -o "${filepath}" \
				"https://github.com/ofWorks/ofLibs/releases/download/${LIBSVERSION}/ofLibs_${LIBNAME}_${PLATFORM}.zip"
			fi
		done
	fi
}

unzipCore() {

	section "Uncompress ofWorks Core Libs"

	# Create licenses folder (use _licenses to avoid case conflicts on macOS)
	mkdir -p "${LIBS_FOLDER}/_licenses"

	for LIBNAME in "${CORELIBS[@]}"; do
		filename="${DOWNLOAD}/ofLibs_${LIBNAME}_${PLATFORM}.zip"
		executa unzip -qq -o "${filename}" -d "${LIBS_FOLDER}"

		# Move license files to _licenses folder with library name prepended
		# Include common case variations for cross-platform compatibility
		# Put lowercase first to prefer original case on case-insensitive filesystems
		for file in \
			"${LIBS_FOLDER}"/*.{txt,md,mit} \
			"${LIBS_FOLDER}"/*.{TXT,MD,MIT} \
			"${LIBS_FOLDER}"/{license,License,LICENSE,licence,Licence,LICENCE} \
			"${LIBS_FOLDER}"/{licenses,Licenses,LICENSES,licences,Licences,LICENCES} \
			"${LIBS_FOLDER}"/{copying,Copying,COPYING} \
			"${LIBS_FOLDER}"/{copying,Copying,COPYING}.* \
			"${LIBS_FOLDER}"/{notice,Notice,NOTICE} \
			"${LIBS_FOLDER}"/{notice,Notice,NOTICE}.*; do
			if [[ -f "$file" ]]; then
				basename=$(basename "$file")
				mv "$file" "${LIBS_FOLDER}/_licenses/${LIBNAME}_${basename}"
			fi
		done
		# Also handle LICENSES directory if it exists
		if [[ -d "${LIBS_FOLDER}/LICENSES" ]]; then
			mv "${LIBS_FOLDER}/LICENSES" "${LIBS_FOLDER}/_licenses/${LIBNAME}_LICENSES"
		fi

			# rm -rf "${LIBS_FOLDER}"/*.{txt,md,MIT}
			# rm -rf "${LIBS_FOLDER}"/{LICENSE,COPYING,LICENSES}
			# rm -rf "${LIBS_FOLDER}"/COPYING.*
	done
}

unzipAddons() {
	section "Uncompress ofWorks Addons Libs"

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

if [[ "$wipeDownloadsAfterInstall" == true && -d "${DOWNLOAD}" ]]; then
	echo "Removing Downloaded and Installed Libraries"
	rm -rf "${DOWNLOAD}"
fi

# Save the commit hash after successful installation
if [[ -n "$REMOTE_COMMIT" ]]; then
	echo "$REMOTE_COMMIT" > "$OF_LIBS_COMMIT_FILE"
	sectionOk "Saved commit hash: $REMOTE_COMMIT"
fi

sectionOk "Install ofLibs done"

# For Windows: create a sourceable file with PATH update
# This allows parent scripts to pick up the chalet PATH
if [[ "$PLATFORM" == "vs" ]]; then
	CHALET_EXPORT_FILE="./.tools/export_path.sh"
	mkdir -p "./.tools"
	CHALET_DIR_ABS="$(cd ./.tools/chalet 2>/dev/null && pwd || echo "")"
	if [[ -n "$CHALET_DIR_ABS" ]]; then
		echo "export PATH=\"$CHALET_DIR_ABS:\$PATH\"" > "$CHALET_EXPORT_FILE"
	fi

	if ! command -v chalet &>/dev/null; then
		echo ""
		alert "WARNING: chalet was installed but is not available in the current PATH."
		alert "If running from a script, use: source $(cd ./.tools && pwd)/export_path.sh"
		alert "Or restart your terminal."
	fi
fi

trap 'printf "${NC}"' EXIT
