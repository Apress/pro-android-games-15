#include <stdio.h>

#include "doom_jni_Natives.h"
#include "jni_doom.h"

#include "doomdef.h"
#include "d_event.h"

#define LOG_TAG "DJNI"

#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)


// Global env ref (for callbacks)

//static JNIEnv *g_jniEnv = 0;
static JavaVM *g_VM;

/**
 * Used for image update
 */
jclass jNativesCls;
jmethodID jSendImageMethod;
jmethodID jStartSoundMethod;

#define GL_DOOM

#ifdef GL_DOOM
jmethodID jGLEndRenderMethod;
#endif 

// Java image pixels: int ARGB
//jintArray jImage;
//int iSize;

extern int main(int argc, char **argv);

/*
 * Class:     doom_util_Natives
 * Method:    DoomMain
 * Signature: ([Ljava/lang/String;)V
 */
JNIEXPORT jint JNICALL Java_game_studio3d_ogl_Natives_GameMain
  (JNIEnv * env, jclass class, jobjectArray jargv)
{
	// obtain a global ref to the caller jclass
	//g_jniEnv = env;

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

	    jni_printf("Main argv[%d]=%s", i, args[i]);

	    // free java string jrow
	    (*env)->ReleaseStringUTFChars(env, jrow, row);
	}

	/*
	 * Load the Image update class (called many times)
	 */
	jclass clazz 	= (*env)->FindClass(env, CB_CLASS);
	jNativesCls	= (jclass)(*env)->NewGlobalRef(env, clazz);
	 
	//jNativesCls = (*env)->FindClass(env, CB_CLASS);
	if ( jNativesCls == 0 ) {
		LOGE("Unable to find class: %s", CB_CLASS);
	    return -1;
	}

	// Load doom.util.Natives.OnImageUpdate(char[])
/*	
	jSendImageMethod = (*env)->GetStaticMethodID(env, jNativesCls
			, CB_CLASS_IU_CB
			, CB_CLASS_IU_SIG);

	if ( jSendImageMethod == 0 ) {
		jni_printf("Unable to find method OnImageUpdate(char[]): %s", CB_CLASS);
	    return -1;
	}
*/
#ifdef GL_DOOM
	// glFinish (swap buffers)
	jGLEndRenderMethod = (*env)->GetStaticMethodID(env, jNativesCls
			, "OnSwapBuffers"
			, "()V");
#endif
/*
	// Load OnStartSound(String name, int vol)
	jStartSoundMethod = (*env)->GetStaticMethodID(env, jNativesCls
			, CB_CLASS_SS_CB
			, CB_CLASS_SS_SIG);

	if ( jStartSoundMethod == 0 ) {
		jni_printf("Unable to find method OnStartSound sig: %s ", CB_CLASS_SS_SIG);
	    return -1;
	}
*/
	// Invoke Doom's main sub. This will loop forever
	// Program args come from Java
	main (clen, args);

	return 0;
}

/*
 * Class:     doom_util_Natives
 * Method:    keyEvent
 * Signature: (II)V
 */
extern void D_PostEvent (event_t* ev);

JNIEXPORT jint JNICALL Java_game_studio3d_ogl_Natives_keyEvent
  (JNIEnv * env, jclass cls, jint type, jint key)
{
	event_t event;
	event.type = (int)type;
	event.data1 = (int)key;
	D_PostEvent(&event);

	return type + key;
}

