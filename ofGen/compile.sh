#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

# chalet detects the C++ compiler by running `g++ -v` and searching its output
# for the literal English word "version". Under a non-English locale GCC
# localizes that banner, so detection fails with:
#   "Error getting the version and description for: '/usr/bin/g++'"
# Force a stable C locale on Linux so the probe is always parseable.
# (C.UTF-8 keeps UTF-8 path handling; scoped to Linux because macOS lacks it.)
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
	export LC_ALL=C.UTF-8
fi

COLOR='\033[0;32m'
NC='\033[0m' # No Color

# Parse arguments
FORCE=false
for arg in "$@"; do
    case $arg in
        --force)
            FORCE=true
            shift
            ;;
    esac
done

section() {
	printf "💻${COLOR} ${@} ${NC}\n\r"
}

sectionInstall() {
    printf "💿${COLOR} ${@} ${NC}\n\r"
}

# Auto-detect platform
if [[ "$OSTYPE" == "cygwin"* ]]; then
	PLATFORM=vs
elif [[ "$OSTYPE" == "msys"* ]]; then
	PLATFORM=msys2
elif [[ "$OSTYPE" == "darwin"* ]]; then
	PLATFORM=macos
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
	ARCH="$(uname -m)"
	if [[ "$ARCH" == "x86_64" ]]; then
		PLATFORM=linux64
	# elif [[ "$ARCH" == "aarch64" ]] && [[ -f /etc/rpi-issue ]]; then
	elif [[ "$ARCH" == "aarch64" ]]; then
		PLATFORM=rpi-aarch64
	elif [[ "$ARCH" == "armv6l" ]] || [[ "$ARCH" == "armv7l" ]]; then
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

section "Detected platform: $PLATFORM"

# Check if libs directory exists
if [[ ! -d "../libs/${PLATFORM}" ]]; then
	echo "Error: Library directory not found: ../libs/${PLATFORM}"
	exit 1
fi

# Version check to skip unnecessary rebuilds
ofgen_latest_version=$(grep '^version:' chalet.yaml | awk '{print $2}')
ofgen_installed_version=$(ofgen --version 2>/dev/null | tr -d '[:space:]' || true)

if [ "$ofgen_installed_version" == "$ofgen_latest_version" ]; then
    if [ "$FORCE" = true ]; then
        section "Forcing rebuild of ofgen $ofgen_latest_version..."
    else
        section "ofgen $ofgen_latest_version is already up to date, skipping build. Use --force to rebuild"
        exit 0
    fi
elif [ -z "$ofgen_installed_version" ]; then
	section "Installing ofgen $ofgen_latest_version..."
else
	section "Recompiling ofgen ($ofgen_installed_version -> $ofgen_latest_version)..."
fi

section "Compiling ofgen"

# Determine if we need sudo
if command -v sudo &> /dev/null; then
  SUDO_CMD="sudo"
else
  SUDO_CMD=""
fi

if command -v chalet &> /dev/null; then
	CCACHE_PARAM=""
	if [[ "${CI:-false}" == "true" ]]; then
		CCACHE_PARAM=" --compiler-cache --show-commands"
	fi

	chalet bundle ${CCACHE_PARAM} &&
	if [[ -d "./dist" ]]; then

		if [[ $PLATFORM == 'vs' ]]; then
			DIST_PATH=$(cygpath -w "$(pwd)/dist")
			powershell.exe -ExecutionPolicy Bypass -Command "
					\$dist = '$DIST_PATH'
					\$userPath = [Environment]::GetEnvironmentVariable('Path','User')
					if (\$userPath -split [IO.Path]::PathSeparator -notcontains \$dist) {
							[Environment]::SetEnvironmentVariable('Path', \$userPath + [IO.Path]::PathSeparator + \$dist, 'User')
					}
			"
			section "Added to Windows PATH"
		else

			sectionInstall "ofWorks ofgen (command line tool for generating OF projects)"
			echo This will install a symlink in /usr/local/bin/ so ofgen can be called from any directory.
			echo You will be asked for user password.

		   	if [[ ! -d "/usr/local/bin" ]]; then
		   		echo "/usr/local/bin does not exist. creating."
		       	${SUDO_CMD} mkdir /usr/local/bin
			fi

			# echo ${SUDO_CMD} ln -sf "$PWD/dist/ofgen" /usr/local/bin/ofgen
			${SUDO_CMD} ln -sf "$PWD/dist/ofgen" /usr/local/bin/ofgen

			echo All good!

		fi


	fi
else
	printf "No Cmake or Chalet found"
fi



# if command -v cmake &> /dev/null; then

# 	run_cmake() {
# 		mkdir -p build
# 		cd build
# 		cmake .. \
# 			-DCMAKE_BUILD_TYPE=Release \
# 			-DYAMLCPP_ROOT=../../libs/${PLATFORM} \
# 			-DFMT_ROOT=../../libs/${PLATFORM} \
# 			-DNLOHMANN_JSON_ROOT=../../libs/${PLATFORM}
# 			# -DPUGIXML_ROOT=../../libs/${PLATFORM} \
# }

# 	if ! run_cmake; then
# 		echo "CMake failed, cleaning build directory and retrying..."
# 		cd ..
# 		rm -rf build
# 		run_cmake
# 	fi
# 	cmake --build . --config Release

# 	# Auto-install in CI, ask in interactive mode
# 	if [[ "${CI:-false}" == "true" ]]; then
# 		section "CI detected, auto-installing..."
# 		${SUDO_CMD} cmake --install . --config Release
# 	else
# 		# Clear any buffered input, then prompt
# 		read -t 0.01 -n 10000 discard 2>/dev/null || true
# 		read -p "Install ofgen to system? (y/n) " -n 1 -r REPLY </dev/tty
# 		echo
# 		if [[ $REPLY =~ ^[Yy]$ ]]; then
# 			${SUDO_CMD} cmake --install . --config Release
# 			section "ofWorks ofgen Installation complete"
# 		else
# 			section "Skipping installation. Binary available at: build/ofgen"
# 		fi
# 	fi
# else
# 	# cmake not found. lets go for the alternative.
# 	if command -v chalet &> /dev/null; then
# 		CCACHE_PARAM=""
# 		if [[ "${CI:-false}" == "true" ]]; then
# 			CCACHE_PARAM=" --compiler-cache --show-commands"
# 		fi

# 		chalet bundle ${CCACHE_PARAM} &&
# 		if [[ -d "./dist" ]]; then
# 			DIST_PATH=$(cygpath -w "$(pwd)/dist")
# 			powershell.exe -ExecutionPolicy Bypass -Command "
# 					\$dist = '$DIST_PATH'
# 					\$userPath = [Environment]::GetEnvironmentVariable('Path','User')
# 					if (\$userPath -split [IO.Path]::PathSeparator -notcontains \$dist) {
# 							[Environment]::SetEnvironmentVariable('Path', \$userPath + [IO.Path]::PathSeparator + \$dist, 'User')
# 					}
# 			"
# 			section "Added to Windows PATH"
# 		fi
# 	else
# 		printf "No Cmake or Chalet found"
# 	fi
# fi
