#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

COLOR='\033[0;32m'
NC='\033[0m' # No Color

section() {
	printf "💻${COLOR} ${@} ${NC}\n\r"
}

sectionInstall() {
    printf "💿${COLOR} ${@} ${NC}\n\r"
}

# Auto-detect platform
if [[ "$OSTYPE" == "msys"* ]]; then
	PLATFORM=vs
elif [[ "$OSTYPE" == "cygwin"* ]]; then
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
