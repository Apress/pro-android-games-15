
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "include/quake_jni_Natives.h"
#include "include/jni_quake.h"

#define NATIVES_CLS "quake/jni/Natives"

#define LOG_TAG "Q2JNI"

#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG,__VA_ARGS__)

// Global env ref (for callbacks)
JavaVM *g_VM;


static jclass jNativesCls;

// swap buffers
static jmethodID jGLEndRenderMethod;

/**
  * Init JNI callbacks 
  */
int Init_JNI(JNIEnv * env) 
{
	/*
	 * Load the Image update class (called many times)
	 */
	jclass cls = (*env)->FindClass(env, NATIVES_CLS );
	jNativesCls = (*env)->NewGlobalRef(env, cls );

	if ( jNativesCls == 0 ) {
		LOGE("Unable to find class: quake/jni/Natives");
	    return 0;
	}
	
	jGLEndRenderMethod = (*env)->GetStaticMethodID(env, jNativesCls
			, "OnSwapBuffers"
			, "()V");
			
	if ( ! jGLEndRenderMethod ) {
		LOGE("Unable to load SwapBuffers()V");
	}
	
	return 1;
}

/***********************************************************
 * JNI Native methods
 ***********************************************************/

extern int main(int argc, char **argv);

/*
 * Class:     quake_jni_Natives
 * Method:    QuakeMain
 * Signature: ([Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_quake_jni_Natives_QuakeMain
  (JNIEnv * env, jclass cls, jobjectArray jargv)
{
	(*env)->GetJavaVM(env, &g_VM);

	// Extract char ** args from Java array
	jsize clen =  getArrayLen(env, jargv);

	char * args[(int)clen];

	int i;
	jstring jrow;
	for (i = 0; i < clen; i++)
	{
	    jrow = (jstring)(*env)->GetObjectArrayElement(env, jargv, i);
	    const char *row  = (*env)->GetStringUTFChars(env, jrow, 0);

	    args[i] = malloc( strlen(row) + 1);
	    strcpy (args[i], row);
		
		jni_printf("Q2Main[%d]=%s", i, args[i]);
		
	    // free java string jrow
	    (*env)->ReleaseStringUTFChars(env, jrow, row);
	}

	if ( ! Init_JNI(env) ) {
		return 0;
	}
	
	// Invoke Quake's main sub. This will loop forever
	// Program args come from Java
	main (clen, args);

	return 1;
}

/*
 * Class:     quake_jni_Natives
 * Method:    RenderFrame
 * Signature: ([Ljava/lang/String;)I
 */
extern void RenderFrame();

JNIEXPORT jint JNICALL Java_quake_jni_Natives_RenderFrame
  (JNIEnv * env, jclass cls)
{
	RenderFrame();
}


/*
 * Class:     quake_jni_Natives
 * Method:    keyPress
 * Signature: (I)I
 */
//extern void Key_Event (int key, qboolean down);

JNIEXPORT jint JNICALL Java_quake_jni_Natives_keyPress
  (JNIEnv * env, jclass cls, jint key)
{
	Key_Event((int)key, 1);
	return key;
}

/*
 * Class:     quake_jni_Natives
 * Method:    keyRelease
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_quake_jni_Natives_keyRelease
  (JNIEnv * env, jclass cls, jint key)
{
	Key_Event((int)key, 0);
	return key;
}

extern int   mx, my;

JNIEXPORT jint JNICALL Java_quake_jni_Natives_mouseLook
  (JNIEnv * env, jclass cls, jint mousex, jint mousey)
{  
	//LOGD("Mouse Look %d, %d", mousex, mousey);
	mx = (int)mousex; 
	my = (int)mousey; 
}

extern int   mouse_side, mouse_fwd;

JNIEXPORT jint JNICALL Java_quake_jni_Natives_mouseMove
  (JNIEnv * env, jclass cls, jint jx, jint jy)
{  
	//LOGD("Mouse Move %d, %d", jx, jy);
	mouse_side = (int)jx; 
	mouse_fwd = (int)jy; 
}

/*
 * Class:     quake_jni_Natives
 * Method:    sendCommand
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_quake_jni_Natives_sendCommand
  (JNIEnv * env, jclass cls, jstring jcmd)
{
	const char *cmd  = (*env)->GetStringUTFChars(env, jcmd, 0);
	
	if ( cmd ) {
		//LOGD("Sending Command %s", cmd);
		Cbuf_AddText (va ("%s\n", cmd));
	}
} 


/*
 * Class:     quake_jni_Natives
 * Method:    SysQuit
 * Signature: ()I
 */
extern void Sys_Quit (void);