/*
 * Class:     doom_util_Natives
 * Method:    motionEvent
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_game_studio3d_ogl_Natives_motionEvent
  (JNIEnv * env, jclass cls, jint x, jint y, jint z)
{
	event_t event;
	event.type = ev_mouse;
	event.data1 = x;
	event.data2 = y;
	event.data3 = z;
	D_PostEvent(&event);
	return 0;
}

#include "SDL.h"

//extern void I_UpdateSound(void *unused, Uint8 *stream, int len);
extern void audio_loop (void *unused, Uint8 *stream, int len);

#define BUF_SIZE 8192
static short stream[BUF_SIZE];

JNIEXPORT jint JNICALL Java_game_studio3d_ogl_Natives_PaintAudio
( JNIEnv* env, jobject thiz, jobject buf )
// ( JNIEnv* env, jobject thiz, jshortArray buf )
//jint PaintAudio ( JNIEnv* env, jobject thiz, jobject buf )
{

	void *stream;
	int len;

	stream = (*env)->GetDirectBufferAddress(env,  buf);
	len = (*env)->GetDirectBufferCapacity (env, buf);

	//LOGD("Paint_audio %p %d\n", stream, len );

	//I_UpdateSound ( NULL, stream, len ); 
	audio_loop ( NULL, stream, len ); 

	/*
	LOGD("Paint_audio %p %d", stream, BUF_SIZE );
	I_UpdateSound ( NULL, (Uint8 *)stream, BUF_SIZE ); 
	
	(*env)->SetShortArrayRegion(env, buf, 0, BUF_SIZE, (jshort *) stream); */
}

/*
 * Class:     doom_util_Natives
 * Method:    setVideoMode
 * Signature: (II)I
 */
/* 
extern void I_UpdateVideoMode(void);

JNIEXPORT jint JNICALL Java_game_studio3d_ogl_Natives_setVideoMode
  (JNIEnv * env, jclass cls, jint w, jint h)
{
	extern int SCREENWIDTH, SCREENHEIGHT, desired_fullscreen;

	SCREENWIDTH = (int)w;
	SCREENHEIGHT = (int)h;
	desired_fullscreen = 0;

	I_UpdateVideoMode();
}
*/

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
void jni_init_graphics(int width, int height)
{
	JNIEnv *env;

	if ( !g_VM) {
		ERROR0("I_JNI: jni_init_graphics No JNI VM available.\n");
		return;
	}

	(*g_VM)->AttachCurrentThread (g_VM,  &env, NULL); // (void **)


	//iSize = width * height;
/*
	if ( jImage ) {
		jni_printf("Resolution changed %dx%d (Ignoring Java OnInitGraphics)", width, height);
		// TODO (*env)->DeleteLocalRef(env, jImage);
		return;
	}
*/

	// Create a new int[] used by jni_send_pixels
	//jImage = (*env)-> NewIntArray(env, iSize);

	// call doom.util.Natives.OnInitGraphics(w, h);
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls
			, CB_CLASS_IG_CB
			, CB_CLASS_IG_SIG);

	if (mid) {
	    (*env)->CallStaticVoidMethod(env, jNativesCls
	    		, mid
	    		, width, height);
	}
	else {
		LOGE("Unable to find callback %s (%d)",CB_CLASS_IG_CB, CB_CLASS_IG_SIG);
	}
	//(*g_VM)->DetachCurrentThread (g_VM);
}

/**
 * Image update Java callback. Gets called many times per sec.
 * It must not lookup JNI classes/methods or create any objects, otherwise
 * the local JNI ref table Will overflow & the app will crash
 */
/* 
void jni_send_pixels(int * data)
{
	JNIEnv *env;

	if ( !g_VM) {
		return;
	}

	(*g_VM)->AttachCurrentThread (g_VM, &env, NULL); // (void **) 

	// Send img back to java.
	if (jSendImageMethod) {
		(*env)->SetIntArrayRegion(env, jImage, 0, iSize, (jint *) data);

		// Call Java method
	    (*env)->CallStaticVoidMethod(env, jNativesCls
	    		, jSendImageMethod
	    		, jImage);
	}

}
*/

/**
 * Send a string back to Java
 */
jmethodID mSendStr;

