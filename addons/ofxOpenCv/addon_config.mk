meta:
	ADDON_NAME = ofxOpenCv
	ADDON_DESCRIPTION = Addon for computer vision using the open source library openCv
	ADDON_AUTHOR = OF Team
	ADDON_TAGS = "computer vision" "opencv" "image processing"
	ADDON_URL = http://github.com/openframeworks/openFrameworks

common:

linux64:
	# ADDON_PKG_CONFIG_LIBRARIES = opencv4 harfbuzz
	ADDON_PKG_CONFIG_LIBRARIES = harfbuzz
	# ADDON_LIBS_EXCLUDE = libs/opencv/%
	# ADDON_INCLUDES_EXCLUDE = libs/opencv
	# ADDON_INCLUDES_EXCLUDE += libs/opencv/%

linuxarmv6l:
	# ADDON_PKG_CONFIG_LIBRARIES = opencv4
	# ADDON_LIBS_EXCLUDE = libs/opencv/%
	# ADDON_INCLUDES_EXCLUDE = libs/opencv
	# ADDON_INCLUDES_EXCLUDE += libs/opencv/%

linuxaarch64:
	# ADDON_PKG_CONFIG_LIBRARIES = opencv4
	# ADDON_LIBS_EXCLUDE = libs/opencv/%
	# ADDON_INCLUDES_EXCLUDE = libs/opencv
	# ADDON_INCLUDES_EXCLUDE += libs/opencv/%
	# ADDON_LDFLAGS = -lblas -llapack

msys2:
	# ADDON_PKG_CONFIG_LIBRARIES = opencv4
	# ADDON_LIBS_EXCLUDE = libs/opencv/%
	# ADDON_INCLUDES_EXCLUDE = libs/opencv
	# ADDON_INCLUDES_EXCLUDE += libs/opencv/%

# emscripten:
# 	ADDON_LIBS =
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/liblibopenjp2.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/liblibprotobuf.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_aruco.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_bioinspired.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_calib3d.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_core.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_dnn_objdetect.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_dnn_superres.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_dnn.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_dpm.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_features2d.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_flann.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_fuzzy.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_hfs.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_img_hash.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_imgcodecs.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_imgproc.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_intensity_transform.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_line_descriptor.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_mcc.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_objdetect.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_phase_unwrapping.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_plot.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_rapid.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_reg.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_saliency.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_signal.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_structured_light.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_surface_matching.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_wechat_qrcode.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_xfeatures2d.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libopencv_xobjdetect.a
# 	ADDON_LIBS += libs/opencv/lib/emscripten/%/libzlib.a

ios:
	# osx/iOS only, any framework that should be included in the project
	ADDON_FRAMEWORKS = AssetsLibrary
