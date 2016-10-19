/*
Copyright (C) 2007-2009 Olli Hinkka

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
#include <e32def.h>
#include <e32std.h>
*/

#include <stdio.h>
#include <stdlib.h>

#include "gl.h"
#include "glesinterface.h"

#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1

struct nanoState                 
    {                                  
    GLboolean alpha_test;              
    GLboolean blend;                   
    GLboolean clip_planei;             
    GLboolean color_logic_op;          
    GLboolean color_material;          
    GLboolean cull_face;               
    GLboolean depth_test;              
    GLboolean dither;                  
    GLboolean fog;                     
    GLboolean lighti;                  
    GLboolean lighting;                
    GLboolean line_smooth;             
    GLboolean matrix_palette_oes;      
    GLboolean multisample;             
    GLboolean normalize;               
    GLboolean point_smooth;            
    GLboolean point_sprite_oes;        
    GLboolean polygon_offset_fill;     
    GLboolean rescale_normal;          
    GLboolean sample_alpha_to_coverage;
    GLboolean sample_alpha_to_one;     
    GLboolean sample_coverage;         
    GLboolean scissor_test;            
    GLboolean stencil_test;            
    GLboolean depthmask;               
    GLclampf depth_range_near;         
    GLclampf depth_range_far;          
    GLenum depth_func;                 
    GLenum cullface;                   
    GLenum shademodel;                 
    GLenum sfactor;                    
    GLenum dfactor;                    
    GLenum matrixmode;                 
    };                                 

static struct nanoState nanoglState;

static struct nanoState nanoglInitState = 
    {                                
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_TRUE,                         
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_TRUE,                         
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_FALSE,                        
    GL_TRUE,                         
    0.0f,                            
    1.0f,                            
    GL_LESS,                         
    GL_BACK,                         
    GL_SMOOTH,                       
    GL_ONE,                          
    GL_ZERO,                         
    GL_MODELVIEW,                                        
    };                               

struct booleanstate
    {
    GLboolean value;
    GLboolean changed;
    };

struct floatstate
    {
    GLfloat value;
    GLboolean changed;
    };
    
struct uintstate
    {
    GLuint value;
    GLboolean changed;
    };
    
struct ptrstate
    {
    GLint size;
    GLenum type;
    GLsizei stride;
    GLvoid* ptr;
    GLboolean changed;
    GLboolean enabled;
    };


struct nanotmuState
    {
    struct booleanstate texture_2d;
    struct floatstate texture_env_mode;                                         
    struct uintstate boundtexture;      
    struct ptrstate vertex_array;                                       
    struct ptrstate color_array;
    struct ptrstate texture_coord_array;
    };

static struct nanotmuState tmuState0;
static struct nanotmuState tmuState1;
        
static struct nanotmuState tmuInitState = 
    {
    {GL_FALSE, GL_FALSE},           
    {GL_MODULATE,GL_FALSE},        
    {0x7fffffff,GL_FALSE},
    {4,GL_FLOAT,0, NULL, GL_FALSE, GL_FALSE},
    {4,GL_FLOAT,0, NULL, GL_FALSE, GL_FALSE},
    {4,GL_FLOAT,0, NULL, GL_FALSE, GL_FALSE},
    };   

static struct nanotmuState* activetmuState = &tmuState0;

extern "C++" GlESInterface* glEsImpl;

static GLenum wrapperPrimitiveMode = GL_QUADS;
GLboolean useTexCoordArray = GL_FALSE;
static GLenum activetmu = GL_TEXTURE0;
static GLenum clientactivetmu = GL_TEXTURE0;

#if defined(__MULTITEXTURE_SUPPORT__)
GLboolean useMultiTexCoordArray = GL_FALSE;
#endif


#if !defined (__WINS__)
//#define __FORCEINLINE __forceinline
#define __FORCEINLINE inline
#else
#define __FORCEINLINE
#endif

static GLboolean delayedttmuchange = GL_FALSE;
static GLenum delayedtmutarget = GL_TEXTURE0;

struct VertexAttrib
    {
    float x;
    float y;
    float z;
#if !defined(__MULTITEXTURE_SUPPORT__)
    float padding;
#endif
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
    
    float s;
    float t;
#if defined(__MULTITEXTURE_SUPPORT__)
    float s_multi;
    float t_multi;
#endif
    };

static VertexAttrib vertexattribs[8000];

static GLushort indexArray[30000];

static GLuint vertexCount = 0;
static GLuint indexCount = 0;
static GLuint vertexMark = 0;
static int indexbase = 0;

static VertexAttrib* ptrVertexAttribArray = NULL;
static VertexAttrib* ptrVertexAttribArrayMark = NULL;

static VertexAttrib currentVertexAttrib;
#if defined(__MULTITEXTURE_SUPPORT__)
static VertexAttrib currentVertexAttribInit = {0.0f,0.0f,0.0f,
                                    255,255,255,255,
                                    0.0f,0.0f,
                                    0.0f,0.0f
                                    };
#else
static VertexAttrib currentVertexAttribInit = {0.0f,0.0f,0.0f,0.0f,
                                    255,255,255,255,
                                    0.0f,0.0f,
                                    };
#endif
static GLushort* ptrIndexArray = NULL;

static GLboolean arraysValid = GL_FALSE;

