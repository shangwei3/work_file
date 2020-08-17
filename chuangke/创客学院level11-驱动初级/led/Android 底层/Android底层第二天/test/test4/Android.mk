LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE = libas
LOCAL_SRC_FILES = $(call all-c-files-under,src)
LOCAL_MODULE_PATH :=$(LOCAL_PATH)/lib
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE = test
LOCAL_SRC_FILES = test.c
LOCAL_MODULE_PATH := $(LOCAL_PATH)/bin
LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_SHARED_LIBRARIES += liblog #link Android lib
LOCAL_LDFLAGS += -L$(LOCAL_PATH)/lib -las # link self lib
include $(BUILD_EXECUTABLE)
