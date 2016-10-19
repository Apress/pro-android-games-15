/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

#include "SDL_video.h"
#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "../../events/SDL_events_c.h"

#include "SDL_anvideo.h"
#include "SDL_anevents_c.h"
#include "SDL_anmouse_c.h"
/*
#include <dc/video.h>
#include <dc/pvr.h>
*/
#include <stdio.h>

/* Initialization/Query functions */
static int AN_VideoInit(_THIS, SDL_PixelFormat *vformat);
static SDL_Rect **AN_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags);
static SDL_Surface *AN_SetVideoMode(_THIS, SDL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int AN_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors);
static void AN_VideoQuit(_THIS);

/* Hardware surface functions */
static int AN_AllocHWSurface(_THIS, SDL_Surface *surface);
static int AN_LockHWSurface(_THIS, SDL_Surface *surface);
static void AN_UnlockHWSurface(_THIS, SDL_Surface *surface);
static void AN_FreeHWSurface(_THIS, SDL_Surface *surface);
static int AN_FlipHWSurface(_THIS, SDL_Surface *surface);

/* etc. */
static void AN_UpdateRects(_THIS, int numrects, SDL_Rect *rects);

/* OpenGL */
#if SDL_VIDEO_OPENGL
static void *AN_GL_GetProcAddress(_THIS, const char *proc);
static int AN_GL_LoadLibrary(_THIS, const char *path);
static int AN_GL_GetAttribute(_THIS, SDL_GLattr attrib, int* value);
static void AN_GL_SwapBuffers(_THIS);
#endif

/* DC driver bootstrap functions */

static int AN_Available(void)
{
	return 1;
}

static void AN_DeleteDevice(SDL_VideoDevice *device)
{
	SDL_free(device->hidden);
	SDL_free(device);
}

static SDL_VideoDevice *AN_CreateDevice(int devindex)
{
	SDL_VideoDevice *device;

	/* Initialize all variables that we clean on shutdown */
	device = (SDL_VideoDevice *)SDL_malloc(sizeof(SDL_VideoDevice));
	if ( device ) {
		SDL_memset(device, 0, (sizeof *device));
		device->hidden = (struct SDL_PrivateVideoData *)
				SDL_malloc((sizeof *device->hidden));
	}
	if ( (device == NULL) || (device->hidden == NULL) ) {
		SDL_OutOfMemory();
		if ( device ) {
			SDL_free(device);
		}
		return(0);
	}
	SDL_memset(device->hidden, 0, (sizeof *device->hidden));

	/* Set the function pointers */
	device->VideoInit = AN_VideoInit;
	device->ListModes = AN_ListModes;
	device->SetVideoMode = AN_SetVideoMode;
	device->CreateYUVOverlay = NULL;
	device->SetColors = AN_SetColors;
	device->UpdateRects = AN_UpdateRects;
	device->VideoQuit = AN_VideoQuit;
	device->AllocHWSurface = AN_AllocHWSurface;
	device->CheckHWBlit = NULL;
	device->FillHWRect = NULL;
	device->SetHWColorKey = NULL;
	device->SetHWAlpha = NULL;
	device->LockHWSurface = AN_LockHWSurface;
	device->UnlockHWSurface = AN_UnlockHWSurface;
	device->FlipHWSurface = AN_FlipHWSurface;
	device->FreeHWSurface = AN_FreeHWSurface;
#if SDL_VIDEO_OPENGL
	device->GL_LoadLibrary = AN_GL_LoadLibrary;
	device->GL_GetProcAddress = AN_GL_GetProcAddress;
	device->GL_GetAttribute = AN_GL_GetAttribute;
	device->GL_MakeCurrent = NULL;
	device->GL_SwapBuffers = AN_GL_SwapBuffers;
#endif
	device->SetCaption = NULL;
	device->SetIcon = NULL;
	device->IconifyWindow = NULL;
	device->GrabInput = NULL;
	device->GetWMInfo = NULL;
	device->InitOSKeymap = AN_InitOSKeymap;
	device->PumpEvents = AN_PumpEvents;

	device->free = AN_DeleteDevice;

	return device;
}

VideoBootStrap AN_bootstrap = {
	"dcvideo", "Dreamcast Video",
	AN_Available, AN_CreateDevice
};


