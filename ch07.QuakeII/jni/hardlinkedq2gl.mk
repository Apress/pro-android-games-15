#********************************* NanoGL *********************************
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := NanoGL

DIR:= ../../ch06.Quake/jni/Quake/NanoGL

LOCAL_C_INCLUDES := ../ch06.Quake/jni/Quake/NanoGL/GL

LOCAL_SRC_FILES := 	$(DIR)/eglwrap.cpp \
	$(DIR)/nanogl.cpp 	\
	$(DIR)/nanoWrap.cpp
  

include $(BUILD_STATIC_LIBRARY)

#********************************* QUAKE *********************************

BUILDDIR:=quake2-3.21

include $(CLEAR_VARS)

LOCAL_MODULE    := quake2

COMMON_CFLAGS :=-DANDROID -DGLQUAKE -DLIBQUAKE2 -Dstricmp=strcasecmp

LOCAL_CFLAGS := $(COMMON_CFLAGS) -DREF_HARD_LINKED -DGAME_HARD_LINKED -DEXPORT_C= -DEXTERN_C=

LOCAL_C_INCLUDES := $(COMMON_C_INCLUDES) ../ch06.Quake/jni/Quake/NanoGL


#LIB_ROOT := /cygdrive/c/eclipse-SDK/android-ndk-r5c/platforms/android-4
#-L$(LIB_ROOT)/arch-arm/usr/lib

LOCAL_LDLIBS := -llog -ldl

#LOCAL_SHARED_LIBRARIES := -L$(SDK_ROOT)/arch-arm/usr/lib  -llog -ldl
#LOCAL_LDLIBS := $(COMMON_LDLIBS)

LOCAL_STATIC_LIBRARIES := libNanoGL
#LOCAL_STATIC_LIBRARIES += $(NANOGL_LIB)


# Q2 client
QUAKE2_OBJS := \
	$(BUILDDIR)/client/cl_cin.c \
	$(BUILDDIR)/client/cl_ents.c \
	$(BUILDDIR)/client/cl_fx.c \
	$(BUILDDIR)/client/cl_input.c \
	$(BUILDDIR)/client/cl_inv.c \
	$(BUILDDIR)/client/cl_main.c \
	$(BUILDDIR)/client/cl_newfx.c \
	$(BUILDDIR)/client/cl_parse.c \
	$(BUILDDIR)/client/cl_pred.c \
	$(BUILDDIR)/client/cl_tent.c \
	$(BUILDDIR)/client/cl_scrn.c \
	$(BUILDDIR)/client/cl_view.c \
	$(BUILDDIR)/client/console.c \
	$(BUILDDIR)/client/keys.c \
	$(BUILDDIR)/client/menu.c \
	$(BUILDDIR)/client/snd_dma.c \
	$(BUILDDIR)/client/snd_mem.c \
	$(BUILDDIR)/client/snd_mix.c \
	$(BUILDDIR)/client/qmenu.c \
	$(BUILDDIR)/game/m_flash.c \
	\
	$(BUILDDIR)/qcommon/cmd.c \
	$(BUILDDIR)/qcommon/cmodel.c \
	$(BUILDDIR)/qcommon/common.c \
	$(BUILDDIR)/qcommon/crc.c \
	$(BUILDDIR)/qcommon/cvar.c \
	$(BUILDDIR)/qcommon/files.c \
	$(BUILDDIR)/qcommon/md4.c \
	$(BUILDDIR)/qcommon/net_chan.c \
	\
	$(BUILDDIR)/server/sv_ccmds.c \
	$(BUILDDIR)/server/sv_ents.c \
	$(BUILDDIR)/server/sv_game.c \
	$(BUILDDIR)/server/sv_init.c \
	$(BUILDDIR)/server/sv_main.c \
	$(BUILDDIR)/server/sv_send.c \
	$(BUILDDIR)/server/sv_user.c \
	$(BUILDDIR)/server/sv_world.c \
	\
	$(BUILDDIR)/linux/cd_linux.c \
	$(BUILDDIR)/linux/q_shlinux.c \
	$(BUILDDIR)/linux/vid_menu.c \
	$(BUILDDIR)/linux/glob.c \
	$(BUILDDIR)/linux/net_udp.c \
	\
	$(BUILDDIR)/qcommon/pmove.c \
	\
	$(BUILDDIR)/android/jni_quake.c \
	$(BUILDDIR)/android/snd_android.c \
	$(BUILDDIR)/android/vid_so.c \
	$(BUILDDIR)/android/sys_linux.c \

