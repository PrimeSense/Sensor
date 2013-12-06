# XnDeviceSensorV2 Android makefile.
# libXnDeviceSensorV2.so
#

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS) 

# set path to source
MY_PREFIX := $(LOCAL_PATH)/../../../../Source/XnDeviceSensorV2/

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
	$(LOCAL_PATH)/../../../../Source/XnCommon \

LOCAL_CFLAGS := -fvisibility=hidden -DXN_DEVICE_EXPORTS

LOCAL_LDFLAGS += -Wl,--export-dynamic

LOCAL_SHARED_LIBRARIES := OpenNI libusb XnCore XnFormats XnDDK

LOCAL_PREBUILT_LIBS := libc 

LOCAL_MODULE := XnDeviceSensorV2

include $(BUILD_SHARED_LIBRARY)

