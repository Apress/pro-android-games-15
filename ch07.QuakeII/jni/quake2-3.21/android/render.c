//***************************************************
//* Missing functions
//***************************************************
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>

#define USE_OPENGLES
#define USE_SDL

#ifdef USE_SDL
#include <SDL.h>
// for surf scaling
#include <SDL_rotozoom.h>
#endif


#ifdef USE_OPENGLES
	#include <GLES/gl.h>
#else
	#include <GL/gl.h>
#endif

#define LOG_TAG "Render"

#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

//#include "jni_android.h"

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

static unsigned int mTextureID;

/**
 * Draw an array of pixels in the entire screen using a Quad
 */
static void DrawPixelsRGB565 (unsigned short * pixels, int w, int h) // unsigned short for RGB565
//void DrawPixelsRGB (void * pixels, int w, int h, int depth) // unsigned short for RGB565
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
	//int mTextureID;
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	
	// tex params
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
	
	/*
	if ( depth == 16 ) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5 , (unsigned short *)pixels); 
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE , (unsigned char *)pixels);  // RGB888
	} */
	
	// Texture is RGB565
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5 , pixels); 
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE , pixels);  // RGB888
	
	// start draw
	//glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

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

/**
 * Draw an array of pixels in the entire screen using a Quad
 */
// empty text size. Used to compute an x offset on draw
static int xoffset;
static int yoffset;
 
static void DrawIntoTextureRGB565 (unsigned short * pixels, int w, int h) // unsigned short for RGB565
//void DrawPixels (void * pixels, int w, int h) //, int depth) // unsigned short for RGB565
{
	//int glFormat = depth == 16 ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT_5_6_5 ;
	
	// clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	
	//jni_printf("Render glTexSubImage2D %dx%d xoff=%d", w,h, xoffset);	
	glTexSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, w, h, GL_RGB, GL_UNSIGNED_SHORT_5_6_5 , pixels); 
	
	// Draw quad
	glFrontFace(GL_CCW);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glEnable(GL_TEXTURE_2D);
	glTexCoordPointer(2, GL_FLOAT, 0, coords);
	glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_SHORT, indices);
}


static void CreateEmptyTextureRGB656 (int w, int h, int x_offset, int y_offset) 
{
	int size = w * h * 2;
	
	xoffset = x_offset;
	yoffset = y_offset;
	
	//unsigned short pixels[size];
	unsigned short * pixels = (unsigned short *)malloc(size);
	
	memset(pixels, 0, size);
	
	jni_printf("Create EmptyTexture w=%d h=%d XOffset=%d YOffset=%d", w,h, xoffset, yoffset);
	
	// Init
	glDisable(GL_DITHER);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	
	glClearColor(.5f, .5f, .5f, 1);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	
	// Create tex
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	
	// tex params
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
	
	// Texture is RGB565
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5 , pixels); 
	
	free (pixels);
}

#ifdef USE_SDL

// Surface used to scale when video is not a power of 2
static char zoom = 0;
static double zoomx = 1.0;
static double zoomy = 1.0;

/**********************************************************
 * XImage Constructor
 * The image must be a power of 2 (256x256, 512x256,...) 
 * to render full screen on the OpenGL texture. If the img
 * is not POT (320x240) it will be scaled, Above 512 imgs will fail!
 **********************************************************/
