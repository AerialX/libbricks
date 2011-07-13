LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
ROOT_DIR						:=	.
SOURCE							:=	source source/io source/threading source/audio
INCLUDE							:=	include

CPPFILES						:=	$(foreach dir,$(SOURCE),$(wildcard $(dir)/*.cpp))

LOCAL_MODULE					:=	bricks
LOCAL_MODULE_FILENAME			:=	libbricks
LOCAL_SRC_FILES					:=	$(foreach file,$(CPPFILES),$(ROOT_DIR)/$(file))
LOCAL_C_INCLUDES				:=	$(foreach inc,$(INCLUDE),$(inc)) /home/aaron/android-ndk/sources/cxx-stl/stlport/stlport
LOCAL_LDLIBS					:=	-L/opt/android-ndk/sources/cxx-stl/stlport/libs/armeabi -lstlport_static
LOCAL_CFLAGS					:=	-fexceptions -frtti -O3 -g -DBRICKS_FEATURE_ANDROID

include $(BUILD_SHARED_LIBRARY)
