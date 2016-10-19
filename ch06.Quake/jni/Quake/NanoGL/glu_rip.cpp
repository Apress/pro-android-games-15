/***************************************************************************
                          glu_rip.cpp  -  description

     A few convenience routines ripped from MesaGL -- don't hurt me please
 ***************************************************************************/

#include <stdio.h>

#include "glu.h"

#define CEILING( A, B )  ( (A) % (B) == 0 ? (A)/(B) : (A)/(B)+1 )

void dummy(GLuint j, GLuint k){
}

enum {
  /* Quadric draw styles */
  GLU_SMOOTH	= 100000,
  GLU_FLAT	= 100001,
  GLU_NONE	= 100002,

	/* Quadric draw styles */
  GLU_POINT	= 100010,
  GLU_LINE	= 100011,
  GLU_FILL	= 100012,
  GLU_SILHOUETTE	= 100013,

	/* Quadric orientation */
  GLU_OUTSIDE	= 100020,
  GLU_INSIDE	= 100021,

	/* Tesselator */
  GLU_BEGIN	= 100100,
  GLU_VERTEX	= 100101,
  GLU_END		= 100102,
  GLU_ERROR	= 100103,
  GLU_EDGE_FLAG	= 100104,

  /* Contour types */
  GLU_CW		= 100120,
  GLU_CCW		= 100121,
  GLU_INTERIOR	= 100122,
  GLU_EXTERIOR	= 100123,
  GLU_UNKNOWN	= 100124,

  /* Tesselation errors */
  GLU_TESS_ERROR1	= 100151,  /* missing gluEndPolygon */
  GLU_TESS_ERROR2 = 100152,  /* missing gluBeginPolygon */
  GLU_TESS_ERROR3 = 100153,  /* misoriented contour */
  GLU_TESS_ERROR4 = 100154,  /* vertex/edge intersection */
  GLU_TESS_ERROR5 = 100155,  /* misoriented or self-intersecting loops */
  GLU_TESS_ERROR6 = 100156,  /* coincident vertices */
  GLU_TESS_ERROR7 = 100157,  /* all vertices collinear */
  GLU_TESS_ERROR8 = 100158,  /* intersecting edges */
  GLU_TESS_ERROR9 = 100159,  /* not coplanar contours */

  /* NURBS */
  GLU_AUTO_LOAD_MATRIX	= 100200,
  GLU_CULLING		= 100201,
  GLU_PARAMETRIC_TOLERANCE= 100202,
  GLU_SAMPLING_TOLERANCE	= 100203,
  GLU_DISPLAY_MODE	= 100204,
  GLU_SAMPLING_METHOD	= 100205,
  GLU_U_STEP		= 100206,
  GLU_V_STEP		= 100207,

  GLU_PATH_LENGTH		= 100215,
  GLU_PARAMETRIC_ERROR	= 100216,
  GLU_DOMAIN_DISTANCE	= 100217,

  GLU_MAP1_TRIM_2		= 100210,
  GLU_MAP1_TRIM_3		= 100211,

  GLU_OUTLINE_POLYGON	= 100240,
  GLU_OUTLINE_PATCH	= 100241,

