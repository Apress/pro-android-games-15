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
/*
** GLW_IMP.C
**
** This file contains ALL Linux specific stuff having to do with the
** OpenGL refresh.  When a port is being made the following functions
** must be implemented by the port:
**
** GLimp_EndFrame
** GLimp_Init
** GLimp_Shutdown
** GLimp_SwitchFullscreen
**
*/

#include <termios.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/vt.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "../ref_gl/gl_local.h"

#include "../client/keys.h"

#include "../linux/rw_linux.h"
#include "../linux/glw_linux.h"

glwstate_t glw_state;
/*
static Display *dpy = NULL;
static int scrnum;
static Window win;
static GLXContext ctx = NULL;

#define KEY_MASK (KeyPressMask | KeyReleaseMask)
#define MOUSE_MASK (ButtonPressMask | ButtonReleaseMask | \
		    PointerMotionMask | ButtonMotionMask )
#define X_MASK (KEY_MASK | MOUSE_MASK | VisibilityChangeMask | StructureNotifyMask )
*/

/*****************************************************************************/
/* MOUSE                                                                     */
/*****************************************************************************/

// this is inside the renderer shared lib, so these are called from vid_so

static qboolean        mouse_avail;
static int   mx, my;
static int	old_mouse_x, old_mouse_y;

static int win_x, win_y;

static cvar_t	*m_filter;
static cvar_t	*in_mouse;
static cvar_t	*in_dgamouse;

static cvar_t	*r_fakeFullscreen;

//static XF86VidModeModeInfo **vidmodes;
static int default_dotclock_vidmode;
static int num_vidmodes;
static qboolean vidmode_active = false;

static qboolean	mlooking;

static qboolean mouse_active = false;
static qboolean dgamouse = false;
static qboolean vidmode_ext = false;

// state struct passed in Init
static in_state_t	*in_state;

static cvar_t *sensitivity;
static cvar_t *lookstrafe;
static cvar_t *m_side;
static cvar_t *m_yaw;
static cvar_t *m_pitch;
static cvar_t *m_forward;
static cvar_t *freelook;

static void Force_CenterView_f (void)
{
	in_state->viewangles[PITCH] = 0;
}

static void RW_IN_MLookDown (void) 
{ 
	mlooking = true; 
}

static void RW_IN_MLookUp (void) 
{
	mlooking = false;
	in_state->IN_CenterView_fp ();
}

void RW_IN_Init(in_state_t *in_state_p)
{
	int mtype;
	int i;

	in_state = in_state_p;

	// mouse variables
	m_filter = ri.Cvar_Get ("m_filter", "0", 0);
    	in_mouse = ri.Cvar_Get ("in_mouse", "0", CVAR_ARCHIVE);
    	in_dgamouse = ri.Cvar_Get ("in_dgamouse", "0", CVAR_ARCHIVE);
	freelook = ri.Cvar_Get( "freelook", "0", 0 );
	lookstrafe = ri.Cvar_Get ("lookstrafe", "0", 0);
	sensitivity = ri.Cvar_Get ("sensitivity", "3", 0);
	m_pitch = ri.Cvar_Get ("m_pitch", "0.022", 0);
	m_yaw = ri.Cvar_Get ("m_yaw", "0.022", 0);
	m_forward = ri.Cvar_Get ("m_forward", "1", 0);
	m_side = ri.Cvar_Get ("m_side", "0.8", 0);

	ri.Cmd_AddCommand ("+mlook", RW_IN_MLookDown);
	ri.Cmd_AddCommand ("-mlook", RW_IN_MLookUp);

	ri.Cmd_AddCommand ("force_centerview", Force_CenterView_f);

	mx = my = 0.0;
	mouse_avail = false; // Vlaimir true;
}

void RW_IN_Shutdown(void)
{
	mouse_avail = false;
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
/*
	if (!mouse_avail)
		return;
   
	if (m_filter->value)
	{
		mx = (mx + old_mouse_x) * 0.5;
		my = (my + old_mouse_y) * 0.5;
	}

	old_mouse_x = mx;
	old_mouse_y = my;

	mx *= sensitivity->value;
	my *= sensitivity->value;

// add mouse X/Y movement to cmd
	if ( (*in_state->in_strafe_state & 1) || 
		(lookstrafe->value && mlooking ))
		cmd->sidemove += m_side->value * mx;
	else
		in_state->viewangles[YAW] -= m_yaw->value * mx;

	if ( (mlooking || freelook->value) && 
		!(*in_state->in_strafe_state & 1))
	{
		in_state->viewangles[PITCH] += m_pitch->value * my;
	}
	else
	{
		cmd->forwardmove -= m_forward->value * my;
	}

	mx = my = 0; */
}


