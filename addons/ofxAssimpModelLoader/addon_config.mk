# All variables and this file are optional, if they are not present the PG and the
# makefiles will try to parse the correct values from the file system.
#
# Variables that specify exclusions can use % as a wildcard to specify that anything in
# that position will match. A partial path can also be specified to, for example, exclude
# a whole folder from the parsed paths from the file system
#
# Variables can be specified using = or +=
# = will clear the contents of that variable both specified from the file or the ones parsed
# from the file system
# += will add the values to the previous ones in the file or the ones parsed from the file
# system
#
# The PG can be used to detect errors in this file, just create a new project with this addon
# and the PG will write to the console the kind of error and in which line it is

meta:
	ADDON_NAME = ofxAssimpModelLoader
	ADDON_DESCRIPTION = Addon for loading several 3d formats through the assimp library
	ADDON_AUTHOR = OF Team
	ADDON_TAGS = "3D" "assimp"
	ADDON_URL = http://github.com/openframeworks/openFrameworks

# linuxarmv6l:
# 	ADDON_PKG_CONFIG_LIBRARIES = assimp
# 	ADDON_LIBS_EXCLUDE = libs/assimp
# 	ADDON_INCLUDES_EXCLUDE = libs/assimp/%

# linuxarmv7l:
# 	ADDON_PKG_CONFIG_LIBRARIES = assimp
# 	ADDON_LIBS_EXCLUDE = libs/assimp
# 	ADDON_INCLUDES_EXCLUDE = libs/assimp/%

# linuxaarch64:
# 	ADDON_PKG_CONFIG_LIBRARIES = assimp
# 	ADDON_LIBS_EXCLUDE = libs/assimp
# 	ADDON_INCLUDES_EXCLUDE = libs/assimp/%

# linux:
# 	ADDON_PKG_CONFIG_LIBRARIES = assimp
# 	ADDON_LIBS_EXCLUDE = libs/assimp
# 	ADDON_INCLUDES_EXCLUDE = libs/assimp/%

# linux64:
# 	ADDON_PKG_CONFIG_LIBRARIES = assimp
# 	ADDON_LIBS_EXCLUDE = libs/assimp
# 	ADDON_INCLUDES_EXCLUDE = libs/assimp/%

# msys2:
# 	ADDON_PKG_CONFIG_LIBRARIES = assimp
# 	ADDON_LIBS_EXCLUDE = libs/assimp
# 	ADDON_INCLUDES_EXCLUDE = libs/assimp/%

# ios:
# 	ADDON_LIBS=
# 	ADDON_LIBS=libs/assimp/lib/macos/assimp.xcframework/ios-arm64/assimp.a
# 	ADDON_LIBS=libs/assimp/lib/macos/assimp.xcframework/ios-arm64_x86_64-simulator/assimp.a

# osx:
	# ADDON_INCLUDES_EXCLUDE = libs/assimp/include/%
	# ADDON_LIBS=
	# ADDON_LIBS=libs/assimp/lib/macos/assimp.xcframework/macos-arm64_x86_64/assimp.a

# emscripten:
# 	ADDON_LIBS=
# 	ADDON_LIBS=libs/assimp/lib/emscripten/libassimp.a