  GLU_NURBS_ERROR1  = 100251,   /* spline order un-supported */
  GLU_NURBS_ERROR2  = 100252,   /* too few knots */
  GLU_NURBS_ERROR3  = 100253,   /* valid knot range is empty */
  GLU_NURBS_ERROR4  = 100254,   /* decreasing knot sequence */
  GLU_NURBS_ERROR5  = 100255,   /* knot multiplicity > spline order */
  GLU_NURBS_ERROR6  = 100256,   /* endcurve() must follow bgncurve() */
  GLU_NURBS_ERROR7  = 100257,   /* bgncurve() must precede endcurve() */
  GLU_NURBS_ERROR8  = 100258,   /* ctrlarray or knot vector is NULL */
  GLU_NURBS_ERROR9  = 100259,   /* can't draw pwlcurves */
  GLU_NURBS_ERROR10 = 100260,   /* missing gluNurbsCurve() */
  GLU_NURBS_ERROR11 = 100261,   /* missing gluNurbsSurface() */
  GLU_NURBS_ERROR12 = 100262,   /* endtrim() must precede endsurface() */
  GLU_NURBS_ERROR13 = 100263,   /* bgnsurface() must precede endsurface() */
  GLU_NURBS_ERROR14 = 100264,   /* curve of improper type passed as trim curve */
  GLU_NURBS_ERROR15 = 100265,   /* bgnsurface() must precede bgntrim() */
  GLU_NURBS_ERROR16 = 100266,   /* endtrim() must follow bgntrim() */
  GLU_NURBS_ERROR17 = 100267,   /* bgntrim() must precede endtrim()*/
  GLU_NURBS_ERROR18 = 100268,   /* invalid or missing trim curve*/
  GLU_NURBS_ERROR19 = 100269,   /* bgntrim() must precede pwlcurve() */
  GLU_NURBS_ERROR20 = 100270,   /* pwlcurve referenced twice*/
  GLU_NURBS_ERROR21 = 100271,   /* pwlcurve and nurbscurve mixed */
  GLU_NURBS_ERROR22 = 100272,   /* improper usage of trim data type */
  GLU_NURBS_ERROR23 = 100273,   /* nurbscurve referenced twice */
  GLU_NURBS_ERROR24 = 100274,   /* nurbscurve and pwlcurve mixed */
  GLU_NURBS_ERROR25 = 100275,   /* nurbssurface referenced twice */
  GLU_NURBS_ERROR26 = 100276,   /* invalid property */
  GLU_NURBS_ERROR27 = 100277,   /* endsurface() must follow bgnsurface() */
  GLU_NURBS_ERROR28 = 100278,   /* intersecting or misoriented trim curves */
  GLU_NURBS_ERROR29 = 100279,   /* intersecting trim curves */
  GLU_NURBS_ERROR30 = 100280,   /* UNUSED */
  GLU_NURBS_ERROR31 = 100281,   /* unconnected trim curves */
  GLU_NURBS_ERROR32 = 100282,   /* unknown knot error */
  GLU_NURBS_ERROR33 = 100283,   /* negative vertex count encountered */
  GLU_NURBS_ERROR34 = 100284,   /* negative byte-stride */
  GLU_NURBS_ERROR35 = 100285,   /* unknown type descriptor */
  GLU_NURBS_ERROR36 = 100286,   /* null control point reference */
  GLU_NURBS_ERROR37 = 100287,   /* duplicate point on pwlcurve */

	/* Errors */
  GLU_INVALID_ENUM		= 100900,
  GLU_INVALID_VALUE		= 100901,
  GLU_OUT_OF_MEMORY		= 100902,
  GLU_INCOMPATIBLE_GL_VERSION	= 100903,

	/* New in GLU 1.1 */
  GLU_VERSION	= 100800,
  GLU_EXTENSIONS	= 100801
};


static GLint round2( GLint n )
{
  GLint m;

  for (m=1; m<n; m*=2)
    ;

  /* m>=n */
  if (m-n <= n-m/2) {
    return m;
  }
  else {
    return m/2;
  }
}


static GLint bytes_per_pixel( GLenum format, GLenum type )
{
  GLint n, m;

  switch (format) {
  /*
  case GL_COLOR_INDEX:
  case GL_STENCIL_INDEX:
  case GL_DEPTH_COMPONENT:
  case GL_RED:
  case GL_GREEN:
  case GL_BLUE: */
  case GL_ALPHA:
  case GL_LUMINANCE:
    n = 1;
    break;
  case GL_LUMINANCE_ALPHA:
    n = 2;
    break;
  case GL_RGB:
    n = 3;
    break;
  case GL_RGBA:
    n = 4;
    break;
  default:
    n = 0;
  }

  switch (type) {
  case GL_UNSIGNED_BYTE:	m = sizeof(GLubyte);	break;
  case GL_BYTE:		m = sizeof(GLbyte);	break;
//  case GL_BITMAP:		m = 1;			break;
  case GL_UNSIGNED_SHORT:	m = sizeof(GLushort);	break;
  case GL_SHORT:		m = sizeof(GLshort);	break;
//  case GL_UNSIGNED_INT:	m = sizeof(GLuint);	break;
//  case GL_INT:		m = sizeof(GLint);	break;
  case GL_FLOAT:		m = sizeof(GLfloat);	break;
  default:			m = 0;
  }

  return n * m;
}



