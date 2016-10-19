#ifndef GLIMP_H
#define GLIMP_H

#ifdef NANO_GL
	#include <GL/gl.h>
#else
	
#include <GLES/gl.h>
#include <GLES/glext.h>

#define GL_CLAMP GL_CLAMP_TO_EDGE


typedef GLfloat GLdouble;

enum
{
        IPHONE_QUADS = 0x10000,
        IPHONE_POLYGON
};

#undef GL_QUADS
#define GL_QUADS                                IPHONE_QUADS
#undef GL_POLYGON
#define GL_POLYGON                              IPHONE_POLYGON

#define glClearDepth glClearDepthf
#define glMultMatrixd glMultMatrixf

#define glFogi glFogf
#define glOrtho glOrthof
#define glFrustum glFrustumf
#define glDepthRange glDepthRangef

void glBegin(GLenum mode);
void glEnd(void);
void glTexCoord2f(GLfloat s, GLfloat t);

#define glVertex2f(x, y)       glVertex3f(x, y, 0.0)
#define glVertex2i(x, y)       glVertex3f(x, y, 0.0)

void glVertex3f(GLfloat x, GLfloat y, GLfloat z);
void glVertex3fv(GLfloat *v);

#define glColor3f(r, g, b)     glColor4f(r, g, b, 1.0f)

void glColor3fv( const GLfloat *v );
void glColor3ubv( const GLubyte *v );

void qglColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
void glColor4fv(GLfloat *v);

void glTexCoord2fv(GLfloat *v);

// Not implemented
void glDrawBuffer(GLenum mode);
void glReadBuffer(GLenum mode);

#endif // NANO_GL

// Debugging

#define DEBUG_OPENGL_CALLS

#ifdef DEBUG_OPENGL_CALLS
void checkGLImp(const char* state, const char* file, int line);

#define GLCHECK(PROMPT)  checkGLImp(PROMPT, __FILE__, __LINE__)