static void IN_DeactivateMouse( void ) 
{
/*
	if (!mouse_avail || !dpy || !win)
		return;

	if (mouse_active) {
		uninstall_grabs();
		mouse_active = false;
	}
*/
}

static void IN_ActivateMouse( void ) 
{
/*
	if (!mouse_avail || !dpy || !win)
		return;

	if (!mouse_active) {
		mx = my = 0; // don't spazz
		install_grabs();
		mouse_active = true;
	}
*/
}


void RW_IN_Frame (void)
{
}

void RW_IN_Activate(qboolean active)
{
	if (active || vidmode_active)
		IN_ActivateMouse();
	else
		IN_DeactivateMouse ();
}

/*****************************************************************************/
/* KEYBOARD                                                                  */
/*****************************************************************************/

static void HandleEvents(void)
{
}

Key_Event_fp_t Key_Event_fp;

void KBD_Init(Key_Event_fp_t fp)
{
	Key_Event_fp = fp;
}

void KBD_Update(void)
{
// get events from x server
	HandleEvents();
}

void KBD_Close(void)
{
}

/*****************************************************************************/

static qboolean GLimp_SwitchFullscreen( int width, int height );
qboolean GLimp_InitGL (void);

static void signal_handler(int sig)
{
	//printf("Received signal %d, exiting...\n", sig);
	GLimp_Shutdown();
	exit(0);
}

static void InitSig(void)
{
	signal(SIGHUP, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGILL, signal_handler);
	signal(SIGTRAP, signal_handler);
	signal(SIGIOT, signal_handler);
	signal(SIGBUS, signal_handler);
	signal(SIGFPE, signal_handler);
	signal(SIGSEGV, signal_handler);
	signal(SIGTERM, signal_handler);
}

/*
** GLimp_SetMode
*/
int GLimp_SetMode( int *pwidth, int *pheight, int mode, qboolean fullscreen )
{
	//printf("GLimp_SetMode mode=%d, fs=%d\n", mode, fullscreen);

	int width, height;
	ri.Con_Printf( PRINT_ALL, "Initializing OpenGL display\n");

	if (fullscreen)
		ri.Con_Printf (PRINT_ALL, "...setting fullscreen mode %d:", mode );
	else
		ri.Con_Printf (PRINT_ALL, "...setting mode %d:", mode );

	if ( !ri.Vid_GetModeInfo( &width, &height, mode ) )
	{
		ri.Con_Printf( PRINT_ALL, " invalid mode\n" );
		return rserr_invalid_mode;
	}

	ri.Con_Printf( PRINT_ALL, " %d %d\n", width, height );

	// destroy the existing window
	GLimp_Shutdown ();
	
	*pwidth = width;
	*pheight = height;

	// let the sound and input subsystems know about the new window
	ri.Vid_NewWindow (width, height);
	
	return rserr_ok;
}

/*
** GLimp_Shutdown
**
** This routine does all OS specific shutdown procedures for the OpenGL
** subsystem.  Under OpenGL this means NULLing out the current DC and
** HGLRC, deleting the rendering context, and releasing the DC acquired
** for the window.  The state structure is also nulled out.
**
*/

void GLimp_Shutdown( void )
{
	
//	uninstall_grabs();
	mouse_active = false;
	dgamouse = false;
	
}

/*
** GLimp_Init
**
** This routine is responsible for initializing the OS specific portions
** of OpenGL.  
*/
static qboolean gl_initialized = false;

int GLimp_Init( void *hinstance, void *wndproc )
{
	InitSig();

	if ( ! gl_initialized ) {
		//printf("GLimp_Init: Initializing Nano GL!\n");

		//  Vladimir init nano GL 
		if ( ! nanoGL_Init() ) {
			//ri.Sys_Error(ERR_DROP, "GLimp_Init: Nano GL initialization failed\n");
			//printf ("GLimp_Init: Nano GL initialization failed\n");
			return false;
		}
		gl_initialized = true;
	}

	return true;
}

/*
** GLimp_BeginFrame
*/
void GLimp_BeginFrame( float camera_seperation )
{
}

/*
** GLimp_EndFrame
** 
** Responsible for doing a swapbuffers and possibly for other stuff
** as yet to be determined.  Probably better not to make this a GLimp
** function and instead do a call to GLimp_SwapBuffers.
*/
//extern void jni_gl_endrendering();
void GLimp_EndFrame (void)
{

	qglFlush(); // Vladimir
	//	qglXSwapBuffers(dpy, win);
}

/*
** GLimp_AppActivate
*/
void GLimp_AppActivate( qboolean active )
{
}

/*------------------------------------------------*/
/* X11 Input Stuff
/*------------------------------------------------*/