GLint gluScaleImage( GLenum format,
		     GLint widthin, GLint heightin,
		     GLenum typein, const void *datain,
		     GLint widthout, GLint heightout,
		     GLenum typeout, void *dataout )
{
//printf("gluScaleImg win=%d hin=%d typein=%d wout=%d hout=%d typeout=%d\n"
//	, widthin, heightin, typein
//	, widthout, heightout, typeout);

  GLint components, i, j, k;
  GLfloat *tempin, *tempout;
  GLfloat sx, sy;
  GLint unpackrowlength, unpackalignment, unpackskiprows, unpackskippixels;
  GLint packrowlength, packalignment, packskiprows, packskippixels;
  GLint sizein, sizeout;
  GLint rowstride, rowlen;


  /* Determine number of components per pixel */
  switch (format) {
/*  
  case GL_COLOR_INDEX:
  case GL_STENCIL_INDEX:
  case GL_DEPTH_COMPONENT:
  case GL_RED:
  case GL_GREEN:
  case GL_BLUE: */
  case GL_ALPHA:
  case GL_LUMINANCE:
    components = 1;
    break;
  case GL_LUMINANCE_ALPHA:
    components = 2;
    break;
  case GL_RGB:
    components = 3;
    break;
  case GL_RGBA:
    components = 4;
    break;
  default:
    return GLU_INVALID_ENUM;
  }

  /* Determine bytes per input datum */
  switch (typein) {
  case GL_UNSIGNED_BYTE:	sizein = sizeof(GLubyte);	break;
  case GL_BYTE:		sizein = sizeof(GLbyte);	break;
  case GL_UNSIGNED_SHORT:	sizein = sizeof(GLushort);	break;
  case GL_SHORT:		sizein = sizeof(GLshort);	break;
//  case GL_UNSIGNED_INT:	sizein = sizeof(GLuint);	break;
//  case GL_INT:		sizein = sizeof(GLint);		break;
  case GL_FLOAT:		sizein = sizeof(GLfloat);	break;
//  case GL_BITMAP:
    /* not implemented yet */
  default:
    return GL_INVALID_ENUM;
  }

  /* Determine bytes per output datum */
  switch (typeout) {
  case GL_UNSIGNED_BYTE:	sizeout = sizeof(GLubyte);	break;
  case GL_BYTE:		sizeout = sizeof(GLbyte);	break;
  case GL_UNSIGNED_SHORT:	sizeout = sizeof(GLushort);	break;
  case GL_SHORT:		sizeout = sizeof(GLshort);	break;
//  case GL_UNSIGNED_INT:	sizeout = sizeof(GLuint);	break;
//  case GL_INT:		sizeout = sizeof(GLint);	break;
  case GL_FLOAT:		sizeout = sizeof(GLfloat);	break;
//  case GL_BITMAP:
    /* not implemented yet */
  default:
    return GL_INVALID_ENUM;
  }

  /* Get glPixelStore state */
  unpackrowlength = 0; //  glGetIntegerv( GL_UNPACK_ROW_LENGTH, &unpackrowlength );
  unpackalignment = 1; // RETURNS GARBAGE glGetIntegerv( GL_UNPACK_ALIGNMENT, &unpackalignment );
  unpackskiprows = 0; //  glGetIntegerv( GL_UNPACK_SKIP_ROWS, &unpackskiprows );
  unpackskippixels = 0; //  glGetIntegerv( GL_UNPACK_SKIP_PIXELS, &unpackskippixels );
  packrowlength = 0; //  glGetIntegerv( GL_PACK_ROW_LENGTH, &packrowlength );
  packalignment = 1; // RETURNS GARBAGE glGetIntegerv( GL_PACK_ALIGNMENT, &packalignment );
  packskiprows = 0; //  glGetIntegerv( GL_PACK_SKIP_ROWS, &packskiprows );
  packskippixels = 0; //  glGetIntegerv( GL_PACK_SKIP_PIXELS, &packskippixels );

//printf("gluScaleImg components=%d sizein=%d sizeout=%d win=%d hin=%d unpackalignment=%d packalignment=%d\n",
//	components, sizein, sizeout, widthin, heightin, unpackalignment, packalignment);

  /* Allocate storage for intermediate images */
  tempin = (GLfloat *) malloc( widthin * heightin
			       * components * sizeof(GLfloat) );
  if (!tempin) {
    return GLU_OUT_OF_MEMORY;
  }
  tempout = (GLfloat *) malloc( widthout * heightout
				* components * sizeof(GLfloat) );
  if (!tempout) {
    free( tempin );
    return GLU_OUT_OF_MEMORY;
  }


  /*
   * Unpack the pixel data and convert to floating point
   */

  if (unpackrowlength>0) {
    rowlen = unpackrowlength;
  }
  else {
    rowlen = widthin;
  }
  if (sizein >= unpackalignment) {
    rowstride = components * rowlen;
  }
  else {
    rowstride = unpackalignment/sizein
      * CEILING( components * rowlen * sizein, unpackalignment );
  }

//printf("gluScaleImg Unpacking the pixel data and convert to floating point rowlen=%d rowstride=%d\n"
//	, rowlen, rowstride);
  
  switch (typein) {
  case GL_UNSIGNED_BYTE:
    k = 0;
    for (i=0;i<heightin;i++) {
      GLubyte *ubptr = (GLubyte *) datain
	+ i * rowstride
	+ unpackskiprows * rowstride
	+ unpackskippixels * components;
      for (j=0;j<widthin*components;j++) {
	dummy(j, k);
	tempin[k++] = (GLfloat) *ubptr++;
      }
    }
    break;
  case GL_BYTE:
    k = 0;
    for (i=0;i<heightin;i++) {
      GLbyte *bptr = (GLbyte *) datain
	+ i * rowstride
	+ unpackskiprows * rowstride
	+ unpackskippixels * components;
      for (j=0;j<widthin*components;j++) {
	dummy(j, k);
	tempin[k++] = (GLfloat) *bptr++;
      }
    }
    break;
  case GL_UNSIGNED_SHORT:
    k = 0;
    for (i=0;i<heightin;i++) {
      GLushort *usptr = (GLushort *) datain
	+ i * rowstride
	+ unpackskiprows * rowstride
	+ unpackskippixels * components;
      for (j=0;j<widthin*components;j++) {
	dummy(j, k);
	tempin[k++] = (GLfloat) *usptr++;
      }
    }
    break;
  case GL_SHORT:
    k = 0;
    for (i=0;i<heightin;i++) {
      GLshort *sptr = (GLshort *) datain
	+ i * rowstride
	+ unpackskiprows * rowstride
	+ unpackskippixels * components;
      for (j=0;j<widthin*components;j++) {
	dummy(j, k);
	tempin[k++] = (GLfloat) *sptr++;
      }
    }
    break;
/*	
  case GL_UNSIGNED_INT:
    k = 0;
    for (i=0;i<heightin;i++) {
      GLuint *uiptr = (GLuint *) datain
	+ i * rowstride
	+ unpackskiprows * rowstride
	+ unpackskippixels * components;
      for (j=0;j<widthin*components;j++) {
	dummy(j, k);
	tempin[k++] = (GLfloat) *uiptr++;
      }
    }
    break; 
  case GL_INT:
    k = 0;
    for (i=0;i<heightin;i++) {
      GLint *iptr = (GLint *) datain
	+ i * rowstride
	+ unpackskiprows * rowstride
	+ unpackskippixels * components;
      for (j=0;j<widthin*components;j++) {
	dummy(j, k);
	tempin[k++] = (GLfloat) *iptr++;
      }
    }
    break;
*/	
  case GL_FLOAT:
    k = 0;
    for (i=0;i<heightin;i++) {
      GLfloat *fptr = (GLfloat *) datain
	+ i * rowstride
	+ unpackskiprows * rowstride
	+ unpackskippixels * components;
      for (j=0;j<widthin*components;j++) {
	dummy(j, k);
	tempin[k++] = *fptr++;
      }
    }
    break;
  default:
    return GLU_INVALID_ENUM;
  }


  /*
   * Scale the image!
   */

  if (widthout > 1)
    sx = (GLfloat) (widthin-1) / (GLfloat) (widthout-1);
  else
    sx = (GLfloat) (widthin-1);
  if (heightout > 1)
    sy = (GLfloat) (heightin-1) / (GLfloat) (heightout-1);
  else
    sy = (GLfloat) (heightin-1);

  /*#define POINT_SAMPLE*/
#ifdef POINT_SAMPLE
  for (i=0;i<heightout;i++) {
    GLint ii = i * sy;
    for (j=0;j<widthout;j++) {
      GLint jj = j * sx;

      GLfloat *src = tempin + (ii * widthin + jj) * components;
      GLfloat *dst = tempout + (i * widthout + j) * components;

      for (k=0;k<components;k++) {
	*dst++ = *src++;
      }
    }
  }
#else
  if (sx<1.0 && sy<1.0) {
    /* magnify both width and height:  use weighted sample of 4 pixels */
    GLint i0, i1, j0, j1;
    GLfloat alpha, beta;
    GLfloat *src00, *src01, *src10, *src11;
    GLfloat s1, s2;
    GLfloat *dst;

    for (i=0;i<heightout;i++) {
      i0 = (int) i * (int) sy;
      i1 = i0 + 1;
      if (i1 >= heightin) i1 = heightin-1;
      /*	 i1 = (i+1) * sy - EPSILON;*/
      alpha = i*sy - i0;
      for (j=0;j<widthout;j++) {
	j0 = (int) j * (int) sx;
	j1 = j0 + 1;
	if (j1 >= widthin) j1 = widthin-1;
	/*	    j1 = (j+1) * sx - EPSILON; */
	beta = j*sx - j0;

	/* compute weighted average of pixels in rect (i0,j0)-(i1,j1) */
	src00 = tempin + (i0 * widthin + j0) * components;
	src01 = tempin + (i0 * widthin + j1) * components;
	src10 = tempin + (i1 * widthin + j0) * components;
	src11 = tempin + (i1 * widthin + j1) * components;

	dst = tempout + (i * widthout + j) * components;

	for (k=0;k<components;k++) {
	  s1 = *src00++ * (1.0f-beta) + *src01++ * beta;
	  s2 = *src10++ * (1.0f-beta) + *src11++ * beta;
	  *dst++ = s1 * (1.0f-alpha) + s2 * alpha;
	}
      }
    }
  }
  else {
    /* shrink width and/or height:  use an unweighted box filter */
    GLint i0, i1;
    GLint j0, j1;
    GLint ii, jj;
    GLfloat sum, *dst;

    for (i=0;i<heightout;i++) {
      i0 = (int) i * (int) sy;
      i1 = i0 + 1;
      if (i1 >= heightin) i1 = heightin-1; 
      /*	 i1 = (i+1) * sy - EPSILON; */
      for (j=0;j<widthout;j++) {
	j0 = (int) j * (int) sx;
	j1 = j0 + 1;
	if (j1 >= widthin) j1 = widthin-1;
	/*	    j1 = (j+1) * sx - EPSILON; */

	dst = tempout + (i * widthout + j) * components;

	/* compute average of pixels in the rectangle (i0,j0)-(i1,j1) */
	for (k=0;k<components;k++) {
	  sum = 0.0;
	  for (ii=i0;ii<=i1;ii++) {
	    for (jj=j0;jj<=j1;jj++) {
	      sum += *(tempin + (ii * widthin + jj) * components + k);
	    }
	  }
	  sum /= (j1-j0+1) * (i1-i0+1);
	  *dst++ = sum;
	}
      }
    }
  }
#endif


  /*
   * Return output image
   */

  if (packrowlength>0) {
    rowlen = packrowlength;
  }
  else {
    rowlen = widthout;
  }
  if (sizeout >= packalignment) {
    rowstride = components * rowlen;
  }
  else {
    rowstride = packalignment/sizeout
      * CEILING( components * rowlen * sizeout, packalignment );
  }

  switch (typeout) {
  case GL_UNSIGNED_BYTE:
    k = 0;

    for (i=0;i<heightout;i++) {
      GLubyte *ubptr = (GLubyte *) dataout
	+ i * rowstride
	+ packskiprows * rowstride
	+ packskippixels * components;

      for (j=0;j<widthout*components;j++) {
		dummy(j, k+i);
		*ubptr++ = (GLubyte) tempout[k++];
      }
    }

    break;
  case GL_BYTE:
    k = 0;
    for (i=0;i<heightout;i++) {
      GLbyte *bptr = (GLbyte *) dataout
	+ i * rowstride
	+ packskiprows * rowstride
	+ packskippixels * components;
      for (j=0;j<widthout*components;j++) {
	dummy(j, k+i);
	*bptr++ = (GLbyte) tempout[k++];
      }
    }
    break;
  case GL_UNSIGNED_SHORT:
    k = 0;
    for (i=0;i<heightout;i++) {
      GLushort *usptr = (GLushort *) dataout
	+ i * rowstride
	+ packskiprows * rowstride
	+ packskippixels * components;
      for (j=0;j<widthout*components;j++) {
	dummy(j, k+i);
	*usptr++ = (GLushort) tempout[k++];
      }
    }
    break;
  case GL_SHORT:
    k = 0;
    for (i=0;i<heightout;i++) {
      GLshort *sptr = (GLshort *) dataout
	+ i * rowstride
	+ packskiprows * rowstride
	+ packskippixels * components;
      for (j=0;j<widthout*components;j++) {
	dummy(j, k+i);
	*sptr++ = (GLshort) tempout[k++];
      }
    }
    break;
/*	
  case GL_UNSIGNED_INT:
    k = 0;
    for (i=0;i<heightout;i++) {
      GLuint *uiptr = (GLuint *) dataout
	+ i * rowstride
	+ packskiprows * rowstride
	+ packskippixels * components;
      for (j=0;j<widthout*components;j++) {
	dummy(j, k+i);
	*uiptr++ = (GLuint) tempout[k++];
      }
    }
    break;
  case GL_INT:
    k = 0;
    for (i=0;i<heightout;i++) {
      GLint *iptr = (GLint *) dataout
	+ i * rowstride
	+ packskiprows * rowstride
	+ packskippixels * components;
      for (j=0;j<widthout*components;j++) {
	dummy(j, k+i);
	*iptr++ = (GLint) tempout[k++];
      }
    }
    break;
*/	
  case GL_FLOAT:
    k = 0;
    for (i=0;i<heightout;i++) {
      GLfloat *fptr = (GLfloat *) dataout
	+ i * rowstride
	+ packskiprows * rowstride
	+ packskippixels * components;
      for (j=0;j<widthout*components;j++) {
	dummy(j, k+i);
	*fptr++ = tempout[k++];
      }
    }
    break;
  default:
    return GLU_INVALID_ENUM;
  }


  /* free temporary image storage */
  free( tempin );
  free( tempout );

  return 0;
}



