LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := ch02

INCLUDES := 

LOCAL_CFLAGS := -O2 $(INCLUDES) 


LOCAL_SRC_FILES := lib.c
  

include $(BUILD_SHARED_LIBRARY)