SDL_Surface * JNI_SurfaceNew(int width, int height, int bpp, int flags) 
{
	Uint32 rmask = 0, gmask = 0, bmask =0 , amask = 0;
	
	int realw = 512, realh = 256, offx = 0, offy = 0;
	
	// Image must be a power of 2 for OpenGL to scale it.
	// sizes above 512x256 will render white stuff
	// non p2 imgs 320x240 will render scaled aginst the next p2 size (512x256)
	if ( width > 512 ) {
		//jni_printf("ERROR: INVALID IMAGE SIZE WIDTH %d", width);
		jni_sys_error("ERROR: INVALID IMAGE WIDTH %d (max POT 512x512)", width);
	}
	
	// Invalid size, zoom to POT 256x256
	if ( ( width != 512 && width != 256) || ( height != 256 ) ) {
		zoom = 1;
		zoomx = realw / (float)width;
		zoomy = realh / (float)height;
		
		offx = offy = 0;
		
		LOGD("WARNING Texture of size %dx%d will be scaled to %dx%d zoomx=%.3f zoomy=%.3f"
			, width, height, realw, realh, zoomx, zoomy);
	}
	
	
	// Create the OpenGL texture used to render
	CreateEmptyTextureRGB656 (realw, realh, offx, offy);
	
	LOGD("JNI_SurfaceNew:SDL_CreateRGBSurface %dx%d bpp=%d flags=%d RGBAMask=%x,%x,%x,%x Zoom?=%d"
		, width, height, bpp, flags, rmask, gmask, bmask, amask, zoom);
	
	// This is the real surface used by the client to render the video
	return SDL_CreateRGBSurface (SDL_SWSURFACE, width, height, bpp, rmask, gmask, bmask, amask);
}


/**
 * JNI_Flip8Bit
 */
static void JNI_Flip8Bit(SDL_Surface *surface )
{
	int i;
	int size = surface->w * surface->h;
	int bpp = surface->format->BitsPerPixel;
	
	unsigned short pixels [size]; // RGB565

	SDL_Color * colors = surface->format->palette->colors;
	
	//LOGD("JNI_Flip surface=%p, pixels=%p, colors=%p, w=%d, h=%d", surface, surface->pixels, colors, surface->w, surface->h);
	
	for ( i = 0 ; i < size ; i++ ) {
		unsigned char pixel =   ((unsigned char *)surface->pixels)[i]; 
		
		pixels[i] =  ( (colors[pixel].r >> 3) << 11) 
			| ( (colors[pixel].g >> 2) << 5) 
			| (colors[pixel].b >> 3);   // RGB565
			
	}

	//LOGD("DrawPixels Surface: %p (%d %d) bpp=%d", pixels, surface->w, surface->h, bpp);
	DrawIntoTextureRGB565 (pixels, surface->w,  surface->h);
	
	//DrawPixelsRGB565(pixels,  surface->w,  surface->h); 
	
	jni_swap_buffers ();
}

/**
 * JNI_Flip16RGB
 */
static void JNI_Flip16RGB(SDL_Surface *surface )
{
	int i;
	int size = surface->w * surface->h;

	SDL_PixelFormat * fmt = surface->format;
	
	int bpp = fmt->BitsPerPixel;
	
	unsigned short pixels [size]; // RGB565

	Uint16 temp, pixel;
	Uint8 red, green, blue;
	
	//LOGD("JNI_Flip surface=%p, pixels=%p, w=%d, h=%d bpp=%d"
	//	, surface, surface->pixels, surface->w, surface->h, bpp);
	
	for ( i = 0 ; i < size ; i++) {
		pixel =  ((Uint16 *)surface->pixels)[i];
		
		// Red
		temp = pixel & fmt->Rmask;
		temp = temp >> fmt->Rshift;
		temp = temp << fmt->Rloss;
		red = (Uint8) temp;

		// green
		temp = pixel & fmt->Gmask;
		temp = temp >> fmt->Gshift;
		temp = temp << fmt->Gloss;
		green = (Uint8) temp;

		// blue
		temp = pixel & fmt->Bmask;
		temp = temp >> fmt->Bshift;
		temp = temp << fmt->Bloss;
		blue = (Uint8) temp;
		
		pixels[i] =  ((red >> 3) << 11) 
			| ((green >> 2) << 5) 
			| (blue >> 3);   // RGB565
			
	}
	
	
	//LOGD("JNI_Flip16bit DrawPixels Surface: %p (%d %d)", pixels, surface->w, surface->h);
	
	//DrawPixelsRGB565(pixels,  surface->w,  surface->h); //, bpp); 
	DrawIntoTextureRGB565 (pixels, surface->w,  surface->h);
	
	jni_swap_buffers ();
}

