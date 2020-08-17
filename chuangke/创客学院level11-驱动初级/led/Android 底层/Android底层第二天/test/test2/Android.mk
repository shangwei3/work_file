LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE = libas

LOCAL_SRC_FILES = $(call all-subdir-c-files)

LOCAL_MODULE_PATH :=$(LOCAL_PATH)/lib
LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc

include $(BUILD_SHARED_LIBRARY)
