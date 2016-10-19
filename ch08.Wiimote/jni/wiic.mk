#LOCAL_PATH := $(call my-dir)

#include $(CLEAR_VARS)

LOCAL_MODULE    := wiimote

DIR := WiiCNew/src/wiic

SOURCES := 	$(DIR)/balanceboard.c $(DIR)/classic.c \
	$(DIR)/dynamics.c \
	$(DIR)/events.c \
	$(DIR)/io.c \
	$(DIR)/io_nix.c \
	$(DIR)/ir.c \
	$(DIR)/nunchuk.c \
	$(DIR)/guitar_hero_3.c \
	$(DIR)/speaker.c \
	$(DIR)/motionplus.c \
	$(DIR)/wiic.c \
	android/jni_wiic.c \
	android/jni_wiic_handlers.c \

		  
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include $(LOCAL_PATH)/$(DIR)

#-DWITH_WIIC_DEBUG
LOCAL_CFLAGS := -DLINUX  -DANDROID 

#LOCAL_ALLOW_UNDEFINED_SYMBOLS := true

LOCAL_SRC_FILES := $(SOURCES)

LOCAL_LDLIBS := -L$(LOCAL_PATH)/lib -Ljni/lib -llog -lbluetooth

include $(BUILD_SHARED_LIBRARY)

#dump:
#	@echo inc=$(LOCAL_C_INCLUDES)