/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#include "../ref_soft/r_local.h"
#include "../linux/rw_linux.h"

#include <android/log.h>

#define LOG_TAG "Q2Soft"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG,__VA_ARGS__)

#include "jni_render.h"
#include "include/jni_quake.h"

static SDL_Surface* sdl_screen;


/*
** SWimp_InitGraphics
**
** This initializes the software refresh's implementation specific
** graphics subsystem.  In the case of Windows it creates DIB or
** DDRAW surfaces.
**
** The necessary width and height parameters are grabbed from
** vid.width and vid.height.
*/
static qboolean SWimp_InitGraphics( qboolean fullscreen )
{
	// free resources in use
	SWimp_Shutdown ();

	// let the sound and input subsystems know about the new window
	ri.Vid_NewWindow (vid.width, vid.height);

	// Call Java 
	//jni_init_video(vid.width, vid.height);

	sdl_screen = JNI_SurfaceNew (vid.width, vid.height, 8, SDL_SWSURFACE);
	
	vid.rowbytes = vid.width;
	vid.buffer = (byte *) sdl_screen->pixels;
	
	jni_printf("Video: width=%d height=%d", vid.width, vid.height);
	jni_printf("Video: buffer=%p size=%d", vid.buffer, sizeof(vid.buffer) );
	
	return true;
}

void SWimp_BeginFrame( float camera_separation )
{
}

void		SWimp_EndFrame (void)
{
	JNI_Flip(sdl_screen);
}

/*
** SWimp_Init
**
** This routine is responsible for initializing the implementation
** specific stuff in a software rendering subsystem.
*/
int	SWimp_Init( void *hInstance, void *wndProc )
{
	//jni_printf("SWimp_Init");
}

/*
** SWimp_SetPalette
**
** System specific palette setting routine.  A NULL palette means
** to use the existing palette.  The palette is expected to be in
** a padded 4-byte xRGB format.
*/
void		SWimp_SetPalette( const unsigned char *palette)
{
	int i;
	SDL_Color cmap[256];

	//jni_printf("SWimp_SetPalette pal=%p", palette);
	
	//if (!vid_initialized)
	//	return;
	
    if ( !palette )
        palette = ( const unsigned char * ) sw_state.currentpalette;
	
	//Check_Gamma(palette);
	
	for (i=0 ; i<256 ; i++)
	{ 
		cmap[i].r = palette[i*4] * 257;
		cmap[i].g = palette[i*4+1] * 257;
		cmap[i].b = palette[i*4+2] * 257;
	}
	SDL_SetColors (sdl_screen, cmap, 0, 256);
}

void		SWimp_Shutdown( void )
{
	//jni_printf("SWimp_Shutdown");
}

/*
** SWimp_SetMode
*/
rserr_t		SWimp_SetMode( int *pwidth, int *pheight, int mode, qboolean fullscreen )
{
	jni_printf("SWimp_SetMode pwidth=%p pheight=%p mode=%d", pwidth, pheight, mode);
	rserr_t retval = rserr_ok;

	ri.Con_Printf (PRINT_ALL, "setting mode %d:", mode );

	if ( !ri.Vid_GetModeInfo( pwidth, pheight, mode ) )
	{
		ri.Con_Printf( PRINT_ALL, " invalid mode\n" );
		return rserr_invalid_mode;
	}

	ri.Con_Printf( PRINT_ALL, " %d %d\n", *pwidth, *pheight);

	if ( !SWimp_InitGraphics( false ) ) {
		// failed to set a valid mode in windowed mode
		return rserr_invalid_mode;
	}

	R_GammaCorrectAndSetPalette( ( const unsigned char * ) d_8to24table );

	return retval;
}

void		SWimp_AppActivate( qboolean active )
{
	jni_printf("SWimp_AppActivate");
}

/*****************************************************************************/

void RW_IN_Init(in_state_t *in_state_p)
{
}

void RW_IN_Shutdown(void)
{
	//mouse_avail = false;
}

/*
===========
IN_Commands
===========
*/
void RW_IN_Commands (void)
{
}

/*
===========
IN_Move
===========
*/
void RW_IN_Move (usercmd_t *cmd)
{
}
void RW_IN_Frame (void)
{
}

void RW_IN_Activate(qboolean active)
{
}

/*****************************************************************************/

Key_Event_fp_t Key_Event_fp;

void KBD_Init(Key_Event_fp_t fp)
{
	Key_Event_fp = fp;
}

void KBD_Update(void)
{
// get events from x server
	//HandleEvents();
}

void KBD_Close(void)
{
}

/*****************************************************************************/

