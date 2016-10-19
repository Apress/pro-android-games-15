#ifdef BUILD_EXECUTABLE

#include <stdio.h>
#include <stdlib.h>

#include <GLES/gl.h>
#include <EGL/egl.h>

/*
#include <EGL/eglnatives.h>
typedef struct egl_native_window_t*     EGLNativeWindowType;
*/

const EGLint config16bpp[] =
{
EGL_RED_SIZE, 5,
EGL_GREEN_SIZE, 6,
EGL_BLUE_SIZE, 5,
EGL_NONE
};

EGLContext eglContext;
EGLSurface eglSurface;
EGLConfig eglConfig;
EGLDisplay eglDisplay;
EGLNativeWindowType displayWindow;

int GLES_Init(int * w, int *h)
{
	EGLint majorVersion, minorVersion;
    int numConfigs;

// Display
	eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	
	printf("GLES: Display %p\n", eglDisplay);
	
 // Window surface that covers the entire screen, from libui.
    displayWindow = android_createDisplaySurface();

	* w = displayWindow->width;
	* h = displayWindow->height;
	
    printf("GLES: Window specs: %d*%d format=%d\n", *w, *h , displayWindow->format);

	// Init	
    eglInitialize(eglDisplay, &majorVersion, &minorVersion);
    printf("GLES: Version: %d.%d\n",majorVersion,minorVersion);

	// Choose config
	if (!eglChooseConfig(eglDisplay, config16bpp, &eglConfig, 1, &numConfigs))
    {
    	printf("eglChooseConfig failed\n");
    	if (eglContext==0) printf("Error code: %x\n", eglGetError());
    }

	// create context
    eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, NULL);

    if (eglContext==0) {
		printf("Error code: %x\n", eglGetError());
		exit(-1);
	}
    printf("GLES: GL context: %x\n", eglContext);
	
	// create surface
    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, displayWindow,  NULL);
	
    if (eglSurface==0) {
		printf("Error code: %x\n", eglGetError());
		exit (-1);
	}
	printf("GLES: GL surface: %x\n", eglSurface);

	// make current
    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);

}

void GLES_SwapBuffers()
{
	eglSwapBuffers(eglDisplay, eglSurface);
}
#endif