void gluPerspective( GLdouble fovy, GLdouble aspect,
                     GLdouble zNear, GLdouble zFar )
{
  GLdouble xmin, xmax, ymin, ymax;

  ymax = zNear * tan( fovy * M_PI / 360.0 );
  ymin = -ymax;

  xmin = ymin * aspect;
  xmax = ymax * aspect;

  glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}


void gluLookAt( GLdouble eyex, GLdouble eyey, GLdouble eyez,
                GLdouble centerx, GLdouble centery, GLdouble centerz,
                GLdouble upx, GLdouble upy, GLdouble upz )
{
  GLdouble m[16];
  GLdouble x[3], y[3], z[3];
  GLdouble mag;

  /* Make rotation matrix */

  /* Z vector */
  z[0] = eyex - centerx;
  z[1] = eyey - centery;
  z[2] = eyez - centerz;
  mag = sqrt( z[0]*z[0] + z[1]*z[1] + z[2]*z[2] );
  if (mag) {  /* mpichler, 19950515 */
    z[0] /= mag;
    z[1] /= mag;
    z[2] /= mag;
  }

  /* Y vector */
  y[0] = upx;
  y[1] = upy;
  y[2] = upz;

  /* X vector = Y cross Z */
  x[0] =  y[1]*z[2] - y[2]*z[1];
  x[1] = -y[0]*z[2] + y[2]*z[0];
  x[2] =  y[0]*z[1] - y[1]*z[0];

  /* Recompute Y = Z cross X */
  y[0] =  z[1]*x[2] - z[2]*x[1];
  y[1] = -z[0]*x[2] + z[2]*x[0];
  y[2] =  z[0]*x[1] - z[1]*x[0];

  /* mpichler, 19950515 */
  /* cross product gives area of parallelogram, which is < 1.0 for
   * non-perpendicular unit-length vectors; so normalize x, y here
   */

  mag = sqrt( x[0]*x[0] + x[1]*x[1] + x[2]*x[2] );
  if (mag) {
    x[0] /= mag;
    x[1] /= mag;
    x[2] /= mag;
  }

  mag = sqrt( y[0]*y[0] + y[1]*y[1] + y[2]*y[2] );
  if (mag) {
    y[0] /= mag;
    y[1] /= mag;
    y[2] /= mag;
  }

#define M(row,col)  m[col*4+row]
  M(0,0) = x[0];  M(0,1) = x[1];  M(0,2) = x[2];  M(0,3) = 0.0;
  M(1,0) = y[0];  M(1,1) = y[1];  M(1,2) = y[2];  M(1,3) = 0.0;
  M(2,0) = z[0];  M(2,1) = z[1];  M(2,2) = z[2];  M(2,3) = 0.0;
  M(3,0) = 0.0;   M(3,1) = 0.0;   M(3,2) = 0.0;   M(3,3) = 1.0;
#undef M
  glMultMatrixd( m );

  /* Translate Eye to Origin */
  glTranslatef( (float) -eyex, (float) -eyey, (float) -eyez );

}




