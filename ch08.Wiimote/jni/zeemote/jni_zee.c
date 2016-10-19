#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <android/log.h>

#define LOG_TAG "ZeeJni"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG,__VA_ARGS__)

// Global env ref (for callbacks)
static JavaVM *g_VM;

// callback class
jclass jCbClass;

#define CB_CLASS "game/engine/controller/Zeemote"

JNIEXPORT void JNICALL Java_game_engine_controller_Zeemote_Initialize
  (JNIEnv * env, jclass cls)
{
	// save vm
	(*env)->GetJavaVM(env, &g_VM);
	
	/*
	 * Load callback cclass
	 */
	LOGD("Loading callback class %s", CB_CLASS);
	
	jclass clazz 	= (*env)->FindClass(env, CB_CLASS);
	jCbClass	= (jclass)(*env)->NewGlobalRef(env, clazz);
	 
	if ( jCbClass == 0 ) {
		LOGE("Unable to find cb class: %s", CB_CLASS);
		//jni_send_error(100, "Unable to find cb class: %s", CB_CLASS);
	    return;
	}
}
    
/**
 * Connect. This sub block execution
 */
extern void DiscoverAndConnect () ;
 
JNIEXPORT void JNICALL Java_game_engine_controller_Zeemote_DiscoverAndConnect
  (JNIEnv * env, jclass cls)
{
	LOGD("Discover and connect");
	DiscoverAndConnect () ;
}

JNIEXPORT void JNICALL Java_game_engine_controller_Zeemote_DisconnectAll
  (JNIEnv * env, jclass cls)
{
	LOGD("Disconnect");
	Disconnect();
}

/**
 * Send a string back to Java
 */
static void zee_send_str(int rc, const char * methodName, const char * text) {
	JNIEnv *env;

	if ( !g_VM) {
		LOGE("No VM:%s", text);
		return;
	}

	(*g_VM)->AttachCurrentThread (g_VM,  &env, NULL); // (void **)

	if ( jCbClass == 0 ) {
		LOGE("No callback class. Abort!");
    	return;
	}

	jmethodID mSendStr = (*env)->GetStaticMethodID(env, jCbClass
		, methodName
		, "(ILjava/lang/String;)V");

	if (mSendStr) {
	    jstring jstr = (*env)->NewStringUTF(env, text);

	    (*env)->CallStaticVoidMethod(env, jCbClass
	    		, mSendStr
	    		, rc, jstr );

 	    (*env)->DeleteLocalRef(env,jstr);
	}
	else {
		LOGE("Unable to find method %s sig+%s", methodName, "(ILjava/lang/String;)V");
	}
}

void zee_send_event(int idx, char *format, ...)
{
	va_list         argptr;
	static char     string[1024];

	va_start (argptr, format);
	vsprintf (string, format,argptr);
	va_end (argptr);
	
	//LOGD("OnEvent: %s", string);
	zee_send_str(idx, "OnEvent", string);
}