static void jni_send_str( const char * text, int level) {
	JNIEnv *env;

	if ( !g_VM) {
		printf("%s", text);
		return;
	}

	(*g_VM)->AttachCurrentThread (g_VM,  &env, NULL); // (void **)


	if ( !jNativesCls ) {
		jNativesCls = (*env)->FindClass(env, CB_CLASS);

		if ( jNativesCls == 0 ) {
	    		ERROR1("Unable to find class: %s", CB_CLASS);
	    		return;
		}
	}

	// Call doom.jni.Natives.OnMessage(String)
	if (! mSendStr ) {
		mSendStr = (*env)->GetStaticMethodID(env, jNativesCls
			, CB_CLASS_MSG_CB
			, CB_CLASS_MSG_SIG);
	}
	if (mSendStr) {
	    (*env)->CallStaticVoidMethod(env, jNativesCls
	    		, mSendStr
	    		, (*env)->NewStringUTF(env, text)
			, (jint) level );
	}
	else {
	    ERROR2("Unable to find method: %s, signature: %s\n"
	    		, CB_CLASS_MSG_CB, CB_CLASS_MSG_SIG );
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
	static char             string[1024];

	va_start (argptr, format);
	vsprintf (string, format,argptr);
	va_end (argptr);

	//jni_send_str (string, 0);
	LOGD(string);
}

void jni_message(int level, char *format, ...)
{
	va_list         argptr;
	static char             string[1024];

	va_start (argptr, format);
	vsprintf (string, format,argptr);
	va_end (argptr);

	jni_send_str (string, level);
}


/**
 * Called when a fatal error has occurred.
 * The receiver should terminate
 */
void jni_sys_error(const char * text) {
	JNIEnv *env;

	LOGE("Sys Error: %s", text);
	
	if ( !g_VM) {
		LOGE("JNI FATAL:%s\n", text);
		exit(-1);
	}

	(*g_VM)->AttachCurrentThread (g_VM,  &env, NULL); // (void **)

	if ( !env) {
		LOGE("JNI FATAL: Unable to attach to cuur thread: %s.", text);
		exit(-1);
	}

	if ( !jNativesCls ) {
		jNativesCls = (*env)->FindClass(env, CB_CLASS);

		if ( jNativesCls == 0 ) {
	    	LOGE("JNI FATAL: Unable to find class: %s", CB_CLASS);
	    	exit(-1);
		}
	}
	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls
		, CB_CLASS_FATAL_CB
		, CB_CLASS_FATAL_SIG);

	if (mid) {
	    (*env)->CallStaticVoidMethod(env, jNativesCls
	    		, mid
	    		, (*env)->NewStringUTF(env, text) );
	}
	else {
	    LOGE("JNI FATAL: Unable to find method: %s, signature: %s\n"
	    		, CB_CLASS_FATAL_CB, CB_CLASS_FATAL_SIG );
	    exit (-1);
	}
}


/**
 * Fires multiple times when a sound is played
 * @param name Sound name
 * @param volume
 */
/* 
void jni_start_sound (const char * name, int vol)
{
	
	 // Attach to the curr thread otherwise we get JNI WARNING:
	 // threadid=3 using env from threadid=15 which aborts the VM
	JNIEnv *env;

	if ( !g_VM) {
		//ERROR0("I_JNI: jni_start_sound No JNI Environment available.\n");
		return;
	}

	(*g_VM)->AttachCurrentThread (g_VM,  &env, NULL); // (void **)

	if ( jStartSoundMethod == 0 ) {
		jni_printf("BUG: Invalid Doom JNI method method OnStartSound %s", CB_CLASS_SS_SIG);
	    return ;
	}

	// Create a new char[] used by jni_send_pixels
	// Used to prevent JNI ref table overflows
	int iSize = strlen(name);
	jbyteArray jSound = (*env)-> NewByteArray(env, iSize);

	(*env)->SetByteArrayRegion(env, jSound, 0, iSize, (jbyte *) name);

	// Call Java method
	(*env)->CallStaticVoidMethod(env, jNativesCls
			, jStartSoundMethod
			, jSound //(*env)->NewStringUTF(env, name)
			, (jint) vol);

	(*env)->DeleteLocalRef(env,jSound);
}
*/