int AN_VideoInit(_THIS, SDL_PixelFormat *vformat)
{
	/* Determine the screen depth (use default 16-bit depth) */
	/* we change this during the SDL_SetVideoMode implementation... */
	vformat->BitsPerPixel = 16;
	vformat->Rmask = 0x0000f800;
	vformat->Gmask = 0x000007e0;
	vformat->Bmask = 0x0000001f;

	/* We're done! */
	return(0);
}

const static SDL_Rect
	RECT_800x600 = {0,0,800,600},
	RECT_640x480 = {0,0,640,480},
	RECT_320x240 = {0,0,320,240};
const static SDL_Rect *vid_modes[] = {
	&RECT_800x600,
	&RECT_640x480,
	&RECT_320x240,
	NULL
};

SDL_Rect **AN_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags)
{
	switch(format->BitsPerPixel) {
	case 15:
	case 16:
		return &vid_modes;
	case 32:
		if (!(flags & SDL_OPENGL))
		return &vid_modes;
	default:
		return NULL;
	}
//	return (SDL_Rect **) -1;
}
/*
pvr_init_params_t params = {
        / Enable opaque and translucent polygons with size 16 
        { PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_16 },

        / Vertex buffer size 
        512*1024
};
*/

#if SDL_VIDEO_OPENGL
static int pvr_inited;
#endif

#define DM_320x240	0
#define DM_640x480	1
#define DM_800x608	2

#define PM_RGB555	3
#define PM_RGB565	4
#define PM_RGB888	5

SDL_Surface *AN_SetVideoMode(_THIS, SDL_Surface *current,
				int width, int height, int bpp, Uint32 flags)
{
	int disp_mode,pixel_mode,pitch;
	Uint32 Rmask, Gmask, Bmask;

	if (width==320 && height==240) disp_mode=DM_320x240;
	else if (width==640 && height==480) disp_mode=DM_640x480;
	else if (width==800 && height==600) disp_mode=DM_800x608;
	else {
		SDL_SetError("Couldn't find requested mode in list");
		return(NULL);
	}

	switch(bpp) {
	case 15: pixel_mode = PM_RGB555; pitch = width*2;
		/* 5-5-5 */
		Rmask = 0x00007c00;
		Gmask = 0x000003e0;
		Bmask = 0x0000001f;
		break;
	case 16: pixel_mode = PM_RGB565; pitch = width*2;
		/* 5-6-5 */
		Rmask = 0x0000f800;
		Gmask = 0x000007e0;
		Bmask = 0x0000001f;
		break;
	case 24: bpp = 32;
	case 32: pixel_mode = PM_RGB888; pitch = width*4;
		Rmask = 0x00ff0000;
		Gmask = 0x0000ff00;
		Bmask = 0x000000ff;
#if SDL_VIDEO_OPENGL
		if (!(flags & SDL_OPENGL))
#endif
		break;
	default:
		SDL_SetError("Couldn't find requested mode in list");
		return(NULL);
	}

//  if ( bpp != current->format->BitsPerPixel ) {
	if ( ! SDL_ReallocFormat(current, bpp, Rmask, Gmask, Bmask, 0) ) {
		return(NULL);
	}
//  }

	/* Set up the new mode framebuffer */
	current->flags = (SDL_FULLSCREEN|SDL_HWSURFACE);
	current->w = width;
	current->h = height;
	current->pitch = pitch;

#if SDL_VIDEO_OPENGL
	if (pvr_inited) {
		pvr_inited = 0;
		pvr_shutdown();
	}
#endif
	printf("Android set mode %s,%s uimplemeneted\n", __FILE__, __FUNCTION__);
	//vid_set_mode(disp_mode,pixel_mode);

	// TODO:
	SDL_SetError("Android video not unimplemented");
	return NULL;
	current->pixels = NULL; //vram_s;

#if SDL_VIDEO_OPENGL
	if (flags & SDL_OPENGL) {
		this->gl_config.driver_loaded = 1;
		current->flags = SDL_FULLSCREEN | SDL_OPENGL;
		current->pixels = NULL;
		pvr_inited = 1;
		pvr_init(&params);
		glKosInit();
		glKosBeginFrame();
	} else
#endif
	if (flags | SDL_DOUBLEBUF) {
		current->flags |= SDL_DOUBLEBUF;
		current->pixels = (void*)((int)current->pixels | 0x400000);
	}

	/* We're done */
	return(current);
}

/* We don't actually allow hardware surfaces other than the main one */
static int AN_AllocHWSurface(_THIS, SDL_Surface *surface)
{
	return(-1);
}
static void AN_FreeHWSurface(_THIS, SDL_Surface *surface)
{
	return;
}

