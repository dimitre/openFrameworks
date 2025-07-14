meta:
	ADDON_NAME = ofxURL
	ADDON_DESCRIPTION = Addon for loading files remotely via http or https
	ADDON_AUTHOR = OF Team
	ADDON_URL = http://github.com/openframeworks/openFrameworks

common:
	ADDON_DEFINES = OFXURL

macos:
	ADDON_FRAMEWORKS = Foundation SystemConfiguration
	# ADDON_LIBS += $(OF_ROOT)/libs/macos/lib/libcurl.a
	# ADDON_LIBS += $(OF_ROOT)/libs/macos/lib/libcrypto.a
	ADDON_LIBS = ../../../libs/macos/lib/libcurl.a
	ADDON_LIBS += ../../../libs/macos/lib/libcrypto.a
	ADDON_LIBS += ../../../libs/macos/lib/libssl.a
