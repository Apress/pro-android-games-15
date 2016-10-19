#********************************* NanoGL *********************************
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := NanoGL

DIR:= $(LOCAL_MODULE)

LOCAL_C_INCLUDES := jni/Quake/NanoGL/GL

LOCAL_SRC_FILES := 	$(DIR)/eglwrap.cpp \
	$(DIR)/nanogl.cpp 	\
	$(DIR)/nanoWrap.cpp
  

include $(BUILD_STATIC_LIBRARY)

#********************************* QUAKE *********************************

include $(CLEAR_VARS)

LOCAL_MODULE    := quake


LOCAL_CFLAGS := -DANDROID -DGLQUAKE -Wfatal-errors \
	-D_stricmp=strcasecmp -D_strnicmp=strncasecmp -Dstricmp=strcasecmp -Dstrnicmp=strncasecmp \
	-O3 -fexpensive-optimizations -fomit-frame-pointer \
	

#LOCAL_C_INCLUDES := jni/Quake/android ../NanoGL/project/jni
LOCAL_C_INCLUDES := jni/Quake/android jni/Quake/NanoGL

# SDK 1.5 -lEGL -lGLESv1_CM -lui
SDK_ROOT := /cygdrive/c/eclipse-SDK/android-ndk-r5c/platforms/android-3

# libs for the shared lib
LOCAL_LDLIBS := -L$(SDK_ROOT)/arch-arm/usr/lib -ldl -llog

#LOCAL_STATIC_LIBRARIES 	:= ../../../../../../../../../cygdrive/c/eclipse-SDK/android-ndk-r5c/out/apps/NanoGL/armeabi/libNanoGL
#LOCAL_STATIC_LIBRARIES 	:= /cygdrive/c/eclipse-SDK/android-ndk-r5c/out/apps/NanoGL/armeabi/libNanoGL

LOCAL_STATIC_LIBRARIES 	:= libNanoGL

#LOCAL_SHARED_LIBRARIES := -ldl

RENDERER_SRC := gl_draw.c \
	gl_mesh.c \
	gl_model.c \
	gl_refrag.c \
	gl_rlight.c \
	gl_rmisc.c \
	gl_screen.c \
	gl_rmain.c \
	gl_rsurf.c \
	gl_warp.c \

SND_SRC := 	snd_dma.c \
	snd_mem.c \
	snd_mix.c \

ANDROID_SRC := 	android/jni_quake.c \
	android/snd_android.c \
	android/gl_vidandroid.c \
	android/sys_android.c \

LOCAL_SRC_FILES := 	\
	cl_demo.c \
	cl_input.c \
	cl_main.c \
	cl_parse.c \
	cl_tent.c \
	chase.c \
	cmd.c \
	common.c \
	console.c \
	cvar.c \
	host.c \
	host_cmd.c \
	keys.c \
	menu.c \
	mathlib.c \
	net_dgrm.c \
	net_loop.c \
	net_main.c \
	pr_cmds.c \
	pr_edict.c \
	pr_exec.c \
	r_part.c \
	sbar.c \
	sv_main.c \
	sv_phys.c \
	sv_move.c \
	sv_user.c \
	zone.c	\
	view.c	\
	wad.c \
	world.c \
	net_udp.c \
	net_bsd.c \
	cd_null.c \
	crc.c net_vcr.c \
	$(RENDERER_SRC) $(SND_SRC) $(ANDROID_SRC)

include $(BUILD_SHARED_LIBRARY)
