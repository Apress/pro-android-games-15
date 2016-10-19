//***************************************************
//* Missing functions
//***************************************************
#include <stdio.h>
//#include <stdint.h>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>

#include <SDL.h>

#define USE_OPENGL

#ifdef USE_OPENGL
	#include <GLES/gl.h>
#endif

#define LOG_TAG "DukeJNI"

#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define MAX_STRLEN 1024

typedef unsigned char byte;

/* Signals */
const char * signal_names[] = {
/* SIGHUP,1*/ "Hangup",
/* SIGINT,2*/ "Interrupt",
/* SIGQUIT,3*/ "Quit",
/* SIGILL,4*/ "Illegal instruction",
/* SIGTRAP,5*/ "Trap",
/* SIGABRT,6*/ "Aborted",
/* SIGBUS,7*/ "Bus error",
/* SIGFPE,8*/ "Floating point exception",
/* SIGKILL,9*/ "Killed",
/* SIGUSR1,10*/ "User signal 1",
/* SIGSEGV,11*/ "Segmentation fault",
/* SIGUSR2,12*/ "User signal 2",
/* SIGPIPE,13*/ "Broken pipe",
/* SIGALRM,14*/ "Alarm clock",
/* SIGTERM,15*/ "Terminated",
/* SIGSTKFLT,16*/ "Stack fault",
/* SIGCHLD,17*/ "Child exited",
/* SIGCONT,18*/ "Continue",
/* SIGSTOP,19*/ "Stopped (signal)",
/* SIGTSTP,20*/ "Stopped",
/* SIGTTIN,21*/ "Stopped (tty input)",
/* SIGTTOU,22*/ "Stopper (tty output)",
/* SIGURG,23*/ "Urgent I/O condition",
/* SIGXCPU,24*/ "CPU time limit exceeded",
/* SIGXFSZ,25*/ "File size limit exceeded",
/* SIGVTALRM,26*/ "Virtual timer expired",
/* SIGPROF,27*/ "Profiling timer expired",
/* SIGWINCH,28*/ "Window size changed",
/* SIGIO,29*/ "I/O possible",
/* SIGPWR,30*/ "Power failure",
/* SIGSYS,31*/ "Bad system call"
};

#ifdef USE_OPENGL
	static const float vertices[] = { 		// X, Y, Z
				-1.0f, -1.0f, 0,
			 1.0f, -1.0f, 0,
			1.0f,  1.0f, 0,
			-1.0f, 1.0f, 0
	};
	static const float coords[] = {
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f,
	 };
static const unsigned short indices[] = { 0, 1, 2, 3};
/**
 * Draw an array of pixels in the entire screen using a Quad
 */
void DrawPixels (unsigned short * pixels, int w, int h) // unsigned short for RGB565
{
	static int inited = 0;
	if ( ! inited) {
		glDisable(GL_DITHER);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
		
		glClearColor(.5f, .5f, .5f, 1);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_2D);
		inited = 1;
	}
	
	// create texture
	int mTextureID;
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	
	// tex params
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
	
	// Texture is RGB565
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5 , pixels); 
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE , pixels);  // RGB888
	
	// start draw
	glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTextureID);

	// Draw quad
	glFrontFace(GL_CCW);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glEnable(GL_TEXTURE_2D);
	glTexCoordPointer(2, GL_FLOAT, 0, coords);
	glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, indices);
	
	// cleanup
	glDeleteTextures(1,  &mTextureID);

}
#endif

/**********************************************************
 * XImage Constructor
 **********************************************************/
SDL_Surface * JNI_SurfaceNew(int width, int height, int bpp, int flags) 
{
	Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0xff0000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif
	
	//screen = JNI_SurfaceNew ( w, h, 8, sdl_video_flags);
	return SDL_CreateRGBSurface (SDL_SWSURFACE, width, height, bpp, rmask, gmask, bmask, amask);
}


