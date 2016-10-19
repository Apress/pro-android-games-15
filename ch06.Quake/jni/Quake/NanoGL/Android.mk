# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := NanoGL

INCLUDE := -Iapps/NanoGL/project/jni/GL

LOCAL_CFLAGS := -O2 $(INCLUDE) # -DHAVE_SYS_TYPES_H -DHAVE_MMAP=0 -D__ANDROID__

# SDK 1.5
#-L/cygdrive/c/eclipse-SDK/android-ndk-1.5_r1/build/platforms/android-1.5/arch-arm/usr/lib -lEGL -lGLESv1_CM
#LOCAL_LDLIBS :=  
#LOCAL_STATIC_LIBRARIES :=

LOCAL_SRC_FILES := \
	eglwrap.cpp \
	nanogl.cpp \
	nanoWrap.cpp
  

include $(BUILD_STATIC_LIBRARY)