JNIEXPORT jint JNICALL Java_quake_jni_Natives_SysQuit
  (JNIEnv * env, jclass cls)
{
	LOGD("Calling Sys_Quit");
	Sys_Quit ();
}

/**
 * Get java array length
 */
const int getArrayLen(JNIEnv * env, jobjectArray jarray)
{
	return (*env)->GetArrayLength(env, jarray);
}


/***********************************************************
 * Callbacks - Send stuff back to Java
 ***********************************************************/


/**
 * Fires when Doom graphics are initialized.
 * params: img width, height
 */
void jni_init_video(int width, int height)
{
	JNIEnv *env;

	if ( !g_VM) {
		LOGE("jni_init_graphics No JNI VM available.\n");
		return;
	}

	(*g_VM)->AttachCurrentThread (g_VM,  &env, NULL); 

	// Natives  class loaded?
/*	
	if ( jNativesCls == 0) {
		jNativesCls = (*env)->FindClass(env, NATIVES_CLS);
	}
*/	
	// call doom.util.Natives.OnInitGraphics(w, h);
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls
			, "OnInitVideo"
			, "(II)V");

	if (mid) {
	    (*env)->CallStaticVoidMethod(env, jNativesCls, mid, width, height);
	}
	else {
		LOGE("No Java OnInitVideo(II)!");
	}
}

//#ifdef RENDERLOOP
/**
 * Signal GL end rendering
 */
void jni_swap_buffers()
{
	JNIEnv *env;

	if ( !g_VM) {
		//printf("jni_gl_endrendering No JVM!\n");
		return;
	}

	(*g_VM)->AttachCurrentThread (g_VM,  &env, NULL); 

	if ( jGLEndRenderMethod ) {
		// Call Java method
	    (*env)->CallStaticVoidMethod(env, jNativesCls, jGLEndRenderMethod);
	}
}

//#endif


/**
 * Send a string back to Java
 */
//jmethodID mSendStr;

static void jni_send_str( const char * methodName, const char * text) {
	JNIEnv *env;

	if ( !g_VM) {
		printf("%s", text);
		return;
	}

	(*g_VM)->AttachCurrentThread (g_VM,  &env, NULL); 


	if ( jNativesCls == 0 ) {
    		return;
	}

	// Call quake.jni.Natives.OnMessage(String)
//	if (! mSendStr ) {
		jmethodID mSendStr = (*env)->GetStaticMethodID(env, jNativesCls
			, methodName
			, "(Ljava/lang/String;)V");
//	}

	if (mSendStr) {
	    jstring jstr = (*env)->NewStringUTF(env, text);

	    (*env)->CallStaticVoidMethod(env, jNativesCls
	    		, mSendStr
	    		,  jstr );

 	    (*env)->DeleteLocalRef(env,jstr);
	}
}

/**
 * Printf into the java layer
 * does a varargs printf into a temp buffer
 * and calls jni_sebd_str
 */
void jni_printf(char *format, ...)
{
	va_list         argptr;
	static char     string[1024];

	va_start (argptr, format);
	vsprintf (string, format,argptr);
	va_end (argptr);

	LOGD("%s", string);
}


/**
 * Called when a fatal error has occurred.
 * The receiver should terminate
 */
void jni_sys_error(const char * text) {
	JNIEnv *env;

	if ( !g_VM) {
		LOGE("jni_fatal No Java VM available. Aborting\n");
		exit (0);
		return;
	}

	(*g_VM)->AttachCurrentThread (g_VM, &env, NULL); // (void **) 

	if ( !env) {
		LOGE("I_JNI: jni_fatal_error No JNI Environment available.\n");
		return;
	}

	if ( !jNativesCls ) {
    		return;
	}

	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls
		, "OnSysError"
		, "(Ljava/lang/String;)V");

	if (mid) {
	    (*env)->CallStaticVoidMethod(env, jNativesCls
	    		, mid
	    		, (*env)->NewStringUTF(env, text) );
	}
}

/**
 * Sound Painter
 * @param 
  */
extern void SNDDMA_AudioCallback(void *userdata, void *stream, int len);
extern int paint_audio (void *unused, void * stream, int len);

JNIEXPORT jint JNICALL Java_quake_jni_Natives_PaintAudio
 ( JNIEnv* env, jobject thiz, jobject buf )
{
	//extern int paint_audio (void *unused, void * stream, int len);

	void *stream;
	int len;


	stream = (*env)->GetDirectBufferAddress(env,  buf);
	len = (*env)->GetDirectBufferCapacity (env, buf);

	//LOGD("Paint_audio %p %d\n", stream, len );
	
	//SNDDMA_AudioCallback is a better implementaion
	SNDDMA_AudioCallback(NULL, stream, len );
	
	//paint_audio(NULL, stream, len );
	return 0; 
}