CGAME_SRC := \
	$(BUILDDIR)/game/q_shared.c \
	$(BUILDDIR)/game/g_ai.c \
	$(BUILDDIR)/game/p_client.c \
	$(BUILDDIR)/game/g_cmds.c \
	$(BUILDDIR)/game/g_svcmds.c \
	$(BUILDDIR)/game/g_chase.c \
	$(BUILDDIR)/game/g_combat.c \
	$(BUILDDIR)/game/g_func.c \
	$(BUILDDIR)/game/g_items.c \
	$(BUILDDIR)/game/g_main.c \
	$(BUILDDIR)/game/g_misc.c \
	$(BUILDDIR)/game/g_monster.c \
	$(BUILDDIR)/game/g_phys.c \
	$(BUILDDIR)/game/g_save.c \
	$(BUILDDIR)/game/g_spawn.c \
	$(BUILDDIR)/game/g_target.c \
	$(BUILDDIR)/game/g_trigger.c \
	$(BUILDDIR)/game/g_turret.c \
	$(BUILDDIR)/game/g_utils.c \
	$(BUILDDIR)/game/g_weapon.c \
	$(BUILDDIR)/game/m_actor.c \
	$(BUILDDIR)/game/m_berserk.c \
	$(BUILDDIR)/game/m_boss2.c \
	$(BUILDDIR)/game/m_boss3.c \
	$(BUILDDIR)/game/m_boss31.c \
	$(BUILDDIR)/game/m_boss32.c \
	$(BUILDDIR)/game/m_brain.c \
	$(BUILDDIR)/game/m_chick.c \
	$(BUILDDIR)/game/m_flipper.c \
	$(BUILDDIR)/game/m_float.c \
	$(BUILDDIR)/game/m_flyer.c \
	$(BUILDDIR)/game/m_gladiator.c \
	$(BUILDDIR)/game/m_gunner.c \
	$(BUILDDIR)/game/m_hover.c \
	$(BUILDDIR)/game/m_infantry.c \
	$(BUILDDIR)/game/m_insane.c \
	$(BUILDDIR)/game/m_medic.c \
	$(BUILDDIR)/game/m_move.c \
	$(BUILDDIR)/game/m_mutant.c \
	$(BUILDDIR)/game/m_parasite.c \
	$(BUILDDIR)/game/m_soldier.c \
	$(BUILDDIR)/game/m_supertank.c \
	$(BUILDDIR)/game/m_tank.c \
	$(BUILDDIR)/game/p_hud.c \
	$(BUILDDIR)/game/p_trail.c \
	$(BUILDDIR)/game/p_view.c \
	$(BUILDDIR)/game/p_weapon.c \
	
REF_FILES := \
	$(BUILDDIR)/ref_gl/gl_draw.c \
	$(BUILDDIR)/ref_gl/gl_image.c \
	$(BUILDDIR)/ref_gl/gl_light.c \
	$(BUILDDIR)/ref_gl/gl_mesh.c \
	$(BUILDDIR)/ref_gl/gl_model.c \
	$(BUILDDIR)/ref_gl/gl_rmain.c \
	$(BUILDDIR)/ref_gl/gl_rmisc.c \
	$(BUILDDIR)/ref_gl/gl_rsurf.c \
	$(BUILDDIR)/ref_gl/gl_warp.c \
	\
	$(BUILDDIR)/linux/qgl_linux.c \
	$(BUILDDIR)/android/gl_glx.c \

LOCAL_SRC_FILES := $(QUAKE2_OBJS) $(CGAME_SRC) $(REF_FILES) 

include $(BUILD_SHARED_LIBRARY)
