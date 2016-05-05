/* -----------------------------------------------------------------------------
*
*  npgeometry.h
*
*  ANTz - realtime 3D data visualization tools for the real-world, based on NPE.
*
*  ANTz is hosted at http://openantz.com and NPE at http://neuralphysics.org
*
*  Written in 2010-2013 by Shane Saxon - contact@saxondigital.net
*
*  Please see main.c for a complete list of additional code contributors.
*
*  To the extent possible under law, the author(s) have dedicated all copyright 
*  and related and neighboring rights to this software to the public domain
*  worldwide. This software is distributed without any warranty.
*
*  Released under the CC0 license, which is GPL compatible.
*
*  You should have received a copy of the CC0 Public Domain Dedication along
*  with this software (license file named COPYING.txt). If not, see
*  http://creativecommons.org/publicdomain/zero/1.0/
*
* --------------------------------------------------------------------------- */

#ifndef NPGLSURFACE_H_
#define NPGLSURFACE_H_


#include "../../npdata.h"

#ifdef WIN32
#include <Windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#ifdef __cplusplus
extern "C" {
#endif

ANTZCORE_API void npInitGLPrimitive(void* dataRef);
void npCloseGLPrimitive (void* dataRef);

void npGLSurface (bool texture, pNPnode node, void* dataRef);

ANTZCORE_API void npGLPrimitive(int geometry, float ratio);

void npGLTexture(pNPnode node, void* dataRef);

void npGLSolidSphere(GLdouble radius, GLint slices, GLint stacks);
void npGLWireSphere(GLdouble radius, GLint slices, GLint stacks);
void npGLSolidCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
void npGLWireCone(GLdouble base, GLdouble height, GLint slices, GLint stacks);
void npGLSolidTorus(GLdouble innerRadius, GLdouble outerRadius,
    GLint slices, GLint stacks);
void npGLWireTorus(GLdouble innerRadius, GLdouble outerRadius,
    GLint slices, GLint stacks);

void npGlutPrimitive(int primitive);

void npWireCube(GLdouble size);
void npSolidCube(GLdouble size);

void drawSphere(GLdouble radius, GLint slices, GLint stacks, GLenum drawStyle);

void drawBox(GLfloat size, GLenum type);
void drawTorus(GLfloat r, GLfloat R, GLint nsides, GLint rings);

void npWireDodecahedron();
void npSolidDodecahedron();
void npWireOctahedron();
void npSolidOctahedron();
void npWireIcosahedron();
void npSolidIcosahedron();
void npWireTetrahedron();
void npSolidTetrahedron();

#ifdef __cplusplus
}
#endif

#endif