void JNI_UpdateRect(SDL_Surface *surface) //, SDL_Color *sdlp) //, int x, int y, int w, int h) 
{
	int i;
	int size = surface->w * surface->h;
	
#ifdef USE_OPENGL
	unsigned short pixels [size]; // RGB565
	//unsigned char pixels [size * 3]; // RGB888
#else	
	int pixels [size];
#endif

	SDL_Color * sdlp = surface->format->palette->colors;
	
	//LOGD("JNI_UpdateRect surface=%p, pixels=%p, w=%d, h=%d", surface, surface->pixels, surface->w, surface->h);
	
	for ( i = 0 ; i < size ; i++ ) {
		unsigned char Byte =  ((unsigned char *)surface->pixels)[i];
		
#ifdef USE_OPENGL
		pixels[i] =  ( (sdlp[Byte].r >> 3) << 11) | ( (sdlp[Byte].g >> 2) << 5) | (sdlp[Byte].b >> 3);   // RGB565
		/* RGB888 */
		/*pixels[i * 3] = sdlp[Byte].r ;
		pixels[i * 3 + 1] = sdlp[Byte].g;
		pixels[i * 3 + 2] = sdlp[Byte].b; */
	
#else		
		pixels[i] = (0xFF << 24)  	// ARGB888
			| (sdlp[Byte].r << 16)
			| (sdlp[Byte].g << 8)
			| (sdlp[Byte].b); 
#endif			
	}

#ifdef USE_OPENGL
	//LOGD("DrawPixels Surface: %p (%d %d)", pixels, surface->w, surface->h);
	DrawPixels(pixels, surface->w, surface->h);
		
	jni_swap_buffers ();
#else

	//send Pixels thru JNI
	jni_send_pixels(pixels); //, w, h);
#endif
}

void JNI_Flip(unsigned char *surface, int w, int h, SDL_Color * sdlp) //, int x, int y, int w, int h) 
{
	int i;
	int size = w * h;
	
	unsigned short pixels [size]; // RGB565
	//unsigned char pixels [size * 3]; // RGB888

	//SDL_Color * sdlp = surface->format->palette->colors;
	
	//LOGD("JNI_UpdateRect surface=%p, pixels=%p, w=%d, h=%d", surface, surface->pixels, surface->w, surface->h);
	
	for ( i = 0 ; i < size ; i++ ) {
		unsigned char pixel =  surface[i];
		
		pixels[i] =  ( (sdlp[pixel].r >> 3) << 11) | ( (sdlp[pixel].g >> 2) << 5) | (sdlp[pixel].b >> 3);   // RGB565
		/* RGB888 */
		/*pixels[i * 3] = sdlp[pixel].r ;
		pixels[i * 3 + 1] = sdlp[Byte].g;
		pixels[i * 3 + 2] = sdlp[Byte].b; */
	
	}

	//LOGD("DrawPixels Surface: %p (%d %d)", pixels, surface->w, surface->h);
	DrawPixels(pixels, w, h);
		
	jni_swap_buffers ();
}


/**
 * Debug
 */
void SysError (char *error, ...)
{
    va_list	argptr;
    static char  string[MAX_STRLEN];

    // Message first.
    va_start (argptr,error);
    vsprintf (string, error ,argptr);
    va_end (argptr);
	
    // Send the error back to JNI layer
    jni_sys_error(string);
	
}

static void signal_handler(int sig)
{
	SysError("Received signal %d (%s), exiting.", sig, signal_names[sig - 1]);
}

void InitSig(void)
{
	sysv_signal(SIGHUP, signal_handler);
	sysv_signal(SIGINT, signal_handler);
	sysv_signal(SIGQUIT, signal_handler);
	sysv_signal(SIGILL, signal_handler);
	sysv_signal(SIGTRAP, signal_handler);
	sysv_signal(SIGIOT, signal_handler);
	sysv_signal(SIGBUS, signal_handler);
	sysv_signal(SIGFPE, signal_handler);
	sysv_signal(SIGSEGV, signal_handler);
	sysv_signal(SIGTERM, signal_handler);
}

void assert(int condition) 
{ 
	if (!condition) {
		I_Error("Assertion failed: %d (%s, %s:%d)\n" 
			, condition ,__FUNCTION__,__FILE__,__LINE__); 
	}
}

