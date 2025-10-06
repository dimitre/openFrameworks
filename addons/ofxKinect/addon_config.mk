
meta:
	ADDON_NAME = ofxKinect
	ADDON_DESCRIPTION = Addon for interfacing with microsoft kinect cameras
	ADDON_AUTHOR = Dan Wilcox, Theo Watson, Kyle McDonald, Arturo Castro
	ADDON_TAGS = "computer vision" "3D sensing" "kinect"
	ADDON_URL = http://github.com/ofTheo/ofxKinect

common:

linux64:
	# ADDON_PKG_CONFIG_LIBRARIES = libusb-1.0

	ADDON_SOURCES_EXCLUDE = libs/libfreenect/platform/%
	ADDON_INCLUDES_EXCLUDE = libs/libfreenect/platform/%
	# ADDON_INCLUDES_EXCLUDE += libs/libusb-1.0/%

msys2:
	# ADDON_PKG_CONFIG_LIBRARIES = libusb-1.0

	ADDON_SOURCES_EXCLUDE = libs/libfreenect/platform/%
	ADDON_INCLUDES_EXCLUDE = libs/libfreenect/platform/%
	# ADDON_INCLUDES_EXCLUDE += libs/libusb-1.0/%


vs:
	# source files, these will be usually parsed from the file system looking
	# in the src folders in libs and the root of the addon. if your addon needs
	# to include files in different places or a different set of files per platform
	# they can be specified here

	# include search paths, this will be usually parsed from the file system
	# but if the addon or addon libraries need special search paths they can be
	# specified here separated by spaces or one per line using +=
	ADDON_INCLUDES += libs/libfreenect/platform/windows

	# when parsing the file system looking for include paths exclude this for all or
	# a specific platform

linuxarmv6l:
	# ADDON_PKG_CONFIG_LIBRARIES = libusb-1.0

	ADDON_SOURCES_EXCLUDE = libs/libfreenect/platform/%
	ADDON_INCLUDES_EXCLUDE = libs/libfreenect/platform/%
	# ADDON_INCLUDES_EXCLUDE += libs/libusb-1.0/%

linuxaarch64:
	# ADDON_PKG_CONFIG_LIBRARIES = libusb-1.0

	ADDON_SOURCES_EXCLUDE = libs/libfreenect/platform/%
	ADDON_INCLUDES_EXCLUDE = libs/libfreenect/platform/%
	# ADDON_INCLUDES_EXCLUDE += libs/libusb-1.0/%

macos:
	ADDON_SOURCES_EXCLUDE = libs/libfreenect/platform/%
	ADDON_INCLUDES_EXCLUDE = libs/libfreenect/platform/%

ios:
