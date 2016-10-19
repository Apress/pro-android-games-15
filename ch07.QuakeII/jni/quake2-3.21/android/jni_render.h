
#ifndef JNI_WOLF_H
#define JNI_WOLF_H

#include <stdio.h>

/* JNI Includes */
//#include <jni.h>
#include <SDL.h>

SDL_Surface * JNI_SurfaceNew(int width, int height, int bpp, int flags);

/*
// Send a fatal error msg, before exi
void jni_sys_error(char *format, ...);

// printf str messages back to java
void jni_printf(char *format, ...);


// For sending img updates back to Java
void jni_init_video(int width, int height);

void jni_init_audio (const int frq, const int channels, const int bits);

// MUSIC
void jni_start_music (const char * name, int loop);
void jni_stop_music (); //const char * name);
void jni_set_music_volume (int vol);
*/

#endif
