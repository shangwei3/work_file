LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE = test

#LOCAL_SRC_FILES = $(call all-c-files-under,src) test.c
LOCAL_SRC_FILES = $(call all-subdir-c-files)

LOCAL_MODULE_PATH :=$(LOCAL_PATH)/bin
LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_SHARED_LIBRARIES += liblog

include $(BUILD_EXECUTABLE)
