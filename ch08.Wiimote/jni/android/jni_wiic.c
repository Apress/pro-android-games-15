#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <android/log.h>
#include "wiijni.h"

#define LOG_TAG "WII"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG,__VA_ARGS__)

#include "wiic.h"

// 1 wiimote
#define MAX_WIIMOTES	4

wiimote** wiimotes;

// # of wiimotes
static int nmotes = 0;

// action signals
int sig_exit = 0;
static int sig_toggle_rumble = 0;
static int sig_disconnect = 0;
static int sig_setled = -1;
static int sig_get_status = 0;

// Global env ref (for callbacks)
static JavaVM *g_VM;

// callback class
jclass jCbClass;

#define CB_CLASS "game/engine/controller/Wiimote"

#define printf LOGD

/* old wiimote RVL-CNT-01 New RVL-CNT-01-TR */
int g_isTR = 0;

/**
 * Initialize
 */
JNIEXPORT void JNICALL Java_game_engine_controller_Wiimote_Initialize
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
		jni_send_error(100, "Unable to find cb class: %s", CB_CLASS);
	    return;
	}
	
}

static int discover()
{
	// Find, timeout 10 secs
	jni_send_message("Finding wiimotes");
	nmotes = wiic_find(wiimotes, MAX_WIIMOTES, 10);
	
	if ( !nmotes ) {
		jni_send_error(102, "No wiimotes found.");
		return 0;
	}
	jni_send_message("Found %d wiimote(s)", nmotes);
	return 1;
}

static void process_signals()
{
	int i;
	for (i = 0; i < MAX_WIIMOTES; ++i) {
		if ( sig_disconnect) {
			wiic_disconnect(wiimotes[i]);
		}
		if ( sig_toggle_rumble ) {
			wiic_toggle_rumble(wiimotes[i]);
			sig_toggle_rumble= 0;
		}
		if ( sig_setled >= 0 ) {
			wiic_set_leds(wiimotes[i], sig_setled);
			sig_setled = -1;
		}
		if ( sig_get_status ) {
			wiic_status(wiimotes[i]);
			sig_get_status = 0;
		}
	}
}

/**
 * Connect. This sub block execution
 */
