//#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

#define LOG_TAG "DoomGlimp"

#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#include "glimp.h"

#ifndef NANO_GL

typedef enum {
	false = 0,
	true  = 1
} boolean;

#define MAX_ARRAY_SIZE          6144

void assert(int expression, char *fmt, ...);

static GLenum _GLimp_beginmode;
static float _GLimp_texcoords[MAX_ARRAY_SIZE][2];
static float _GLimp_vertexes[MAX_ARRAY_SIZE][3];
static float _GLimp_colors[MAX_ARRAY_SIZE][4];
static GLuint _GLimp_numInputVerts, _GLimp_numOutputVerts;
static boolean _GLimp_texcoordbuffer;
static boolean _GLimp_colorbuffer;

unsigned int QGLBeginStarted = 0;

// Disable assert?
//#define NDEBUG 

extern void jni_printf(char *format, ...);

void glBegin(GLenum mode)
{
        //assert(!QGLBeginStarted);
        QGLBeginStarted = true;
        _GLimp_beginmode = mode;
        _GLimp_numInputVerts = _GLimp_numOutputVerts = 0;
        _GLimp_texcoordbuffer = false;
        _GLimp_colorbuffer = false;
}

void glEnd(void)
{
        GLenum mode = GL_TRIANGLES;

        assert(QGLBeginStarted, "glEnd: glBegin not started");
        QGLBeginStarted = false;
/*
        if (_GLimp_beginmode == IPHONE_QUADS && _GLimp_numInputVerts % 4 == 0)
        {
                assert(_GLimp_numOutputVerts < MAX_ARRAY_SIZE - 2, "Out Verts %d > %d", _GLimp_numOutputVerts, (MAX_ARRAY_SIZE -2) );
                bcopy(_GLimp_vertexes[_GLimp_numOutputVerts - 4],
                                _GLimp_vertexes[_GLimp_numOutputVerts],
                                sizeof(_GLimp_vertexes[0]));
                bcopy(_GLimp_texcoords[_GLimp_numOutputVerts - 4],
                                _GLimp_texcoords[_GLimp_numOutputVerts],
                                sizeof(_GLimp_texcoords[0]));
                bcopy(_GLimp_vertexes[_GLimp_numOutputVerts - 2],
                                _GLimp_vertexes[_GLimp_numOutputVerts + 1],
                                sizeof(_GLimp_vertexes[0]));
                bcopy(_GLimp_texcoords[_GLimp_numOutputVerts - 2],
                                _GLimp_texcoords[_GLimp_numOutputVerts + 1],
                                sizeof(_GLimp_texcoords[0]));
                _GLimp_numOutputVerts+= 2;
       	}
		else if (_GLimp_beginmode == IPHONE_POLYGON) {
			//jni_printf ("glEnd POLYGON iverts=%d , overts=%d, "
			//	, _GLimp_numInputVerts, _GLimp_numOutputVerts);
		}
*/
        if (_GLimp_texcoordbuffer)
        {
                glTexCoordPointer(2, GL_FLOAT, 0, _GLimp_texcoords); // sizeof(_GLimp_texcoords[0])
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        }
        else
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        if (_GLimp_colorbuffer)
        {
                glColorPointer(4, GL_FLOAT, 0, _GLimp_colors); // sizeof(_GLimp_colors[0])
                glEnableClientState(GL_COLOR_ARRAY);
        }
        else
                glDisableClientState(GL_COLOR_ARRAY);

        glVertexPointer(3, GL_FLOAT, 0, _GLimp_vertexes); // sizeof(_GLimp_vertexes[0])
        glEnableClientState(GL_VERTEX_ARRAY);

/*
	int i ;
	jni_printf ("glEnd iverts=%d , overts=%d, cb=%d, tcb=%d"
		, _GLimp_numInputVerts, _GLimp_numOutputVerts, _GLimp_colorbuffer, _GLimp_texcoordbuffer);

	for (i = 0 ; i < _GLimp_numOutputVerts ; i++ ) jni_printf("tc[%d]= %.2f, %.2f"
		, i, _GLimp_texcoords[i][0], _GLimp_texcoords[i][1] );
	for (i = 0 ; i < _GLimp_numOutputVerts ; i++ ) jni_printf("v[%d]=%.2f, %.2f, %.2f"
		, i, _GLimp_vertexes[i][0], _GLimp_vertexes[i][1], _GLimp_vertexes[i][2] );
	for (i = 0 ; i < _GLimp_numOutputVerts ; i++ ) jni_printf("c[%d]= %.2f, %.2f, %.2f, %.2f"
		, i, _GLimp_colors[i][0], _GLimp_colors[i][1], _GLimp_colors[i][2], _GLimp_colors[i][3] );
*/


        if (_GLimp_beginmode == IPHONE_QUADS) {
                mode = GL_TRIANGLES;
	}
        else if (_GLimp_beginmode == IPHONE_POLYGON) {
        	// mode = GL_TRIANGLE_FAN; 
		assert(0, "glEnd POLYGONS not implemented, only QUADS.");
	}
        else
                mode = _GLimp_beginmode;

        glDrawArrays(mode, 0, _GLimp_numOutputVerts);
}

void qglColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
        GLfloat v[4] = {r, g, b, a};

        glColor4fv(v);
}

void glColor4fv(GLfloat *v)
{
        if (QGLBeginStarted)
        {
                assert(_GLimp_numOutputVerts < MAX_ARRAY_SIZE, "Out Verts %d > %d", _GLimp_numOutputVerts, MAX_ARRAY_SIZE);
                bcopy(v, _GLimp_colors[_GLimp_numOutputVerts], sizeof(_GLimp_colors[0]));
                _GLimp_colorbuffer = true;
        }
        else
        {
                glColor4f(v[0], v[1], v[2], v[3]);
        }
}

void glColor3fv( const GLfloat *v )
{
        GLfloat v1[4] = {v[0], v[1], v[2], 255};

        glColor4fv(v1);
}

void glColor3ubv( const GLubyte *v )
{
        GLfloat v1[4] = {v[0], v[1], v[2], 255};

        glColor4fv(v1);
}

void glTexCoord2f(GLfloat s, GLfloat t)
{
        GLfloat v[2] = {s, t};

        glTexCoord2fv(v);
}

void glTexCoord2fv(GLfloat *v)
{
        assert(_GLimp_numOutputVerts < MAX_ARRAY_SIZE, "Out Verts %d > %d", _GLimp_numOutputVerts, MAX_ARRAY_SIZE);
        bcopy(v, _GLimp_texcoords[_GLimp_numOutputVerts], sizeof(_GLimp_texcoords[0]));
        _GLimp_texcoordbuffer = true;
}

void glVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
        GLfloat v[3] = {x, y, z};

        glVertex3fv(v);
}

void glVertex3fv(GLfloat *v)
{
        assert(_GLimp_numOutputVerts < MAX_ARRAY_SIZE, "Out Verts %d > %d", _GLimp_numOutputVerts, MAX_ARRAY_SIZE);
        bcopy(v, _GLimp_vertexes[_GLimp_numOutputVerts++], sizeof(_GLimp_vertexes[0]));
        ++_GLimp_numInputVerts;

	 
        if (_GLimp_beginmode == IPHONE_QUADS && _GLimp_numInputVerts % 4 == 0)
        {
                assert(_GLimp_numOutputVerts < MAX_ARRAY_SIZE - 2, "Out Verts < MAX_ARRAY_SIZE -2");
                bcopy(_GLimp_vertexes[_GLimp_numOutputVerts - 4],
                                _GLimp_vertexes[_GLimp_numOutputVerts],
                                sizeof(_GLimp_vertexes[0]));
                bcopy(_GLimp_texcoords[_GLimp_numOutputVerts - 4],
                                _GLimp_texcoords[_GLimp_numOutputVerts],
                                sizeof(_GLimp_texcoords[0]));
                bcopy(_GLimp_vertexes[_GLimp_numOutputVerts - 2],
                                _GLimp_vertexes[_GLimp_numOutputVerts + 1],
                                sizeof(_GLimp_vertexes[0]));
                bcopy(_GLimp_texcoords[_GLimp_numOutputVerts - 2],
                                _GLimp_texcoords[_GLimp_numOutputVerts + 1],
                                sizeof(_GLimp_texcoords[0]));
                _GLimp_numOutputVerts+= 2;
       	} 
       	else if (_GLimp_beginmode == IPHONE_POLYGON) {
                assert(0, "glVertex3fv POLYGONS not implemented, only QUADS.");
	}

}


void glDrawBuffer(GLenum mode)
{
	//jni_printf("glDrawBuffer not implemented");
}

void glReadBuffer(GLenum mode)
{
	jni_printf("glReadBuffer not implemented");
}

void assert(int expression, char *fmt, ...)
{
	va_list		argptr;
	char		text[1024];

	va_start (argptr,fmt);
	vsprintf (text,fmt,argptr);
	va_end (argptr);

	if (!expression) {
		//perror("glimp.c Assertion failed\n");
		jni_fatal_error ("glimp.c Assertion failed: %s", text);
		//abort();
	}
}
#endif // NANO_GL


#ifdef DEBUG_OPENGL_CALLS
void checkGLImp(const char* state, const char* file, int line) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
    	//I_Error("%s: error 0x%04X at %s:%d\n", state, error, file, line);
		LOGE("%s: error 0x%04X at %s:%d", state, error, file, line);
    }
//	else
//		LOGD("%s status 0x%04X", state, error);
}

#endif





