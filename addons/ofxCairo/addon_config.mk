meta:
	ADDON_NAME = ofxCairo
	ADDON_DESCRIPTION = Addon for cairo
	ADDON_AUTHOR = OF Team
	ADDON_URL = http://github.com/openframeworks/openFrameworks

# common:
	# ADDON_DEFINES = OFXURL

macos:
	ADDON_LIBS = $(OF_ROOT)/libs/macos/lib/libcairo.a
	ADDON_LIBS += $(OF_ROOT)/libs/macos/lib/libpixman-1.a
	# ADDON_LIBS += $(OF_ROOT)/libs/macos/lib/libpixman-arm-neon.a

	# ADDON_LIBS = ../../../libs/macos/lib/libcurl.a
	# ADDON_LIBS += ../../../libs/macos/lib/libcrypto.a
	# ADDON_LIBS += ../../../libs/macos/lib/libssl.a