GLint gluBuild2DMipmaps( GLenum target, GLint components,
			 GLint width, GLint height, GLenum format,
			 GLenum type, const void *data )
{
  GLint w, h, maxsize;
  void *image;
  GLint neww, newh, level, bpp;
  int error;

  if (width < 1 || height < 1)
    return GLU_INVALID_VALUE;

  glGetIntegerv( GL_MAX_TEXTURE_SIZE, &maxsize );

  w = round2( width );
  h = round2( height );

  while (width < maxsize)
    {
      maxsize = maxsize/2;
    }

  while (height < maxsize)
    {
      maxsize = maxsize/2;
    }

  if (w>maxsize) w = maxsize;
  if (h>maxsize) h = maxsize;


  bpp = bytes_per_pixel( format, type );
  
//  printf("gluBuild2DMipmaps bpp=%d w=%d h=%d fmt=%d type=%d\n", bpp, width, height, format, type);
  
  if (bpp==0) {
    /* probably a bad format or type enum */
    return GLU_INVALID_ENUM;
  }

  if (w!=width || h!=height) {
    /* must rescale image to get "top" mipmap texture image */
    image = malloc( (w+4) * h * bpp );
    if (!image) {
      return GLU_OUT_OF_MEMORY;
    }
    error = gluScaleImage( format, width, height, type, data,
			   w, h, type, image );
    if (error) {
      return error;
    }
  }
  else {
    image = (void *) data;
  }

  level = 0;
  while (1) {
	components = format; // Vladimir
    glTexImage2D( target, level, components, w, h, 0, format, type, image );


    if (w==1 && h==1)  break;

    neww = (w<2) ? 1 : w/2;
    newh = (h<2) ? 1 : h/2;

    error =  gluScaleImage( format, w, h, type, data,
			    neww, newh, type, image );
    if (error) {
      return error;
    }

    w = neww;
    h = newh;
    level++;
  }

  if (image!=data) {
    free( image );
  }

  return 0;
}