#define glAlphaFunc(...) do { glAlphaFunc(__VA_ARGS__); GLCHECK("glAlphaFunc");} while(0)
#define glClearColor(...) do { glClearColor(__VA_ARGS__); GLCHECK("glClearColor");} while(0)
#define glClearDepthf(...) do { glClearDepthf(__VA_ARGS__); GLCHECK("glClearDepthf");} while(0)
#define glClipPlanef(...) do { glClipPlanef(__VA_ARGS__); GLCHECK("glClipPlanef");} while(0)
#define glColor4f(...) do { glColor4f(__VA_ARGS__); GLCHECK("glColor4f");} while(0)
#define glDepthRangef(...) do { glDepthRangef(__VA_ARGS__); GLCHECK("glDepthRangef");} while(0)
#define glFogf(...) do { glFogf(__VA_ARGS__); GLCHECK("glFogf");} while(0)
#define glFogfv(...) do { glFogfv(__VA_ARGS__); GLCHECK("glFogfv");} while(0)
#define glFrustumf(...) do { glFrustumf(__VA_ARGS__); GLCHECK("glFrustumf");} while(0)
#define glGetClipPlanef(...) do { glGetClipPlanef(__VA_ARGS__); GLCHECK("glGetClipPlanef");} while(0)
#define glGetFloatv(...) do { glGetFloatv(__VA_ARGS__); GLCHECK("glGetFloatv");} while(0)
#define glGetLightfv(...) do { glGetLightfv(__VA_ARGS__); GLCHECK("glGetLightfv");} while(0)
#define glGetMaterialfv(...) do { glGetMaterialfv(__VA_ARGS__); GLCHECK("glGetMaterialfv");} while(0)
#define glGetTexEnvfv(...) do { glGetTexEnvfv(__VA_ARGS__); GLCHECK("glGetTexEnvfv");} while(0)
#define glGetTexParameterfv(...) do { glGetTexParameterfv(__VA_ARGS__); GLCHECK("glGetTexParameterfv");} while(0)
#define glLightModelf(...) do { glLightModelf(__VA_ARGS__); GLCHECK("glLightModelf");} while(0)
#define glLightModelfv(...) do { glLightModelfv(__VA_ARGS__); GLCHECK("glLightModelfv");} while(0)
#define glLightf(...) do { glLightf(__VA_ARGS__); GLCHECK("glLightf");} while(0)
#define glLightfv(...) do { glLightfv(__VA_ARGS__); GLCHECK("glLightfv");} while(0)
#define glLineWidth(...) do { glLineWidth(__VA_ARGS__); GLCHECK("glLineWidth");} while(0)
#define glLoadMatrixf(...) do { glLoadMatrixf(__VA_ARGS__); GLCHECK("glLoadMatrixf");} while(0)
#define glMaterialf(...) do { glMaterialf(__VA_ARGS__); GLCHECK("glMaterialf");} while(0)
#define glMaterialfv(...) do { glMaterialfv(__VA_ARGS__); GLCHECK("glMaterialfv");} while(0)
#define glMultMatrixf(...) do { glMultMatrixf(__VA_ARGS__); GLCHECK("glMultMatrixf");} while(0)
#define glMultiTexCoord4f(...) do { glMultiTexCoord4f(__VA_ARGS__); GLCHECK("glMultiTexCoord4f");} while(0)
#define glNormal3f(...) do { glNormal3f(__VA_ARGS__); GLCHECK("glNormal3f");} while(0)
#define glOrthof(...) do { glOrthof(__VA_ARGS__); GLCHECK("glOrthof");} while(0)
#define glPointParameterf(...) do { glPointParameterf(__VA_ARGS__); GLCHECK("glPointParameterf");} while(0)
#define glPointParameterfv(...) do { glPointParameterfv(__VA_ARGS__); GLCHECK("glPointParameterfv");} while(0)
#define glPointSize(...) do { glPointSize(__VA_ARGS__); GLCHECK("glPointSize");} while(0)
#define glPolygonOffset(...) do { glPolygonOffset(__VA_ARGS__); GLCHECK("glPolygonOffset");} while(0)
#define glRotatef(...) do { glRotatef(__VA_ARGS__); GLCHECK("glRotatef");} while(0)
#define glScalef(...) do { glScalef(__VA_ARGS__); GLCHECK("glScalef");} while(0)
#define glTexEnvf(...) do { glTexEnvf(__VA_ARGS__); GLCHECK("glTexEnvf");} while(0)
#define glTexEnvfv(...) do { glTexEnvfv(__VA_ARGS__); GLCHECK("glTexEnvfv");} while(0)
#define glTexParameterf(...) do { glTexParameterf(__VA_ARGS__); GLCHECK("glTexParameterf");} while(0)
#define glTexParameterfv(...) do { glTexParameterfv(__VA_ARGS__); GLCHECK("glTexParameterfv");} while(0)
#define glTranslatef(...) do { glTranslatef(__VA_ARGS__); GLCHECK("glTranslatef");} while(0)
#define glActiveTexture(...) do { glActiveTexture(__VA_ARGS__); GLCHECK("glActiveTexture");} while(0)
#define glAlphaFuncx(...) do { glAlphaFuncx(__VA_ARGS__); GLCHECK("glAlphaFuncx");} while(0)
#define glBindBuffer(...) do { glBindBuffer(__VA_ARGS__); GLCHECK("glBindBuffer");} while(0)
#define glBindTexture(...) do { glBindTexture(__VA_ARGS__); GLCHECK("glBindTexture");} while(0)
#define glBlendFunc(...) do { glBlendFunc(__VA_ARGS__); GLCHECK("glBlendFunc");} while(0)
#define glBufferData(...) do { glBufferData(__VA_ARGS__); GLCHECK("glBufferData");} while(0)
#define glBufferSubData(...) do { glBufferSubData(__VA_ARGS__); GLCHECK("glBufferSubData");} while(0)
#define glClear(...) do { glClear(__VA_ARGS__); GLCHECK("glClear");} while(0)
#define glClearColorx(...) do { glClearColorx(__VA_ARGS__); GLCHECK("glClearColorx");} while(0)
#define glClearDepthx(...) do { glClearDepthx(__VA_ARGS__); GLCHECK("glClearDepthx");} while(0)
#define glClearStencil(...) do { glClearStencil(__VA_ARGS__); GLCHECK("glClearStencil");} while(0)
#define glClientActiveTexture(...) do { glClientActiveTexture(__VA_ARGS__); GLCHECK("glClientActiveTexture");} while(0)
#define glClipPlanex(...) do { glClipPlanex(__VA_ARGS__); GLCHECK("glClipPlanex");} while(0)
#define glColor4ub(...) do { glColor4ub(__VA_ARGS__); GLCHECK("glColor4ub");} while(0)
#define glColor4x(...) do { glColor4x(__VA_ARGS__); GLCHECK("glColor4x");} while(0)
#define glColorMask(...) do { glColorMask(__VA_ARGS__); GLCHECK("glColorMask");} while(0)
#define glColorPointer(...) do { glColorPointer(__VA_ARGS__); GLCHECK("glColorPointer");} while(0)
#define glCompressedTexImage2D(...) do { glCompressedTexImage2D(__VA_ARGS__); GLCHECK("glCompressedTexImage2D");} while(0)
#define glCompressedTexSubImage2D(...) do { glCompressedTexSubImage2D(__VA_ARGS__); GLCHECK("glCompressedTexSubImage2D");} while(0)
#define glCopyTexImage2D(...) do { glCopyTexImage2D(__VA_ARGS__); GLCHECK("glCopyTexImage2D");} while(0)
#define glCopyTexSubImage2D(...) do { glCopyTexSubImage2D(__VA_ARGS__); GLCHECK("glCopyTexSubImage2D");} while(0)
#define glCullFace(...) do { glCullFace(__VA_ARGS__); GLCHECK("glCullFace");} while(0)
#define glDeleteBuffers(...) do { glDeleteBuffers(__VA_ARGS__); GLCHECK("glDeleteBuffers");} while(0)
#define glDeleteTextures(...) do { glDeleteTextures(__VA_ARGS__); GLCHECK("glDeleteTextures");} while(0)
#define glDepthFunc(...) do { glDepthFunc(__VA_ARGS__); GLCHECK("glDepthFunc");} while(0)
#define glDepthMask(...) do { glDepthMask(__VA_ARGS__); GLCHECK("glDepthMask");} while(0)
#define glDepthRangex(...) do { glDepthRangex(__VA_ARGS__); GLCHECK("glDepthRangex");} while(0)
#define glDisable(...) do { glDisable(__VA_ARGS__); GLCHECK("glDisable");} while(0)
#define glDisableClientState(...) do { glDisableClientState(__VA_ARGS__); GLCHECK("glDisableClientState");} while(0)
#define glDrawArrays(...) do { glDrawArrays(__VA_ARGS__); GLCHECK("glDrawArrays");} while(0)
#define glDrawElements(...) do { glDrawElements(__VA_ARGS__); GLCHECK("glDrawElements");} while(0)
#define glEnable(...) do { glEnable(__VA_ARGS__); GLCHECK("glEnable");} while(0)
#define glEnableClientState(...) do { glEnableClientState(__VA_ARGS__); GLCHECK("glEnableClientState");} while(0)
#define glFinish(...) do { glFinish(__VA_ARGS__); GLCHECK("glFinish");} while(0)
#define glFlush(...) do { glFlush(__VA_ARGS__); GLCHECK("glFlush");} while(0)
#define glFogx(...) do { glFogx(__VA_ARGS__); GLCHECK("glFogx");} while(0)
#define glFogxv(...) do { glFogxv(__VA_ARGS__); GLCHECK("glFogxv");} while(0)
#define glFrontFace(...) do { glFrontFace(__VA_ARGS__); GLCHECK("glFrontFace");} while(0)
#define glFrustumx(...) do { glFrustumx(__VA_ARGS__); GLCHECK("glFrustumx");} while(0)
#define glGetBooleanv(...) do { glGetBooleanv(__VA_ARGS__); GLCHECK("glGetBooleanv");} while(0)
#define glGetBufferParameteriv(...) do { glGetBufferParameteriv(__VA_ARGS__); GLCHECK("glGetBufferParameteriv");} while(0)
#define glGetClipPlanex(...) do { glGetClipPlanex(__VA_ARGS__); GLCHECK("glGetClipPlanex");} while(0)
#define glGenBuffers(...) do { glGenBuffers(__VA_ARGS__); GLCHECK("glGenBuffers");} while(0)
#define glGenTextures(...) do { glGenTextures(__VA_ARGS__); GLCHECK("glGenTextures");} while(0)
// #define glGetError(...) do { glGetError(__VA_ARGS__); GLCHECK("glGetError");} while(0)
// #define glGetFixedv(...) do { glGetFixedv(__VA_ARGS__); GLCHECK("glGetFixedv");} while(0)
// #define glGetIntegerv(...) do { glGetIntegerv(__VA_ARGS__); GLCHECK("glGetIntegerv");} while(0)
// #define glGetLightxv(...) do { glGetLightxv(__VA_ARGS__); GLCHECK("glGetLightxv");} while(0)
// #define glGetMaterialxv(...) do { glGetMaterialxv(__VA_ARGS__); GLCHECK("glGetMaterialxv");} while(0)
// #define glGetPointerv(...) do { glGetPointerv(__VA_ARGS__); GLCHECK("glGetPointerv");} while(0)
// #define glGetString(...) do { glGetString(__VA_ARGS__); GLCHECK("glGetString");} while(0)
// #define glGetTexEnviv(...) do { glGetTexEnviv(__VA_ARGS__); GLCHECK("glGetTexEnviv");} while(0)
// #define glGetTexEnvxv(...) do { glGetTexEnvxv(__VA_ARGS__); GLCHECK("glGetTexEnvxv");} while(0)
// #define glGetTexParameteriv(...) do { glGetTexParameteriv(__VA_ARGS__); GLCHECK("glGetTexParameteriv");} while(0)
// #define glGetTexParameterxv(...) do { glGetTexParameterxv(__VA_ARGS__); GLCHECK("glGetTexParameterxv");} while(0)
#define glHint(...) do { glHint(__VA_ARGS__); GLCHECK("glHint");} while(0)
#define glIsBuffer(...) do { glIsBuffer(__VA_ARGS__); GLCHECK("glIsBuffer");} while(0)
#define glIsEnabled(...) do { glIsEnabled(__VA_ARGS__); GLCHECK("glIsEnabled");} while(0)
#define glIsTexture(...) do { glIsTexture(__VA_ARGS__); GLCHECK("glIsTexture");} while(0)
#define glLightModelx(...) do { glLightModelx(__VA_ARGS__); GLCHECK("glLightModelx");} while(0)
#define glLightModelxv(...) do { glLightModelxv(__VA_ARGS__); GLCHECK("glLightModelxv");} while(0)
#define glLightx(...) do { glLightx(__VA_ARGS__); GLCHECK("glLightx");} while(0)
#define glLightxv(...) do { glLightxv(__VA_ARGS__); GLCHECK("glLightxv");} while(0)
#define glLineWidthx(...) do { glLineWidthx(__VA_ARGS__); GLCHECK("glLineWidthx");} while(0)
#define glLoadIdentity(...) do { glLoadIdentity(__VA_ARGS__); GLCHECK("glLoadIdentity");} while(0)
#define glLoadMatrixx(...) do { glLoadMatrixx(__VA_ARGS__); GLCHECK("glLoadMatrixx");} while(0)
#define glLogicOp(...) do { glLogicOp(__VA_ARGS__); GLCHECK("glLogicOp");} while(0)
#define glMaterialx(...) do { glMaterialx(__VA_ARGS__); GLCHECK("glMaterialx");} while(0)
#define glMaterialxv(...) do { glMaterialxv(__VA_ARGS__); GLCHECK("glMaterialxv");} while(0)
#define glMatrixMode(...) do { glMatrixMode(__VA_ARGS__); GLCHECK("glMatrixMode");} while(0)
#define glMultMatrixx(...) do { glMultMatrixx(__VA_ARGS__); GLCHECK("glMultMatrixx");} while(0)
#define glMultiTexCoord4x(...) do { glMultiTexCoord4x(__VA_ARGS__); GLCHECK("glMultiTexCoord4x");} while(0)
#define glNormal3x(...) do { glNormal3x(__VA_ARGS__); GLCHECK("glNormal3x");} while(0)
#define glNormalPointer(...) do { glNormalPointer(__VA_ARGS__); GLCHECK("glNormalPointer");} while(0)
#define glOrthox(...) do { glOrthox(__VA_ARGS__); GLCHECK("glOrthox");} while(0)
#define glPixelStorei(...) do { glPixelStorei(__VA_ARGS__); GLCHECK("glPixelStorei");} while(0)
#define glPointParameterx(...) do { glPointParameterx(__VA_ARGS__); GLCHECK("glPointParameterx");} while(0)
#define glPointParameterxv(...) do { glPointParameterxv(__VA_ARGS__); GLCHECK("glPointParameterxv");} while(0)
#define glPointSizex(...) do { glPointSizex(__VA_ARGS__); GLCHECK("glPointSizex");} while(0)
#define glPolygonOffsetx(...) do { glPolygonOffsetx(__VA_ARGS__); GLCHECK("glPolygonOffsetx");} while(0)
#define glPopMatrix(...) do { glPopMatrix(__VA_ARGS__); GLCHECK("glPopMatrix");} while(0)
#define glPushMatrix(...) do { glPushMatrix(__VA_ARGS__); GLCHECK("glPushMatrix");} while(0)
#define glReadPixels(...) do { glReadPixels(__VA_ARGS__); GLCHECK("glReadPixels");} while(0)
#define glRotatex(...) do { glRotatex(__VA_ARGS__); GLCHECK("glRotatex");} while(0)
#define glSampleCoverage(...) do { glSampleCoverage(__VA_ARGS__); GLCHECK("glSampleCoverage");} while(0)
#define glSampleCoveragex(...) do { glSampleCoveragex(__VA_ARGS__); GLCHECK("glSampleCoveragex");} while(0)
#define glScalex(...) do { glScalex(__VA_ARGS__); GLCHECK("glScalex");} while(0)
#define glScissor(...) do { glScissor(__VA_ARGS__); GLCHECK("glScissor");} while(0)
#define glShadeModel(...) do { glShadeModel(__VA_ARGS__); GLCHECK("glShadeModel");} while(0)
#define glStencilFunc(...) do { glStencilFunc(__VA_ARGS__); GLCHECK("glStencilFunc");} while(0)
#define glStencilMask(...) do { glStencilMask(__VA_ARGS__); GLCHECK("glStencilMask");} while(0)
#define glStencilOp(...) do { glStencilOp(__VA_ARGS__); GLCHECK("glStencilOp");} while(0)
#define glTexCoordPointer(...) do { glTexCoordPointer(__VA_ARGS__); GLCHECK("glTexCoordPointer");} while(0)
#define glTexEnvi(...) do { glTexEnvi(__VA_ARGS__); GLCHECK("glTexEnvi");} while(0)
#define glTexEnvx(...) do { glTexEnvx(__VA_ARGS__); GLCHECK("glTexEnvx");} while(0)
#define glTexEnviv(...) do { glTexEnviv(__VA_ARGS__); GLCHECK("glTexEnviv");} while(0)
#define glTexEnvxv(...) do { glTexEnvxv(__VA_ARGS__); GLCHECK("glTexEnvxv");} while(0)
#define glTexImage2D(...) do { glTexImage2D(__VA_ARGS__); GLCHECK("glTexImage2D");} while(0)
#define glTexParameteri(...) do { glTexParameteri(__VA_ARGS__); GLCHECK("glTexParameteri");} while(0)
#define glTexParameterx(...) do { glTexParameterx(__VA_ARGS__); GLCHECK("glTexParameterx");} while(0)
#define glTexParameteriv(...) do { glTexParameteriv(__VA_ARGS__); GLCHECK("glTexParameteriv");} while(0)
#define glTexParameterxv(...) do { glTexParameterxv(__VA_ARGS__); GLCHECK("glTexParameterxv");} while(0)
#define glTexSubImage2D(...) do { glTexSubImage2D(__VA_ARGS__); GLCHECK("glTexSubImage2D");} while(0)
#define glTranslatex(...) do { glTranslatex(__VA_ARGS__); GLCHECK("glTranslatex");} while(0)
#define glVertexPointer(...) do { glVertexPointer(__VA_ARGS__); GLCHECK("glVertexPointer");} while(0)
#define glViewport(...) do { glViewport(__VA_ARGS__); GLCHECK("glViewport");} while(0)



#else
#define GLCHECK(PROMPT) do {} while(0)
#endif


#endif // IPHONE_GLIMP_H