/**
 * Fires when a background song is requested
 */
//jmethodID jSendSoundMethod;
jclass jAudioClass;

#define AUDIO_CLASS "game/studio3d/surface/NativeAudio"

#define MUSIC_CB_START  "OnStartMusic"
#define MUSIC_CB_START_SIG  "(Ljava/lang/String;I)V"

#define MUSIC_CB_STOP  "OnStopMusic"
#define MUSIC_CB_STOP_SIG  "()V"

/**
 * Fires when a background song is requested
 */
void jni_start_music (const char * name, int loop)
{

	 // Attach to the curr thread otherwise we get JNI WARNING:
	 // threadid=3 using env from threadid=15 which aborts the VM
	 
	JNIEnv *env;

	if ( !g_VM) {
		return;
	}

	(*g_VM)->AttachCurrentThread (g_VM,  &env, NULL); // (void **)

	if ( ! jAudioClass ) { 
		LOGD("StartMusic: Loading Audio class %s", AUDIO_CLASS);
		
		jclass cls 	= (*env)->FindClass(env, AUDIO_CLASS);
		jAudioClass = (*env)->NewGlobalRef(env, cls); 
	}
	
	jmethodID mid = (*env)->GetStaticMethodID(env, jAudioClass
		, MUSIC_CB_START
		, MUSIC_CB_START_SIG);

	if (mid) {
		(*env)->CallStaticVoidMethod(env, jAudioClass
				, mid
				, (*env)->NewStringUTF(env, name)
				, (jint) loop );

	}
	else {
		LOGE("StartMusic: Unable to load Music JNI method %s %s", MUSIC_CB_START, MUSIC_CB_START_SIG);
	}
}


/**
 * Fires when a background song is stopped
 */
void jni_stop_music () //const char * name)
{
	JNIEnv *env;

	if ( !g_VM) {
		return;
	}

	(*g_VM)->AttachCurrentThread (g_VM,  &env, NULL); 

	if ( ! jAudioClass ) { 
		LOGD("StopMusic: Loading Audio class %s", AUDIO_CLASS);
		
		jclass cls 	= (*env)->FindClass(env, AUDIO_CLASS);
		jAudioClass = (*env)->NewGlobalRef(env, cls); 
	}
	
	jmethodID mid = (*env)->GetStaticMethodID(env, jAudioClass
		, MUSIC_CB_STOP
		, MUSIC_CB_STOP_SIG);

	if (mid) {
		(*env)->CallStaticVoidMethod(env, jAudioClass
				, mid
				//, (*env)->NewStringUTF(env, name)
				);

	}
	else {
		LOGE("StopMusic: Unable to load Music JNI method %s %s %s", AUDIO_CLASS, MUSIC_CB_STOP, MUSIC_CB_STOP_SIG);
	}
}

/**
 * Set bg msic vol callback
 */
/* 
void jni_set_music_volume (int vol) {
	JNIEnv *env;

	if ( !g_VM) {
		return;
	}

	(*g_VM)->AttachCurrentThread (g_VM,  &env, NULL); // (void **)

	jmethodID mid = (*env)->GetStaticMethodID(env, jNativesCls
		, CB_CLASS_SETMV_CB
		, CB_CLASS_SETMV_SIG);

	if (mid) {
		(*env)->CallStaticVoidMethod(env, jNativesCls
				, mid
				, (jint) vol
				);

	}

}
*/

#ifdef GL_DOOM
/**
 * Signal GL end rendering: Swap buffers
 */
void jni_swap_buffers()
{
	JNIEnv *env;

	if ( !g_VM) {
		return;
	}

	(*g_VM)->AttachCurrentThread (g_VM,  &env, NULL); // (void **)

	if ( jGLEndRenderMethod ) {
		// Call Java method
	    (*env)->CallStaticVoidMethod(env, jNativesCls, jGLEndRenderMethod);
	}
}
#endif


