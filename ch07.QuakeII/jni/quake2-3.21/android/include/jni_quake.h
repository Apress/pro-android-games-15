
#ifndef JNI_QUAKE_H
#define JNI_QUAKE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* JNI Includes */
#include <jni.h>



// Send a fatal error msg, before exi
void jni_sys_error(const char * text);

// printf str messages back to java
void jni_printf(char *format, ...);

// Quake console msgs
//void jni_con_printf(char *format, ...);

// For sending img updates back to Java
void jni_init_video(int width, int height);
//void jni_send_pixels(int * data, int x, int y, int w, int h);
//void jni_start_sound (const char * name, int vol);

//void jni_gl_endrendering(void);

const int getArrayLen(JNIEnv * env, jobjectArray jarray);


#endif
