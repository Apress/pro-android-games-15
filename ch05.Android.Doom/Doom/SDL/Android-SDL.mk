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
#LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libSDL

SDL_ROOT := SDL/SDL-1.2.13
INCLUDE := -Iapps/SDL/$(SDL_ROOT)/include -Iapps/Doom/$(SDL_ROOT)/include

LOCAL_CFLAGS := -O2 $(INCLUDE) -DHAVE_SYS_TYPES_H -DHAVE_MMAP=0 -D__ANDROID__ -DNO_MALLINFO=1

# SDK 1.5
#LOCAL_LDLIBS := -L/cygdrive/c/eclipse-SDK/android-ndk-1.5_r1/build/platforms/android-1.5/arch-arm/usr/lib -lEGL -lGLESv1_CM

#
# Android Video (NanoGL)
# Not implemented yet
AND_NGL_FILES	:= # $(SDL_ROOT)/src/video/android/SDL_anvideo.c $(SDL_ROOT)/src/video/android/SDL_anevents.c \
	$(SDL_ROOT)/src/video/android/SDL_anmouse.c
 

#$(SDL_ROOT)/src/joystick/linux/SDL_sysjoystick.c
 
LOCAL_SRC_FILES := \
  $(SDL_ROOT)/src/SDL.c $(SDL_ROOT)/src/SDL_error.c $(SDL_ROOT)/src/SDL_fatal.c \
  $(SDL_ROOT)/src/audio/SDL_audio.c $(SDL_ROOT)/src/audio/SDL_audiocvt.c $(SDL_ROOT)/src/audio/SDL_audiodev.c $(SDL_ROOT)/src/audio/SDL_mixer.c \
  $(SDL_ROOT)/src/audio/SDL_mixer_MMX.c $(SDL_ROOT)/src/audio/SDL_mixer_MMX_VC.c $(SDL_ROOT)/src/audio/SDL_mixer_m68k.c \
  $(SDL_ROOT)/src/audio/SDL_wave.c $(SDL_ROOT)/src/cdrom/SDL_cdrom.c $(SDL_ROOT)/src/cpuinfo/SDL_cpuinfo.c $(SDL_ROOT)/src/events/SDL_active.c \
  $(SDL_ROOT)/src/events/SDL_events.c $(SDL_ROOT)/src/events/SDL_expose.c $(SDL_ROOT)/src/events/SDL_keyboard.c $(SDL_ROOT)/src/events/SDL_mouse.c \
  $(SDL_ROOT)/src/events/SDL_quit.c $(SDL_ROOT)/src/events/SDL_resize.c $(SDL_ROOT)/src/file/SDL_rwops.c $(SDL_ROOT)/src/joystick/SDL_joystick.c $(SDL_ROOT)/src/stdlib/SDL_getenv.c \
  $(SDL_ROOT)/src/stdlib/SDL_iconv.c $(SDL_ROOT)/src/stdlib/SDL_malloc.c $(SDL_ROOT)/src/stdlib/SDL_qsort.c $(SDL_ROOT)/src/stdlib/SDL_stdlib.c $(SDL_ROOT)/src/stdlib/SDL_string.c \
  $(SDL_ROOT)/src/thread/SDL_thread.c $(SDL_ROOT)/src/timer/SDL_timer.c $(SDL_ROOT)/src/video/SDL_RLEaccel.c $(SDL_ROOT)/src/video/SDL_blit.c $(SDL_ROOT)/src/video/SDL_blit_0.c \
  $(SDL_ROOT)/src/video/SDL_blit_1.c $(SDL_ROOT)/src/video/SDL_blit_A.c $(SDL_ROOT)/src/video/SDL_blit_N.c $(SDL_ROOT)/src/video/SDL_bmp.c $(SDL_ROOT)/src/video/SDL_cursor.c \
  $(SDL_ROOT)/src/video/SDL_gamma.c $(SDL_ROOT)/src/video/SDL_pixels.c $(SDL_ROOT)/src/video/SDL_stretch.c $(SDL_ROOT)/src/video/SDL_surface.c $(SDL_ROOT)/src/video/SDL_video.c \
  $(SDL_ROOT)/src/video/SDL_yuv.c $(SDL_ROOT)/src/video/SDL_yuv_mmx.c $(SDL_ROOT)/src/video/SDL_yuv_sw.c $(SDL_ROOT)/src/audio/dummy/SDL_dummyaudio.c \
  $(SDL_ROOT)/src/video/dummy/SDL_nullevents.c $(SDL_ROOT)/src/video/dummy/SDL_nullmouse.c $(SDL_ROOT)/src/video/dummy/SDL_nullvideo.c \
  $(SDL_ROOT)/src/joystick/dummy/SDL_sysjoystick.c $(SDL_ROOT)/src/cdrom/dummy/SDL_syscdrom.c $(SDL_ROOT)/src/thread/generic/SDL_syscond.c \
  $(SDL_ROOT)/src/thread/generic/SDL_sysmutex.c $(SDL_ROOT)/src/thread/generic/SDL_syssem.c $(SDL_ROOT)/src/thread/generic/SDL_systhread.c \
  $(SDL_ROOT)/src/timer/unix/SDL_systimer.c $(SDL_ROOT)/src/loadso/dummy/SDL_sysloadso.c \
  $(SDL_ROOT)/src/video/fbcon/SDL_fbvideo.c $(SDL_ROOT)/src/video/fbcon/SDL_fbevents.c \
  $(SDL_ROOT)/src/video/fbcon/SDL_fbriva.c $(SDL_ROOT)/src/video/fbcon/SDL_fbmatrox.c \
  $(SDL_ROOT)/src/video/fbcon/SDL_fbelo.c $(SDL_ROOT)/src/video/fbcon/SDL_fb3dfx.c \
  $(AND_NGL_FILES)
  
  
#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)

#
# SDL_net
#
include $(CLEAR_VARS)

SDL_ROOT := SDL/SDL_net-1.2.7

LOCAL_MODULE    := SDL_net
LOCAL_CFLAGS 	:= -O2 $(INCLUDE) -Iapps/SDL/SDL_net-1.2.7 -DHAVE_SYS_TYPES_H -DHAVE_MMAP=0 -D__ANDROID__

LOCAL_LDLIBS := -Lapps/SDL/project/libs/armeabi -lSDL

LOCAL_SRC_FILES := \
	$(SDL_ROOT)/SDLnet.c		\
	$(SDL_ROOT)/SDLnetTCP.c		\
	$(SDL_ROOT)/SDLnetUDP.c		\
	$(SDL_ROOT)/SDLnetselect.c

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)


#
# SDL_mixer
#
include $(CLEAR_VARS)

SDL_ROOT 		:= SDL_mixer-1.2.11

LOCAL_MODULE    := SDL_mixer
LOCAL_CFLAGS 	:= -O2 $(INCLUDE) -Iapps/SDL/$(SDL_ROOT) -DHAVE_SYS_TYPES_H -DHAVE_MMAP=0 -D__ANDROID__

LOCAL_LDLIBS 	:= -Lapps/SDL/project/libs/armeabi -lSDL

LOCAL_SRC_FILES := \
	$(SDL_ROOT)/mixer.c		\
	$(SDL_ROOT)/music.c		\
	$(SDL_ROOT)/load_voc.c		\
	$(SDL_ROOT)/load_aiff.c \
	$(SDL_ROOT)/effects_internal.c \
	$(SDL_ROOT)/effect_position.c \

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)