/* We need to wait for vertical retrace on page flipped displays */
static int AN_LockHWSurface(_THIS, SDL_Surface *surface)
{
	return(0);
}

static void AN_UnlockHWSurface(_THIS, SDL_Surface *surface)
{
	return;
}

static int AN_FlipHWSurface(_THIS, SDL_Surface *surface)
{
	if (surface->flags & SDL_DOUBLEBUF) {
		//vid_set_start((int)surface->pixels & 0xffffff);
		printf("Android Flip Surface uimplemeneted\n");
		surface->pixels = (void*)((int)surface->pixels ^ 0x400000);
	}
	return(0);
}

static void AN_UpdateRects(_THIS, int numrects, SDL_Rect *rects)
{
	/* do nothing. */
}

static int AN_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors)
{
	/* do nothing of note. */
	return(1);
}

/* Note:  If we are terminated, this could be called in the middle of
   another SDL video routine -- notably UpdateRects.
*/
static void AN_VideoQuit(_THIS)
{
#if SDL_VIDEO_OPENGL
	if (pvr_inited) {
		pvr_inited = 0;
		pvr_shutdown();
	}
#endif
}

#if SDL_VIDEO_OPENGL

void dmyfunc(void) {}

typedef void (*funcptr)();
const static struct {
	char *name;
	funcptr addr;
} glfuncs[] = {
#define	DEF(func)	{#func,&func}
	DEF(glBegin),
	DEF(glBindTexture),
	DEF(glBlendFunc),
	DEF(glColor4f),
//	DEF(glCopyImageID),
	DEF(glDisable),
	DEF(glEnable),
	DEF(glEnd),
	DEF(glFlush),
	DEF(glGenTextures),
	DEF(glGetString),
	DEF(glLoadIdentity),
	DEF(glMatrixMode),
	DEF(glOrtho),
	DEF(glPixelStorei),
//	DEF(glPopAttrib),
//	DEF(glPopClientAttrib),
	{"glPopAttrib",&dmyfunc},
	{"glPopClientAttrib",&dmyfunc},
	DEF(glPopMatrix),
//	DEF(glPushAttrib),
//	DEF(glPushClientAttrib),
	{"glPushAttrib",&dmyfunc},
	{"glPushClientAttrib",&dmyfunc},
	DEF(glPushMatrix),
	DEF(glTexCoord2f),
	DEF(glTexEnvf),
	DEF(glTexImage2D),
	DEF(glTexParameteri),
	DEF(glTexSubImage2D),
	DEF(glVertex2i),
	DEF(glViewport),
#undef	DEF
};

static void *AN_GL_GetProcAddress(_THIS, const char *proc)
{
	void *ret;
	int i;

	ret = glKosGetProcAddress(proc);
	if (ret) return ret;

	for(i=0;i<sizeof(glfuncs)/sizeof(glfuncs[0]);i++) {
		if (SDL_strcmp(proc,glfuncs[i].name)==0) return glfuncs[i].addr;
	}

	return NULL;
}

static int AN_GL_LoadLibrary(_THIS, const char *path)
{
	this->gl_config.driver_loaded = 1;

	return 0;
}

static int AN_GL_GetAttribute(_THIS, SDL_GLattr attrib, int* value)
{
	GLenum mesa_attrib;
	int val;

	switch(attrib) {
	case SDL_GL_RED_SIZE:
		val = 5;
		break;
	case SDL_GL_GREEN_SIZE:
		val = 6;
		break;
	case SDL_GL_BLUE_SIZE:
		val = 5;
		break;
	case SDL_GL_ALPHA_SIZE:
		val = 0;
		break;
	case SDL_GL_DOUBLEBUFFER:
		val = 1;
		break;
	case SDL_GL_DEPTH_SIZE:
		val = 16; /* or 32? */
		break;
	case SDL_GL_STENCIL_SIZE:
		val = 0;
		break;
	case SDL_GL_ACCUM_RED_SIZE:
		val = 0;
		break;
	case SDL_GL_ACCUM_GREEN_SIZE:
		val = 0;
	case SDL_GL_ACCUM_BLUE_SIZE:
		val = 0;
		break;
	case SDL_GL_ACCUM_ALPHA_SIZE:
		val = 0;
		break;
	default :
		return -1;
	}
	*value = val;
	return 0;
}

static void AN_GL_SwapBuffers(_THIS)
{
	glKosFinishFrame();
	glKosBeginFrame();
}
#endif
