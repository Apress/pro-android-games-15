LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := zeemote

DIR := zeemote

SOURCES := 	$(DIR)/zeemote.c $(DIR)/jni_zee.c

		  
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include $(LOCAL_PATH)/$(DIR)


LOCAL_CFLAGS := -DLINUX  -DANDROID

#LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

LOCAL_SRC_FILES := $(SOURCES)

LOCAL_LDLIBS := -L$(LOCAL_PATH)/lib -Ljni/lib -llog -lbluetooth

include $(BUILD_SHARED_LIBRARY)

