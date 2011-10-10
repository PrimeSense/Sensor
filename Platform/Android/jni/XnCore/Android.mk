# XnCore Android makefile.
# libXnCore.so
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS) 

# set path to source
MY_PREFIX := $(LOCAL_PATH)/../../../../Source/XnCore/

# list all source files
MY_SRC_FILES := \
	$(MY_PREFIX)*.cpp

# expand the wildcards
MY_SRC_FILE_EXPANDED := $(wildcard $(MY_SRC_FILES))

# make those paths relative to here
LOCAL_SRC_FILES := $(MY_SRC_FILE_EXPANDED:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/../../../../Include/ \
	$(LOCAL_PATH)/../../../../Source/ \

LOCAL_CFLAGS:= -fvisibility=hidden -DXN_CORE_EXPORTS

LOCAL_LDFLAGS += -Wl,--export-dynamic --dynamic-linker 

LOCAL_SHARED_LIBRARIES := OpenNI libusb

LOCAL_PREBUILT_LIBS := libc 

LOCAL_MODULE := XnCore

include $(BUILD_SHARED_LIBRARY)