void InitGLStructs()
    {
    ptrVertexAttribArray = vertexattribs;
    ptrVertexAttribArrayMark = ptrVertexAttribArray;
    ptrIndexArray = indexArray;
    
    memcpy(&nanoglState, &nanoglInitState, sizeof(struct nanoState));
    memcpy(&tmuState0,&tmuInitState,sizeof(struct nanotmuState));
    memcpy(&tmuState1,&tmuInitState,sizeof(struct nanotmuState));    
    memcpy(&currentVertexAttrib,&currentVertexAttribInit,sizeof(struct VertexAttrib));
    
    activetmuState = &tmuState0;
    wrapperPrimitiveMode = GL_QUADS;
    useTexCoordArray = GL_FALSE;
    activetmu = GL_TEXTURE0;
    clientactivetmu = GL_TEXTURE0;
    delayedttmuchange = GL_FALSE;
    delayedtmutarget = GL_TEXTURE0;
    vertexCount = 0;
    indexCount = 0;
    vertexMark = 0;
    indexbase = 0;
    arraysValid = GL_FALSE;
    }


void FlushOnStateChange()
    {
    if (delayedttmuchange)
        {
        delayedttmuchange = GL_FALSE;
        glEsImpl->glActiveTexture(delayedtmutarget);
        }

    if (!vertexCount)
        return;

    if (!arraysValid)
        {
        glEsImpl->glClientActiveTexture(GL_TEXTURE0);
        glEsImpl->glVertexPointer(3, GL_FLOAT, sizeof(VertexAttrib), &vertexattribs[0].x);
        glEsImpl->glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(VertexAttrib), &vertexattribs[0].red);
        glEsImpl->glTexCoordPointer(2, GL_FLOAT, sizeof(VertexAttrib), &vertexattribs[0].s);    
        glEsImpl->glEnableClientState(GL_VERTEX_ARRAY);
        glEsImpl->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEsImpl->glEnableClientState(GL_COLOR_ARRAY);
#if defined(__MULTITEXTURE_SUPPORT__)        
        glEsImpl->glClientActiveTexture(GL_TEXTURE1);
        glEsImpl->glTexCoordPointer(2, GL_FLOAT, sizeof(VertexAttrib), &vertexattribs[0].s_multi); 
        glEsImpl->glEnableClientState(GL_TEXTURE_COORD_ARRAY);  
        glEsImpl->glClientActiveTexture(GL_TEXTURE0);   
#endif
        arraysValid = GL_TRUE;
        }

    glEsImpl->glDrawElements( GL_TRIANGLES,vertexCount,GL_UNSIGNED_SHORT, indexArray );


#if defined(__MULTITEXTURE_SUPPORT__)            
    useMultiTexCoordArray = GL_FALSE;
#endif
    vertexCount = 0;
    indexCount = 0;
    ptrVertexAttribArray = vertexattribs;
    ptrVertexAttribArrayMark = ptrVertexAttribArray;
    ptrIndexArray = indexArray;
    useTexCoordArray = GL_FALSE;
    }

void glBegin(GLenum mode)
    {
    wrapperPrimitiveMode = mode;
    vertexMark = vertexCount;    
    ptrVertexAttribArrayMark = ptrVertexAttribArray;
    indexbase = indexCount;
    }
    
void glEnd(void)
    {
    vertexCount+=((unsigned char*)ptrVertexAttribArray-(unsigned char*)ptrVertexAttribArrayMark)/sizeof(VertexAttrib);
    if (vertexCount < 3)
        {
        return;
        }
    switch (wrapperPrimitiveMode)
        {
        case GL_QUADS:
            {
            *ptrIndexArray++ = indexCount;
            *ptrIndexArray++ = indexCount+1;
            *ptrIndexArray++ = indexCount+2;
            *ptrIndexArray++ = indexCount;
            *ptrIndexArray++ = indexCount+2;
            *ptrIndexArray++ = indexCount+3;
            indexCount+=4;
            vertexCount+=2;
            }
            break;
        case GL_TRIANGLES:
            {
            int  vcount = (vertexCount-vertexMark)/3;
            for (int count = 0; count < vcount; count++)
                {
                *ptrIndexArray++ = indexCount;
                *ptrIndexArray++ = indexCount+1;
                *ptrIndexArray++ = indexCount+2;            
                indexCount+=3;    
                }
            }
            break;
        case GL_TRIANGLE_STRIP:
            {
            *ptrIndexArray++ = indexCount;
            *ptrIndexArray++ = indexCount+1;
            *ptrIndexArray++ = indexCount+2;
            indexCount+=3;
            int vcount = ((vertexCount-vertexMark)-3);
            if (vcount && ((long)ptrIndexArray & 0x02))
                {
                *ptrIndexArray++ = indexCount-1; // 2 
                *ptrIndexArray++ = indexCount-2; // 1
                *ptrIndexArray++ = indexCount;   // 3
                indexCount++;
                vcount-=1;    
                int odd = vcount&1;
                vcount/=2;
                unsigned int* longptr = (unsigned int*) ptrIndexArray;

                for (int count = 0; count < vcount; count++)
                    {
                    *(longptr++) = (indexCount-2) | ((indexCount-1)<<16);                    
                    *(longptr++) = (indexCount) | ((indexCount)<<16);                    
                    *(longptr++) = (indexCount-1) | ((indexCount+1)<<16);
                    indexCount+=2;
                    }
                ptrIndexArray = (unsigned short*)(longptr);
                if (odd)
                    {
                    *ptrIndexArray++ = indexCount-2; // 2 
                    *ptrIndexArray++ = indexCount-1; // 1
                    *ptrIndexArray++ = indexCount;   // 3
                    indexCount++;    
                    }
                }
           else
                {
                //already aligned
                int odd = vcount&1;
                vcount/=2;
                unsigned int* longptr = (unsigned int*) ptrIndexArray;

                for (int count = 0; count < vcount; count++)
                    {                    
                    *(longptr++) = (indexCount-1) | ((indexCount-2)<<16);                    
                    *(longptr++) = (indexCount) | ((indexCount-1)<<16);                    
                    *(longptr++) = (indexCount) | ((indexCount+1)<<16);
                    indexCount+=2;

                    }
                ptrIndexArray = (unsigned short*)(longptr);
                if (odd)
                    {
                    
                    *ptrIndexArray++ = indexCount-1; // 2 
                    *ptrIndexArray++ = indexCount-2; // 1
                    *ptrIndexArray++ = indexCount;   // 3
                    indexCount++;    
                    }
                }            
            vertexCount+=(vertexCount-vertexMark-3)*2;
            }
            break;
        case GL_POLYGON:
        case GL_TRIANGLE_FAN:
            {
            *ptrIndexArray++ = indexCount++;
            *ptrIndexArray++ = indexCount++;
            *ptrIndexArray++ = indexCount++;
            int vcount = ((vertexCount-vertexMark)-3);
            for (int count = 0; count < vcount; count++)
                {
                *ptrIndexArray++ = indexbase;
                *ptrIndexArray++ = indexCount-1;
                *ptrIndexArray++ = indexCount++;                
                vertexCount+=2;
                }
             }   
            break;

        default:
            break;
        }
    }

