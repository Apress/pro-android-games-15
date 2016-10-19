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

include apps/Doom/SDL/Android-SDL.mk

include $(CLEAR_VARS)

LOCAL_MODULE    := doom

LP := $(LOCAL_PATH)

# SDK root 
NDK_VER := android-ndk-r5c
SDK16_ROOT	:= platforms/android-4/arch-arm
SDK16_INC 	:= $(SDK16_ROOT)/usr/include

SDL_ROOT	:= ../$(NDK_VER)/apps/SDL
INC 		:= -Iapps/Doom/prboom \
	-I$(SDL_ROOT)/SDL-1.2.13/include \
	-I$(SDL_ROOT)/SDL_net-1.2.7 \
	-I$(SDL_ROOT)/SDL_mixer-1.2.11 \
	-I$(SDK16_INC) \

#-DHAVE_MIXER	
DOOM_FLAGS 	:= -DNORMALUNIX -DLINUX -DHAVE_NET -DUSE_SDL_NET -DHAVE_CONFIG_H
SDL_FLAGS 	:= -DHAVE_SYS_TYPES_H -DHAVE_MMAP=0 -D__ANDROID__ 

# -DANDROID -DGL_DOOM -funroll-loops -ffast-math -O2 -fexpensive-optimizations -fomit-frame-pointer
OPTS 			:= -O2 
LOCAL_CFLAGS 	:= $(DOOM_FLAGS) $(SDL_FLAGS) $(OPTS) $(INC)

# SDK >= 1.5 -lGLESv1_CM
#LOCAL_LDLIBS := -L/cygdrive/c/eclipse-SDK/android-ndk-1.5_r1/build/platforms/android-1.5/arch-arm/usr/lib -llog -lGLESv1_CM
LOCAL_LDLIBS := -L$(SDK16_ROOT)/usr/lib -llog -lGLESv1_CM


# SDL static libs location
#SDL_LIB_PATH := /cygdrive/c/eclipse-SDK/$(NDK_VER)/out/apps/SDL/armeabi

# $(SDL_LIB_PATH)libSDL_mixer 
#LOCAL_STATIC_LIBRARIES := $(SDL_LIB_PATH)/libSDL_net $(SDL_LIB_PATH)/libSDL 
LOCAL_STATIC_LIBRARIES := SDL_net SDL 

DIR := prboom

MAIN_OBJS 	:= \
 $(DIR)/am_map.c    $(DIR)/m_cheat.c     $(DIR)/p_lights.c  $(DIR)/p_user.c    $(DIR)/sounds.c \
 $(DIR)/hu_lib.c    $(DIR)/md5.c         $(DIR)/p_map.c     $(DIR)/r_bsp.c     $(DIR)/s_sound.c \
 $(DIR)/d_deh.c     $(DIR)/hu_stuff.c     $(DIR)/m_menu.c      $(DIR)/p_maputl.c  $(DIR)/r_data.c    $(DIR)/st_lib.c \
 $(DIR)/d_items.c   $(DIR)/m_misc.c      $(DIR)/p_mobj.c    $(DIR)/r_demo.c    $(DIR)/st_stuff.c \
 $(DIR)/d_main.c    $(DIR)/info.c             $(DIR)/p_plats.c   $(DIR)/r_draw.c    $(DIR)/tables.c \
 $(DIR)/doomdef.c         $(DIR)/m_random.c    $(DIR)/p_pspr.c    $(DIR)/r_filter.c  $(DIR)/version.c \
 $(DIR)/doomstat.c       $(DIR)/p_ceilng.c    $(DIR)/p_saveg.c   $(DIR)/r_fps.c     $(DIR)/v_video.c \
 $(DIR)/p_checksum.c  $(DIR)/p_setup.c   $(DIR)/r_main.c    $(DIR)/wi_stuff.c \
 $(DIR)/dstrings.c   $(DIR)/p_doors.c     $(DIR)/p_sight.c   $(DIR)/r_patch.c   $(DIR)/w_memcache.c \
 $(DIR)/f_finale.c       $(DIR)/p_enemy.c     $(DIR)/p_spec.c    $(DIR)/r_plane.c   $(DIR)/w_mmap.c \
 $(DIR)/f_wipe.c    $(DIR)/lprintf.c      $(DIR)/p_floor.c     $(DIR)/p_switch.c  $(DIR)/r_segs.c    $(DIR)/w_wad.c \
 $(DIR)/g_game.c    $(DIR)/m_argv.c       $(DIR)/p_genlin.c    $(DIR)/p_telept.c  $(DIR)/r_sky.c     $(DIR)/z_bmalloc.c \
 $(DIR)/m_bbox.c       $(DIR)/p_inter.c     $(DIR)/p_tick.c    $(DIR)/r_things.c  $(DIR)/z_zone.c \
 $(DIR)/d_client.c   $(DIR)/d_server.c \
 $(DIR)/droid/i_video.c $(DIR)/droid/i_network.c $(DIR)/droid/i_joy.c \
 $(DIR)/droid/i_system.c $(DIR)/droid/i_main.c $(DIR)/droid/i_sound.c \
 $(DIR)/droid/jni_doom.c \
 $(DIR)/droid/render.c \
 $(DIR)/mmus2mid.c \
 

GL_OBJS := 

LOCAL_SRC_FILES := $(MAIN_OBJS) $(GL_OBJS)

include $(BUILD_SHARED_LIBRARY)

