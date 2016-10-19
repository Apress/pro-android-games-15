//#include <gl/gl.h>

#include <float.h>
#include "../ref_gl/gl_local.h"
#include "glw_linux.h"

void ( * qglColorTableEXT)( int, int, int, int, int, const void * ) = 0;
void ( * qglActiveTextureARB) ( GLenum ) = 0;
void ( * qglSelectTextureSGIS)( GLenum ) = 0;
void ( * qglLockArraysEXT)( int, int) = 0;
void ( * qglUnlockArraysEXT) ( void ) = 0;
void ( * qglPointParameterfEXT)( GLenum param, GLfloat value ) = 0;
void ( * qglPointParameterfvEXT)( GLenum param, const GLfloat *value ) = 0;
void ( * qglMTexCoord2fSGIS)( GLenum, GLfloat, GLfloat ) = 0;
void ( * qglClientActiveTextureARB) ( GLenum ) = 0;


qboolean QGL_Init( const char *dllname )
{
	ri.Con_Printf(PRINT_ALL,  "QGL_Init:\n");
	gl_config.allow_cds = true;
    return true;
}

void QGL_Shutdown( void )
{
    qglColorTableEXT = 0;
    qglActiveTextureARB = 0;
    qglSelectTextureSGIS = 0;
    qglLockArraysEXT = 0;
    qglUnlockArraysEXT = 0;
    qglPointParameterfEXT = 0;
    qglPointParameterfvEXT = 0; 
    qglMTexCoord2fSGIS = 0;
    qglClientActiveTextureARB = 0;
}

void GLimp_EnableLogging( qboolean enable )
{
}

void GLimp_LogNewFrame( void )
{
	fprintf( glw_state.log_fp, "*** R_BeginFrame ***\n" );
}