/**
 * JNI_Flip32RGB
 */
static void JNI_Flip32RGB(SDL_Surface *surface )
{
	int i;
	int size = surface->w * surface->h;

	SDL_PixelFormat * fmt = surface->format;
	
	int bpp = fmt->BitsPerPixel;
	
	unsigned short pixels [size]; // RGB565

	Uint32 temp, pixel;
	Uint8 red, green, blue;
	
	//LOGD("JNI_Flip surface=%p, pixels=%p, w=%d, h=%d bpp=%d"
	//	, surface, surface->pixels, surface->w, surface->h, bpp);
	
	for ( i = 0 ; i < size ; i++) {
		pixel =  ((Uint32 *)surface->pixels)[i];
		
		// Red
		temp = pixel & fmt->Rmask;
		temp = temp >> fmt->Rshift;
		temp = temp << fmt->Rloss;
		red = (Uint8) temp;

		// green
		temp = pixel & fmt->Gmask;
		temp = temp >> fmt->Gshift;
		temp = temp << fmt->Gloss;
		green = (Uint8) temp;

		// blue
		temp = pixel & fmt->Bmask;
		temp = temp >> fmt->Bshift;
		temp = temp << fmt->Bloss;
		blue = (Uint8) temp;
		
		pixels[i] =  ((red >> 3) << 11) 
			| ((green >> 2) << 5) 
			| (blue >> 3);   // RGB565
			
	}
	
	
	//LOGD("JNI_Flip16bit DrawPixels Surface: %p (%d %d)", pixels, surface->w, surface->h);
	//DrawPixelsRGB565(pixels,  surface->w,  surface->h); //, bpp); 
	DrawIntoTextureRGB565 (pixels, surface->w,  surface->h);
	
	jni_swap_buffers ();
}

/**
 * JNI_FlipByBPP
 */
static void JNI_FlipByBPP (SDL_Surface *surface)
{
	int bpp = surface->format->BitsPerPixel;
	
	switch ( bpp ) {
		case 8:
			JNI_Flip8Bit (surface);
			break;
		case 16:
			JNI_Flip16RGB (surface);
			break;
		case 32:
			JNI_Flip32RGB (surface);
			break;
		default:
			jni_printf("Invalid depth %d for surface of size %dx%d", bpp, surface->w, surface->h);
	}
}

/**
 * JNI_Flip
 */
void JNI_Flip(SDL_Surface *surface )
{	
	int bpp = surface->format->BitsPerPixel;
	
	if ( zoom ) {
		// if surface is 8bit scaled will be 8bit else surface is 32 bit RGBA!!
		SDL_Surface * sized = zoomSurface( surface, zoomx, zoomy, SMOOTHING_OFF);
		
		//jni_printf("zoomed s=%p new w=%d newh=%d bpp=%d", sized, sized->w, sized->h, sized->format->BitsPerPixel);
		
		JNI_FlipByBPP (sized);
		
		// Must clean up!
		SDL_FreeSurface(sized);
	} 
	else {
		JNI_FlipByBPP (surface);
	}
}


#endif


/**
 * Debug
 */
void SysError (const char *error, ...)
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
	//LOGE("Received signal %d (%s).", sig, signal_names[sig - 1]);
}

void TrapEvilSignals(void)
{
	LOGI("Trapping evil signals.");
	signal(SIGHUP, signal_handler);
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGILL, signal_handler);
	signal(SIGTRAP, signal_handler);
	signal(SIGIOT, signal_handler); 
	signal(SIGBUS, signal_handler);
	signal(SIGFPE, signal_handler);
	signal(SIGSEGV, signal_handler);
	signal(SIGTERM, signal_handler);
}


/**
 * Debug
 */

void assert(int condition) 
{ 
	if (!condition) {
		SysError("Assertion failed: %d (%s, %s:%d)\n" 
			, condition ,__FUNCTION__,__FILE__,__LINE__); 
	}
}
