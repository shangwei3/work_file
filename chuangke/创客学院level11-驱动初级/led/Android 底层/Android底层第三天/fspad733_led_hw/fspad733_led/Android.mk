LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := fspad733_led.default
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SRC_FILES := fspad733_led_hw.c
LOCAL_SHARED_LIBRARIES := liblog libcutils
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