JNIEXPORT void JNICALL Java_game_engine_controller_Wiimote_DiscoverAndConnect
  (JNIEnv * env, jclass cls)
{
	int i;
	if ( ! wiimotes ) {
		LOGD("Creating wiimotes");
		wiimotes =  wiic_init(MAX_WIIMOTES);
	}
	
	if ( ! discover() )
		return;

	jni_send_message("Connecting to %d wiimote(s)", nmotes);
	int connected = wiic_connect(wiimotes, nmotes);
	
	if ( !connected) {
		jni_send_error(110, "Connect: Failed to connect to any wiimote.");
		return ;
	}
	//LOGD("Connected to %i wiimotes (of %i found).\n", connected, nmotes);
	jni_send_message("Connected to %i wiimotes (of %i found).\n", connected, nmotes);
	
	/*
	 *	Now set the LEDs and rumble for a second so it's easy
	 *	to tell which wiimotes are connected (just like the wii does).
	 */
	wiic_set_leds(wiimotes[0], WIIMOTE_LED_1);
	wiic_rumble(wiimotes[0], 1);
	usleep(1000000);
	wiic_rumble(wiimotes[0], 0);

	/* wiic_status(wiimotes[0]); */

	/* Speaker Not working
	wiic_set_speaker(wiimotes[0], 1);
	wiic_sound(wiimotes[0]); */
	
	// no motion sensing
	wiic_motion_sensing(wiimotes[0], 0);
	//wiic_motion_sensing(wiimotes[0], 1);
	//wiic_set_motion_plus(wiimotes[0],1);
	
	/*
	 *	This is the main loop
	 */
	while (!sig_exit) 
	{
		process_signals();
		
		if (wiic_poll(wiimotes, connected)) {
			/*
			 *	This happens if something happened on any wiimote.
			 *	So go through each one and check if anything happened.
			 */
			//int i = 0;
			for (i = 0; i < MAX_WIIMOTES; ++i) {
				//LOGD("Wii Event %d ", wiimotes[i]->event);
				switch (wiimotes[i]->event) {
					case WIIC_EVENT:
						// a generic event occured
						handle_event(wiimotes[i]);
						break;

					case WIIC_STATUS:
						// a status event occured
						handle_ctrl_status(wiimotes[i]);
						break;

					case WIIC_DISCONNECT:
					case WIIC_UNEXPECTED_DISCONNECT:
						// the wiimote disconnected
						handle_disconnect(wiimotes[i]);
						break;

					case WIIC_READ_DATA:
						/*
						 *	Data we requested to read was returned.
						 *	Take a look at wiimotes[i]->read_req
						 *	for the data.
						 */
						break;

					case WIIC_NUNCHUK_INSERTED:
						/*
						 *	a nunchuk was inserted
						 *	This is a good place to set any nunchuk specific
						 *	threshold values.  By default they are the same
						 *	as the wiimote.
						 */
						printf("Nunchuk inserted.\n");
						jni_send_event(i, "EVT_TYPE=EXT_INSERTED|EXT_NAME=NUNCHUK");
						break;

					case WIIC_CLASSIC_CTRL_INSERTED:
						printf("Classic controller inserted.\n");
						break;

					case WIIC_GUITAR_HERO_3_CTRL_INSERTED:
						// some expansion was inserted 
						handle_ctrl_status(wiimotes[i]);
						break;
						
					case WIIC_MOTION_PLUS_INSERTED:
						printf("Motion Plus inserted.\n");
						break;
						
					case WIIC_BALANCE_BOARD_INSERTED:
						printf("Balance Board connected.\n");
						break;
						
					case WIIC_BALANCE_BOARD_REMOVED:
						printf("Balance Board disconnected.\n");
						break;
						
					case WIIC_NUNCHUK_REMOVED:
					case WIIC_CLASSIC_CTRL_REMOVED:
					case WIIC_GUITAR_HERO_3_CTRL_REMOVED:
					case WIIC_MOTION_PLUS_REMOVED:
						// some expansion was removed 
						handle_ctrl_status(wiimotes[i]);
						printf("An expansion was removed.\n");
						break;
						
					default:
						break;
				}
			}
		}
	}
	
	//LOGD("Connect main loop ended. Cleanup");
	jni_send_error(121, "Connect main loop ended.");
	
	wiic_cleanup(wiimotes, MAX_WIIMOTES);
	wiimotes = NULL;
	nmotes = 0;
	sig_exit = 0;
	sig_disconnect = 0;
}

JNIEXPORT void JNICALL Java_game_engine_controller_Wiimote_ToggleRumble
  (JNIEnv * env, jclass cls)
{
	sig_toggle_rumble = 1;
}

JNIEXPORT void JNICALL Java_game_engine_controller_Wiimote_Status
  (JNIEnv * env, jclass cls)
{
	sig_get_status = 1;
}

JNIEXPORT void JNICALL Java_game_engine_controller_Wiimote_SetLed
  (JNIEnv * env, jclass cls, jint led)
{
	LOGD("Set LED %2X", led);
	sig_setled = led;
}

JNIEXPORT void JNICALL Java_game_engine_controller_Wiimote_Connect
  (JNIEnv * env, jclass cls, jstring jcmd)
{

}

JNIEXPORT void JNICALL Java_game_engine_controller_Wiimote_DisconnectAll
  (JNIEnv * env, jclass cls)
{
	if ( nmotes == 0 ) {
		LOGE("No wiimotes to disconnect");
		return;
	}
	sig_disconnect = 1;	// Send disconnect
	sig_exit = 1; 		// quit main loop
}

JNIEXPORT void JNICALL Java_game_engine_controller_Wiimote_SendBytes
  (JNIEnv * env, jclass cls, jbyteArray bytes) //, jint len)
{
	int i;
	jboolean isCopy;
	jbyte * buffer = (*env)->GetByteArrayElements(env, bytes, &isCopy);
	jsize  len = (*env)->GetArrayLength(env, bytes);
	
	for ( i = 0 ; i < len ; i++ ) {
		LOGD("Byte[%d]=0x%.2x", i, buffer[i]); 
	}
	if ( nmotes > 0 ) {
		wiic_io_write(wiimotes[0], buffer, len);
	}
	else {
		LOGE("No wiimotes available");
	}
	(*env)->ReleaseByteArrayElements(env, bytes, buffer, 0);
}