void glEnable (GLenum cap)
    {
    GLboolean statechanged = GL_FALSE;
    switch(cap)
        {
        case GL_ALPHA_TEST:
            {
            if (!nanoglState.alpha_test)
                {
                nanoglState.alpha_test = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_BLEND:
            {
            if (!nanoglState.blend)
                {
                nanoglState.blend = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        //case GL_CLIP_PLANEi
        case GL_COLOR_LOGIC_OP:
            {
            if (!nanoglState.color_logic_op)
                {
                nanoglState.color_logic_op = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_COLOR_MATERIAL:
            {
            if (!nanoglState.color_material)
                {
                nanoglState.color_material = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_CULL_FACE:
            {
            if (!nanoglState.cull_face)
                {
                nanoglState.cull_face = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_DEPTH_TEST:
            {
            if (!nanoglState.depth_test)
                {
                nanoglState.depth_test = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_DITHER:
            {
            if (!nanoglState.dither)
                {
                nanoglState.dither = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_FOG:
        //case GL_LIGHTi
            {
            if (!nanoglState.fog)
                {
                nanoglState.fog = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_LIGHTING:
            {
            if (!nanoglState.lighting)
                {
                nanoglState.lighting = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_LINE_SMOOTH:
            {
            if (!nanoglState.line_smooth)
                {
                nanoglState.line_smooth = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
/*        case GL_MATRIX_PALETTE_OES:
            {
            if (!nanoglState.matrix_palette_oes)
                {
                nanoglState.matrix_palette_oes = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }*/
        case GL_MULTISAMPLE:
            {
            if (!nanoglState.multisample)
                {
                nanoglState.multisample = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_NORMALIZE:
            {
            if (!nanoglState.normalize)
                {
                nanoglState.normalize = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
/*        case GL_POINT_SPRITE_OES:
            {
            if (!nanoglState.point_sprite_oes)
                {
                nanoglState.point_sprite_oes = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }*/
        case GL_POLYGON_OFFSET_FILL:
            {
            if (!nanoglState.polygon_offset_fill)
                {
                nanoglState.polygon_offset_fill = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_RESCALE_NORMAL:
            {
            if (!nanoglState.rescale_normal)
                {
                nanoglState.rescale_normal = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_SAMPLE_ALPHA_TO_COVERAGE:
            {
            if (!nanoglState.sample_alpha_to_coverage)
                {
                nanoglState.sample_alpha_to_coverage = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_SAMPLE_ALPHA_TO_ONE:
            {
            if (!nanoglState.sample_alpha_to_one)
                {
                nanoglState.sample_alpha_to_one = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_SAMPLE_COVERAGE:
            {
            if (!nanoglState.sample_coverage)
                {
                nanoglState.sample_coverage = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_SCISSOR_TEST:
            {
            if (!nanoglState.scissor_test)
                {
                nanoglState.scissor_test = GL_TRUE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_STENCIL_TEST:
            {
            return;
/*            if (!nanoglState.stencil_test)
                {
                nanoglState.stencil_test = GL_TRUE;
                statechanged = GL_TRUE;
                }*/
            break;
            }
        case GL_TEXTURE_2D:   
            {
            if (!activetmuState->texture_2d.value)
                {
                FlushOnStateChange();
                glEsImpl->glEnable(cap);
                activetmuState->texture_2d.value = GL_TRUE;
                return;              
                }
            break;
            }
        default:
            break;
        }
        
    if (statechanged)
        {
        FlushOnStateChange();
        glEsImpl->glEnable(cap);
        }    
    }

void glDisable (GLenum cap)
    {
    GLboolean statechanged = GL_FALSE;
    switch(cap)
        {
        case GL_ALPHA_TEST:
            {
            if (nanoglState.alpha_test)
                {
                nanoglState.alpha_test = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_BLEND:
            {
            if (nanoglState.blend)
                {
                nanoglState.blend = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        //case GL_CLIP_PLANEi
        case GL_COLOR_LOGIC_OP:
            {
            if (nanoglState.color_logic_op)
                {
                nanoglState.color_logic_op = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_COLOR_MATERIAL:
            {
            if (nanoglState.color_material)
                {
                nanoglState.color_material = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_CULL_FACE:
            {
            if (nanoglState.cull_face)
                {
                nanoglState.cull_face = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_DEPTH_TEST:
            {
            if (nanoglState.depth_test)
                {
                nanoglState.depth_test = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_DITHER:
            {
            if (nanoglState.dither)
                {
                nanoglState.dither = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_FOG:
        //case GL_LIGHTi
            {
            if (nanoglState.fog)
                {
                nanoglState.fog = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_LIGHTING:
            {
            if (nanoglState.lighting)
                {
                nanoglState.lighting = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_LINE_SMOOTH:
            {
            if (nanoglState.line_smooth)
                {
                nanoglState.line_smooth = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
/*        case GL_MATRIX_PALETTE_OES:
            {
            if (nanoglState.matrix_palette_oes)
                {
                nanoglState.matrix_palette_oes = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }*/
        case GL_MULTISAMPLE:
            {
            if (nanoglState.multisample)
                {
                nanoglState.multisample = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_NORMALIZE:
            {
            if (nanoglState.normalize)
                {
                nanoglState.normalize = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
/*        case GL_POINT_SPRITE_OES:
            {
            if (nanoglState.point_sprite_oes)
                {
                nanoglState.point_sprite_oes = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }*/
        case GL_POLYGON_OFFSET_FILL:
            {
            if (nanoglState.polygon_offset_fill)
                {
                nanoglState.polygon_offset_fill = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_RESCALE_NORMAL:
            {
            if (nanoglState.rescale_normal)
                {
                nanoglState.rescale_normal = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_SAMPLE_ALPHA_TO_COVERAGE:
            {
            if (nanoglState.sample_alpha_to_coverage)
                {
                nanoglState.sample_alpha_to_coverage = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_SAMPLE_ALPHA_TO_ONE:
            {
            if (nanoglState.sample_alpha_to_one)
                {
                nanoglState.sample_alpha_to_one = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_SAMPLE_COVERAGE:
            {
            if (nanoglState.sample_coverage)
                {
                nanoglState.sample_coverage = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_SCISSOR_TEST:
            {
            if (nanoglState.scissor_test)
                {
                nanoglState.scissor_test = GL_FALSE;
                statechanged = GL_TRUE;
                }
            break;
            }
        case GL_STENCIL_TEST:
            {
            return;
/*            if (nanoglState.stencil_test)
                {
                nanoglState.stencil_test = GL_FALSE;
                statechanged = GL_TRUE;
                }*/
            break;
            }
        case GL_TEXTURE_2D:   
            {
            if (activetmuState->texture_2d.value)
                {
                FlushOnStateChange();
                glEsImpl->glDisable(cap);
                activetmuState->texture_2d.value = GL_FALSE;
                return;


                }
            break;
            }
        default:
            break;
        }
        
    if (statechanged)
        {
        FlushOnStateChange();
        glEsImpl->glDisable(cap);
        }    
    }
    
void glVertex2f(GLfloat x, GLfloat y)
    {
    glVertex3f(x,y,0.0f);
    }


__FORCEINLINE unsigned int ClampTo255(float value)
    {
    unsigned int retval = (unsigned int)(value);
    if (retval > 255)
        {
        retval = 255;
        }
    return retval;
    }

void glColor3f(	GLfloat red, GLfloat green, GLfloat blue)
    {
    currentVertexAttrib.red = (unsigned char)ClampTo255(red*255.0f);
    currentVertexAttrib.green = (unsigned char)ClampTo255(green*255.0f);
    currentVertexAttrib.blue = (unsigned char)ClampTo255(blue*255.0f);
    currentVertexAttrib.alpha = 255;
    }

void glTexCoord2fv( const GLfloat *v ) 
    {
    memcpy(&currentVertexAttrib.s, v, 2*sizeof(float));
    }


void glTexCoord2f(GLfloat s, GLfloat t)
    {
    currentVertexAttrib.s = s;
    currentVertexAttrib.t = t;
    }
    
void glViewport (GLint x, GLint y, GLsizei width, GLsizei height)
    {
    FlushOnStateChange();
    glEsImpl->glViewport(x,y,width,height);
    }
    
void glLoadIdentity (void)
    {
    FlushOnStateChange();
    glEsImpl->glLoadIdentity();
    }
    
void glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
    {
    currentVertexAttrib.red = (unsigned char)ClampTo255(red*255.0f);
    currentVertexAttrib.green = (unsigned char)ClampTo255(green*255.0f);
    currentVertexAttrib.blue = (unsigned char)ClampTo255(blue*255.0f);
    currentVertexAttrib.alpha = (unsigned char)ClampTo255(alpha*255.0f);
    }

void glOrtho (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
    {
    FlushOnStateChange();
    glEsImpl->glOrthof(left,right,bottom,top, zNear,zFar);
    }

void glMatrixMode (GLenum mode)
    {
    if (nanoglState.matrixmode == mode)
        {
        return;
        }
    nanoglState.matrixmode = mode;
    FlushOnStateChange();
    glEsImpl->glMatrixMode(mode);
    }

void glTexParameterf (GLenum target, GLenum pname, GLfloat param)
    {
    if (pname == GL_TEXTURE_BORDER_COLOR)
        {
        return; // not supported by opengl es
        }
    if (    (pname == GL_TEXTURE_WRAP_S ||
             pname == GL_TEXTURE_WRAP_T) &&
             param == GL_CLAMP)
             {
             param = 0x812F;
             }

    FlushOnStateChange();
    glEsImpl->glTexParameterf(target, pname,param);
    }
    
void glTexParameterfv(	GLenum target, GLenum pname, const GLfloat *params) 
    {
    glTexParameterf(target, pname, params[0]);
    }

void glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
    {
    FlushOnStateChange();
    internalformat = format;
    glEsImpl->glTexImage2D(target, level, internalformat, width, height,border,format,type,pixels);
    }

void glDrawBuffer(GLenum /*mode*/)
    {
    }

void glTranslatef (GLfloat x, GLfloat y, GLfloat z)
    {
    FlushOnStateChange();
    glEsImpl->glTranslatef(x,y,z);
    }
    
void glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
    {
    FlushOnStateChange();
    glEsImpl->glRotatef(angle, x, y, z);
    }

void glScalef (GLfloat x, GLfloat y, GLfloat z)
    {
    FlushOnStateChange();
    glEsImpl->glScalef(x,y,z);
    }
    
void glDepthRange(GLclampf zNear, GLclampf zFar)
    {
    if ((nanoglState.depth_range_near == zNear) &&(nanoglState.depth_range_far == zFar))
        {
        return;
        }
    else
        {
        nanoglState.depth_range_near = zNear;
        nanoglState.depth_range_far = zFar;
        }
    FlushOnStateChange();
    glEsImpl->glDepthRangef(zNear, zFar);
    }

void glDepthFunc (GLenum func)
    {
    if (nanoglState.depth_func == func)
        {
        return;
        }
    else
        {
        nanoglState.depth_func = func;
        }
    FlushOnStateChange();
    glEsImpl->glDepthFunc(func);
    }

void glFinish (void)
    {
    FlushOnStateChange();
    glEsImpl->glFinish();
    }

void glGetFloatv (GLenum pname, GLfloat *params)
    {
    FlushOnStateChange();
    glEsImpl->glGetFloatv(pname, params);
    }

void glCullFace (GLenum mode)
    {
    if (nanoglState.cullface == mode)
        {
        return;
        }
    else
        {
        nanoglState.cullface = mode;
        }
    FlushOnStateChange();
    glEsImpl->glCullFace(mode);
    }

void glFrustum (GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
    {
    FlushOnStateChange();
    glEsImpl->glFrustumf(left,right,bottom,top,zNear,zFar);
    }

void glClear (GLbitfield mask)
    {
    FlushOnStateChange();
    glEsImpl->glClear(mask);
    }


void glVertex3f( GLfloat x, GLfloat y, GLfloat z )
    {
    GLfloat* vert = (GLfloat*)ptrVertexAttribArray++;
    *vert++ = x;
    *vert++ = y;
    *vert++ = z;
#if defined(__MULTITEXTURE_SUPPORT__)
    memcpy(vert, &currentVertexAttrib.red, 5*sizeof(GLfloat));
#else
    memcpy(vert+1, &currentVertexAttrib.red, 3*sizeof(GLfloat));
#endif

    }

void glColor4fv( const GLfloat *v )
    {
    currentVertexAttrib.red = (unsigned char)ClampTo255(v[0]*255.0f);
    currentVertexAttrib.green = (unsigned char)ClampTo255(v[1]*255.0f);
    currentVertexAttrib.blue = (unsigned char)ClampTo255(v[2]*255.0f);
    currentVertexAttrib.alpha = (unsigned char)ClampTo255(v[3]*255.0f);
    }
    
void glColor3ubv( const GLubyte* v)
    {
    currentVertexAttrib.red = v[0];
    currentVertexAttrib.green = v[1];
    currentVertexAttrib.blue = v[2];
    currentVertexAttrib.alpha = 255;
    }
    
void glColor4ubv( const GLubyte *v ) 
    {  
    *((unsigned int*)(&currentVertexAttrib.red)) = *((unsigned int*)(v));
    }
    
void glColor3fv( const GLfloat *v ) 
    {
    currentVertexAttrib.red = (unsigned char)ClampTo255(v[0]*255.0f);
    currentVertexAttrib.green = (unsigned char)ClampTo255(v[1]*255.0f);
    currentVertexAttrib.blue = (unsigned char)ClampTo255(v[2]*255.0f);
    currentVertexAttrib.alpha = 255;
    }



void glHint (GLenum target, GLenum mode)
    {
    FlushOnStateChange();
    glEsImpl->glHint(target, mode);
    }

void glBlendFunc (GLenum sfactor, GLenum dfactor)
    {
    if ((nanoglState.sfactor == sfactor) && (nanoglState.dfactor == dfactor))
        {
        return;
        }
    nanoglState.sfactor = sfactor;
    nanoglState.dfactor = dfactor;
    FlushOnStateChange();
    glEsImpl->glBlendFunc(sfactor, dfactor);
    }

void glPopMatrix (void)
    {
    FlushOnStateChange();
    glEsImpl->glPopMatrix();
    }

void glShadeModel (GLenum mode)
    {
    if (nanoglState.shademodel == mode)
        {
        return;
        }
    nanoglState.shademodel = mode;
    FlushOnStateChange();
    glEsImpl->glShadeModel(mode);
    }

void glPushMatrix (void)
    {
    FlushOnStateChange();
    glEsImpl->glPushMatrix();
    }

void glTexEnvf (GLenum target, GLenum pname, GLfloat param)
    {
    if (target == GL_TEXTURE_ENV)
        {
        if (pname == GL_TEXTURE_ENV_MODE)
            {
            if (param == activetmuState->texture_env_mode.value)
                {
                return;
                } 
            else
                {
                FlushOnStateChange();
                glEsImpl->glTexEnvf(target, pname, param);
                activetmuState->texture_env_mode.value = param;
                return;
                }
            }
        }
    FlushOnStateChange();
    glEsImpl->glTexEnvf(target, pname, param);
    }

void glVertex3fv( const GLfloat *v )
    {
    GLfloat* vert = (GLfloat*)ptrVertexAttribArray++;
    memcpy(vert, v, 3*sizeof(GLfloat));
#if defined(__MULTITEXTURE_SUPPORT__)
    memcpy(vert+3, &currentVertexAttrib.red, 5*sizeof(GLfloat));
#else
    memcpy(vert+4, &currentVertexAttrib.red, 3*sizeof(GLfloat));
#endif
    }
    

void glDepthMask (GLboolean flag)
    {
    if (nanoglState.depthmask == flag)
        {
        return;
        }
    nanoglState.depthmask = flag;
    FlushOnStateChange();
    glEsImpl->glDepthMask(flag);
    }

void glBindTexture (GLenum target, GLuint texture)
    {
    if (activetmuState->boundtexture.value == texture)
        {
        return;
        }
    FlushOnStateChange();
    activetmuState->boundtexture.value = texture;
    glEsImpl->glBindTexture(target, texture);
    }


void glGetIntegerv (GLenum pname, GLint *params)
    {
    FlushOnStateChange();
    glEsImpl->glGetIntegerv(pname, params);
    }

GLubyte nano_extensions_string[4096];
const GLubyte* glGetString (GLenum name)
    {

    if (name == GL_EXTENSIONS)
        {
#if defined(__MULTITEXTURE_SUPPORT__)
        sprintf((char*)nano_extensions_string,"%s %s",glEsImpl->glGetString(name),"GL_ARB_multitexture EXT_texture_env_add");
#else
        sprintf((char*)nano_extensions_string,"%s %s",glEsImpl->glGetString(name),"EXT_texture_env_add");
#endif
        return nano_extensions_string;
        }
    return glEsImpl->glGetString(name);
    }

void glAlphaFunc (GLenum func, GLclampf ref)
    {
    FlushOnStateChange();
    glEsImpl->glAlphaFunc(func,ref);
    }

void glFlush (void)
    {
    FlushOnStateChange();
    glEsImpl->glFlush();
    }

void glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels)
    {
    if (format == GL_DEPTH_COMPONENT)
        {
        // OpenglEs 1.1 does not support reading depth buffer without an extension
        memset(pixels, 0xff,4);
        return;   
        }
    FlushOnStateChange();
    glEsImpl->glReadPixels(x,y,width,height,format,type,pixels);
    }

void glReadBuffer( GLenum /*mode*/ )
    {
    
    }

void glLoadMatrixf (const GLfloat *m)
    {
    FlushOnStateChange();
    glEsImpl->glLoadMatrixf(m);
    }

void glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
    {
    FlushOnStateChange();
    glEsImpl->glTexSubImage2D(target,level,xoffset,yoffset,width,height,format,type,pixels);
    }

void glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
    {
    FlushOnStateChange();
    glEsImpl->glClearColor(red,green,blue,alpha);
    }

GLenum glGetError (void)
    {
    FlushOnStateChange();
    return GL_NO_ERROR;//glEsImpl->glGetError();
    }

void glActiveTexture (GLenum texture)
    {
    if (activetmu == texture)
        {
        return;
        }
    if (delayedttmuchange)
        {
        delayedttmuchange = GL_FALSE;
        }
    else
        {
        delayedttmuchange = GL_TRUE;
        delayedtmutarget = texture;
        }
    if (texture == GL_TEXTURE0)
        {
        activetmuState = &tmuState0;
        }
    else
        {
        activetmuState = &tmuState1;
        }
    activetmu = texture;
    }
    
void glClientActiveTexture (GLenum texture)
    {
    clientactivetmu = texture;
    }
    
void glPolygonMode( GLenum face, GLenum mode )
    {
    }

void glDeleteTextures( GLsizei n, const GLuint *textures ) 
    {
    FlushOnStateChange();
    glEsImpl->glDeleteTextures(n,textures);
    }

void glClearDepth( GLclampf depth ) 
    {
    FlushOnStateChange();
    glEsImpl->glClearDepthf( depth ); 
    }

void glClipPlane( GLenum plane, const GLdouble *equation ) 
    {
    FlushOnStateChange();
    float array[4];
    array[0] = (GLfloat)(equation[0]);
    array[1] = (GLfloat)(equation[1]);
    array[2] = (GLfloat)(equation[2]);
    array[3] = (GLfloat)(equation[3]);
    glEsImpl->glClipPlanef( plane, array );
    }

void glScissor( GLint x, GLint	y, GLsizei width, GLsizei height ) 
    {
    FlushOnStateChange();
    glEsImpl->glScissor( x, y, width,height);
    }

void glPointSize( GLfloat size ) 
    {
    FlushOnStateChange();
    glEsImpl->glPointSize( size );
    }

void glArrayElement(GLint i) {}
void glLineWidth(GLfloat width) {}
void glCallList( GLuint list ) {}
void glColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha ) {}
void glStencilFunc( GLenum func, GLint ref, GLuint mask ) {}
void glStencilOp( GLenum fail, GLenum zfail, GLenum zpass ) {}

struct ptrstate vertex_array;                                       
struct ptrstate color_array;
struct ptrstate texture_coord_array;

void glDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices ) 
    {
    // ensure that all primitives specified between glBegin/glEnd pairs
    // are rendered first, and that we have correct tmu in use..
    FlushOnStateChange();
    // setup correct vertex/color/texcoord pointers
    if (arraysValid || 
        tmuState0.vertex_array.changed ||
        tmuState0.color_array.changed ||
        tmuState0.texture_coord_array.changed)
        {
        glEsImpl->glClientActiveTexture(GL_TEXTURE0);
        }
    if (arraysValid || tmuState0.vertex_array.changed)
        {
        if (tmuState0.vertex_array.enabled)
            {
            glEsImpl->glEnableClientState(GL_VERTEX_ARRAY);
            }
        else
            {
            glEsImpl->glDisableClientState(GL_VERTEX_ARRAY);
            }
        glEsImpl->glVertexPointer(tmuState0.vertex_array.size, 
                                  tmuState0.vertex_array.type, 
                                  tmuState0.vertex_array.stride, 
                                  tmuState0.vertex_array.ptr);
        tmuState0.vertex_array.changed = GL_FALSE;
        }
    if (arraysValid || tmuState0.color_array.changed)
        {
        if (tmuState0.color_array.enabled)
            {
            glEsImpl->glEnableClientState(GL_COLOR_ARRAY);
            }
        else
            {
            glEsImpl->glDisableClientState(GL_COLOR_ARRAY);
            }
        glEsImpl->glColorPointer(tmuState0.color_array.size, 
                                 tmuState0.color_array.type, 
                                 tmuState0.color_array.stride, 
                                 tmuState0.color_array.ptr);
        tmuState0.color_array.changed = GL_FALSE;
        }
    if (arraysValid || tmuState0.texture_coord_array.changed)
        {
        tmuState0.texture_coord_array.changed = GL_FALSE;
        if (tmuState0.texture_coord_array.enabled)
            {
            glEsImpl->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            }
        else
            {
            glEsImpl->glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            }
        glEsImpl->glTexCoordPointer(tmuState0.texture_coord_array.size, 
                                        tmuState0.texture_coord_array.type, 
                                        tmuState0.texture_coord_array.stride, 
                                        tmuState0.texture_coord_array.ptr);
        }
 
    if (arraysValid || tmuState1.texture_coord_array.changed)
        {
        tmuState1.texture_coord_array.changed = GL_FALSE;
        glEsImpl->glClientActiveTexture(GL_TEXTURE1);
        if (tmuState1.texture_coord_array.enabled)
            {
            glEsImpl->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            }
        else
            {
            glEsImpl->glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            }
        glEsImpl->glTexCoordPointer(tmuState1.texture_coord_array.size, 
                                        tmuState1.texture_coord_array.type, 
                                        tmuState1.texture_coord_array.stride, 
                                        tmuState1.texture_coord_array.ptr);
        }

    arraysValid = GL_FALSE;
    glEsImpl->glDrawElements(mode, count, type, indices);
    }

void glEnableClientState(GLenum  array) 
    {
    struct nanotmuState* clientstate = NULL;
    if (clientactivetmu == GL_TEXTURE0)
        {
        clientstate = &tmuState0;
        }
    else if (clientactivetmu == GL_TEXTURE1)
        {
        clientstate = &tmuState1;
        }
    else
        {
        return;
        }
    switch (array)
        {
        case GL_VERTEX_ARRAY:
            if (clientstate->vertex_array.enabled)
                {
                return;
                }
            clientstate->vertex_array.enabled = GL_TRUE;
            clientstate->vertex_array.changed = GL_TRUE;
            break;
        case GL_COLOR_ARRAY:
            if (clientstate->color_array.enabled)
                {
                return;
                }
            clientstate->color_array.enabled = GL_TRUE;
            clientstate->color_array.changed = GL_TRUE;

            break;
        case GL_TEXTURE_COORD_ARRAY:
            if (clientstate->texture_coord_array.enabled)
                {
                return;
                }
            clientstate->texture_coord_array.enabled = GL_TRUE;
            clientstate->texture_coord_array.changed = GL_TRUE;
            break;
        default:
            break;
        }
    }
void glDisableClientState(GLenum  array) 
    {
    struct nanotmuState* clientstate = NULL;
    if (clientactivetmu == GL_TEXTURE0)
        {
        clientstate = &tmuState0;
        }
    else if (clientactivetmu == GL_TEXTURE1)
        {
        clientstate = &tmuState1;
        }
    else
        {
        return;
        }
    switch (array)
        {
        case GL_VERTEX_ARRAY:
            if (!clientstate->vertex_array.enabled)
                {
                return;
                }
            clientstate->vertex_array.enabled = GL_FALSE;
            clientstate->vertex_array.changed = GL_TRUE;
            break;
        case GL_COLOR_ARRAY:
            if (!clientstate->color_array.enabled)
                {
                return;
                }
            clientstate->color_array.enabled = GL_FALSE;
            clientstate->color_array.changed = GL_TRUE;

            break;
        case GL_TEXTURE_COORD_ARRAY:
            if (!clientstate->texture_coord_array.enabled)
                {
                return;
                }
            clientstate->texture_coord_array.enabled = GL_FALSE;
            clientstate->texture_coord_array.changed = GL_TRUE;
            break;
        default:
            break;
        }
    }
void glVertexPointer(	GLint size,	GLenum type,GLsizei	stride,	const GLvoid *pointer ) 
    {
    if (tmuState0.vertex_array.size == size &&
        tmuState0.vertex_array.stride == stride &&
        tmuState0.vertex_array.type == type &&
        tmuState0.vertex_array.ptr == pointer)
        {
        return;
        }
    tmuState0.vertex_array.size = size;
    tmuState0.vertex_array.stride = stride;
    tmuState0.vertex_array.type = type;
    tmuState0.vertex_array.ptr  = (GLvoid*)pointer;
    tmuState0.vertex_array.changed = GL_TRUE;
    }
void glTexCoordPointer( GLint	size,  GLenum type,  GLsizei stride,  const	GLvoid *pointer	) 
    {
    struct nanotmuState* clientstate = NULL;
    if (clientactivetmu == GL_TEXTURE0)
        {
        clientstate = &tmuState0;
        }
    else if (clientactivetmu == GL_TEXTURE1)
        {
        clientstate = &tmuState1;
        }
    if (clientstate->texture_coord_array.size == size &&
        clientstate->texture_coord_array.stride == stride &&
        clientstate->texture_coord_array.type == type &&
        clientstate->texture_coord_array.ptr == pointer)
        {
        return;
        }
    clientstate->texture_coord_array.size = size;
    clientstate->texture_coord_array.stride = stride;
    clientstate->texture_coord_array.type = type;
    clientstate->texture_coord_array.ptr  = (GLvoid*)pointer;
    clientstate->texture_coord_array.changed = GL_TRUE;
    }
void glColorPointer( GLint size,  GLenum type,  GLsizei stride,  const GLvoid *pointer ) 
    {
    if (tmuState0.color_array.size == size &&
        tmuState0.color_array.stride == stride &&
        tmuState0.color_array.type == type &&
        tmuState0.color_array.ptr == pointer)
        {
        return;
        }
    tmuState0.color_array.size = size;
    tmuState0.color_array.stride = stride;
    tmuState0.color_array.type = type;
    tmuState0.color_array.ptr  = (GLvoid*)pointer;
    tmuState0.color_array.changed = GL_TRUE;
    }
void glPolygonOffset( GLfloat factor, GLfloat units ) 
    {
    FlushOnStateChange();
    glEsImpl->glPolygonOffset(factor, units);
    }
void glStencilMask( GLuint mask ) {}
void glClearStencil( GLint s ) {}

#if defined(__MULTITEXTURE_SUPPORT__)

extern "C" void glMultiTexCoord2fARB( GLenum target, GLfloat s, GLfloat t );

void glMultiTexCoord2fARB( GLenum target, GLfloat s, GLfloat t )
    {
    if (target == GL_TEXTURE0)
        {
        glTexCoord2f(s,t);
        }
    else
        {
        currentVertexAttrib.s_multi = s;
        currentVertexAttrib.t_multi = t;
        }
    }
#endif

/* Vladimir  */
void glDrawArrays( GLenum mode, int first, int count)
{
    FlushOnStateChange();
    glEsImpl->glDrawArrays(mode, first , count);
}
void glMultMatrixf (const GLfloat *m)
{
    FlushOnStateChange();
    glEsImpl->glMultMatrixf(m);
}

void glPixelStorei (GLenum pname, GLint param)
{
    FlushOnStateChange();
    glEsImpl->glPixelStorei(pname, param);
}


void glFogf (GLenum pname, GLfloat param)
{
	FlushOnStateChange();
	glEsImpl->glFogf(pname, param);
}

void glFogfv (GLenum pname, const GLfloat *params)
{
	FlushOnStateChange();
	glEsImpl->glFogfv(pname, params);
}

void glGetTexParameteriv (GLenum target, GLenum pname, GLint *params)
{
	FlushOnStateChange();
	glEsImpl->glGetTexParameteriv(target, pname, params);
}


// This gives: called unimplemented OpenGL ES API (Android)
void glTexParameteri (GLenum target, GLenum pname, GLint param)
{
    if (pname == GL_TEXTURE_BORDER_COLOR) {
		return; // not supported by opengl es
	}
    if (    (pname == GL_TEXTURE_WRAP_S ||
             pname == GL_TEXTURE_WRAP_T) &&
             param == GL_CLAMP)   {
		param = 0x812F;
	}

	FlushOnStateChange();
	glEsImpl->glTexParameteri(target, pname, param);
}

void glTexParameterx (GLenum target, GLenum pname, GLfixed param)
{
    if (pname == GL_TEXTURE_BORDER_COLOR) {
		return; // not supported by opengl es
	}
    if (    (pname == GL_TEXTURE_WRAP_S ||
             pname == GL_TEXTURE_WRAP_T) &&
             param == GL_CLAMP)   {
		param = 0x812F;
	}
	FlushOnStateChange();
	glEsImpl->glTexParameterx(target, pname, param);
}

void glGenTextures (GLsizei n, GLuint *textures)
{
	FlushOnStateChange();
	glEsImpl->glGenTextures(n, textures);
}

void glFrontFace (GLenum mode)
{
	FlushOnStateChange();
	glEsImpl->glFrontFace(mode);
}



// End Vladimir



