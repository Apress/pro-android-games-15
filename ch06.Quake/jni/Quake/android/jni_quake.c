
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <android/log.h>

#define LOG_TAG "QJNI"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG,__VA_ARGS__)

#include "quake_jni_Natives.h"
#include "jni_quake.h"

#include "quakedef.h" 

// Global env ref (for callbacks)
static JavaVM *g_VM;

jclass jNativesCls;
jmethodID jGLEndRenderMethod;

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
		
		jni_printf("Quake Main[%d]=%s", i, args[i]);
		
	    // free java string jrow
	    (*env)->ReleaseStringUTFChars(env, jrow, row);
	}

	
	/*
	 * Load the Image update class (called many times)
	 */
	LOGI("Loading quake/jni/Natives");
	
	jclass clazz 	= (*env)->FindClass(env, "quake/jni/Natives");
	jNativesCls	= (jclass)(*env)->NewGlobalRef(env, clazz);
	 
	//jNativesCls = (*env)->FindClass(env, "quake/jni/Natives");

	if ( jNativesCls == 0 ) {
		LOGE("Unable to find class: quake/jni/Natives");
	    return -1;
	}
#ifdef HAVE_RENDER_LOOP
	jGLEndRenderMethod = (*env)->GetStaticMethodID(env, jNativesCls
			, "OnSwapBuffers"
			, "()V");

	if ( jGLEndRenderMethod == 0 ) {
		jni_printf("Unable to find method quake.jni.OnSwapBuffers()");
	   	return -1;
	}
#endif	
	// Invoke Quake's main sub. This will loop forever
	// Program args come from Java
	main (clen, args);

	return 0;

}

/*
 * Class:     quake_jni_Natives
 * Method:    RenderFrame
 * Signature: ([Ljava/lang/String;)I
 */

JNIEXPORT jint JNICALL Java_quake_jni_Natives_RenderFrame
  (JNIEnv * env, jclass cls)
{
	extern void RenderFrame();
	RenderFrame();
}


/*
 * Class:     quake_jni_Natives
 * Method:    keyPress
 * Signature: (I)I
 */
extern void Key_Event (int key, qboolean down);

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

JNIEXPORT jint JNICALL Java_quake_jni_Natives_mouseMove
  (JNIEnv * env, jclass cls, jint mx, jint my)
{  
	extern int   mouse_x, mouse_y;
	
	//LOGD("Mouse Move %d, %d", mx, my);
	mouse_x = (int)mx; // - (int)(vid.width/2);;
	mouse_y = (int)my; // - (int)(vid.height/2);
}

// Send Quake command
JNIEXPORT jboolean JNICALL Java_quake_jni_Natives_sendCommand 
	(JNIEnv * env, jclass cls, jstring jcmd)
{
	const char *cmd  = (*env)->GetStringUTFChars(env, jcmd, 0);
	
	if ( cmd ) {
		//LOGD("Sending Command %s", cmd);
		Cbuf_AddText (va ("%s\n", cmd));
	}
	
	return JNI_TRUE; 
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
 * Doom Callbacks - Send stuff back to Java
 ***********************************************************/


/**
 * Fires when Doom graphics are initialized.
 * params: img width, height
 */
void jni_init_video(int width, int height)
{
	JNIEnv *env;

	if ( !g_VM) {
		printf("jni_init_graphics No JNI VM available.\n");
		return;
	}

	(*g_VM)->AttachCurrentThread (g_VM,  &env, NULL); // (void **)


	// call doom.util.Natives.OnInitGraphics(w, h);
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls
			, "OnInitVideo"
			, "(II)V");

	if (mid) {
	    (*env)->CallStaticVoidMethod(env, jNativesCls
	    		, mid
	    		, width, height);
	}
}

/**
 * Signal GL end rendering
 */
void jni_gl_endrendering()
{
	JNIEnv *env;

	if ( !g_VM) {
		return;
	}

	(*g_VM)->AttachCurrentThread (g_VM,  &env, NULL); // (void **)

	if ( jGLEndRenderMethod ) {
		// Call Java method
	    	(*env)->CallStaticVoidMethod(env, jNativesCls
	    		, jGLEndRenderMethod
			);

	}
}


/**
 * Send a string back to Java
 */
//jmethodID mSendStr;

static void jni_send_str( const char * methodName, const char * text) {
	JNIEnv *env;

	if ( !g_VM) {
		printf("%s\n", text);
		return;
	}

	(*g_VM)->AttachCurrentThread (g_VM,  &env, NULL); // (void **)


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

	//jni_send_str ("OnMessage", string);
	LOGD("%s", string);
}


/**
 * Called when a fatal error has occurred.
 * The receiver should terminate
 */
void jni_sys_error(const char * text) {
	JNIEnv *env;

	if ( !g_VM) {
		jni_printf("JNI Error: No Java VM available. Aborting\n");
		exit (1);
		return;
	}

	(*g_VM)->AttachCurrentThread (g_VM, &env, NULL); // (void **) 

	if ( !env) {
		jni_printf("JNI Error: No JNI Environment available.\n");
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


JNIEXPORT jint JNICALL Java_quake_jni_Natives_PaintAudio
 ( JNIEnv* env, jobject thiz, jobject buf )
{
	extern int paint_audio (void *unused, void * stream, int len);

	void *stream;
	int len;


	stream = (*env)->GetDirectBufferAddress(env,  buf);
	len = (*env)->GetDirectBufferCapacity (env, buf);

	//LOGD("Paint_audio %p %d\n", stream, len );

	return paint_audio ( NULL, stream, len );
}

// Tell jave engine is qitting
void jni_sys_quit()
{
	JNIEnv *env;
	
	if ( !g_VM) {
		LOGE("No JVM available");
		exit(0);
	}

	(*g_VM)->AttachCurrentThread ( g_VM, &env, NULL); 

	if (! env) {
		LOGE("JNI method ID for OnGameInitialized not found");
		exit(0);
	}
	jmethodID mid  = (*env)->GetStaticMethodID(env, jNativesCls
			, "OnSysQuit"
			, "()V");
	if ( mid) {
		(*env)->CallStaticVoidMethod(env, jNativesCls, mid /* args */);
	}
	else {
		LOGE("No JNI OnSysQuit! Calling exit.");
		exit (0);
	}
}