/**
 * Send a string back to Java
 */
static void jni_send_str(int rc, const char * methodName, const char * text) {
	JNIEnv *env;

	if ( !g_VM) {
		printf("%s\n", text);
		return;
	}

	(*g_VM)->AttachCurrentThread (g_VM,  &env, NULL); // (void **)

	if ( jCbClass == 0 ) {
		LOGE("No callback class. Abort!");
    	return;
	}

	// Call quake.jni.Natives.OnMessage(String)
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
}

/**
 * Printf into the java layer
 * does a varargs printf into a temp buffer
 * and calls jni_sebd_str
 */
void jni_send_message(char *format, ...)
{
	va_list         argptr;
	static char     string[1024];

	va_start (argptr, format);
	vsprintf (string, format,argptr);
	va_end (argptr);

	jni_send_str(0, "OnMessage", string);
}

void jni_send_event(int wiimoteNum, char *format, ...)
{
	va_list         argptr;
	static char     string[1024];

	va_start (argptr, format);
	vsprintf (string, format,argptr);
	va_end (argptr);

	jni_send_str(wiimoteNum, "OnEvent", string);
}

void jni_send_error(int code, char *format, ...)
{
	va_list         argptr;
	static char     string[1024];

	va_start (argptr, format);
	vsprintf (string, format,argptr);
	va_end (argptr);

	jni_send_str(code, "OnMessage", string);
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

void jni_err_printf(char *format, ...)
{
	va_list         argptr;
	static char     string[1024];

	va_start (argptr, format);
	vsprintf (string, format,argptr);
	va_end (argptr);

	LOGE("%s", string);
}
/*
void dump_wiimotes()
{
	int i = 0;
    for (i=0; i<nmotes; i++) {
		jni_send_message("Wiimote found: %s\n", wiimotes[i].link.r_addr);
    }
}
*/

void dump_buf(char * label, uint8_t * buf, int size) 
{
	int i ;
	char str1[128], str2[32];
	sprintf(str1 , "[%s] (%d):[0x%.2x", label, size, buf[0]);
	
	for (i = 1 ; i < size -1 ; i ++ ) {
		sprintf(str2, ",0x%.2x", buf[i]);
		strcat(str1, str2);
	}
	if ( size > 1) {
		sprintf(str2, ",0x%.2x]", buf[size -1]);
		strcat(str1, str2);
	}
	else {
		strcat(str1,"]");
	}
	LOGD("%s", str1);
}

#include <bluetooth/hci.h>

void dump_devices(int hci_sock, inquiry_info * scan_info, int size )
{
	char addr[19];
//#ifdef WITH_WIIC_DEBUG		
	int WIIMOTE_CMP_LEN = 128;
	char dev_name[WIIMOTE_CMP_LEN];
//#endif	
	int i = 0;
	
	for ( i = 0 ; i < size ; i++) {
		ba2str(&scan_info[i].bdaddr, addr);
		
//#ifdef WITH_WIIC_DEBUG	
		if (hci_read_remote_name(hci_sock, &scan_info[i].bdaddr, WIIMOTE_CMP_LEN, dev_name, 5000)) {
			jni_send_error(500, "dev_dump(): Error reading device name for: %s", addr);
			strcpy(dev_name, "");
		}
		else {
			// check 4 new wiimote TR
			if ( strstr(dev_name, "-TR") != NULL ) {
				g_isTR = 1;
			}
		}
//#endif	
		
//#ifdef WITH_WIIC_DEBUG			
		jni_send_message("Found Device %s/%s Wii Dev Class: [%x,%x,%x]/[0x4,0x25,0x00] ", dev_name, addr
/*
#else		
		jni_send_message("Found Device %s Wii Dev Class: [%x,%x,%x]/[0x4,0x25,0x00] ", addr
#endif */		
			, scan_info[i].dev_class[0], scan_info[i].dev_class[1]
			, scan_info[i].dev_class[2]); 
		
		if ( g_isTR ) 
			jni_send_message("** New Wiimote Detected (TR) %s", dev_name);
	}
}
