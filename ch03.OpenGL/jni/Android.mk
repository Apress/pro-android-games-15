LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := ch03

INCLUDES := 

LOCAL_CFLAGS := -O2 $(INCLUDES) 


LOCAL_SRC_FILES := cuberenderer.c cube.c
  
LOCAL_LDLIBS := -llog -lGLESv1_CM

include $(BUILD_SHARED_LIBRARY)

