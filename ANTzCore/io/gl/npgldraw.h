/* -----------------------------------------------------------------------------
*
*  npgldraw.h
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

#ifndef NPGLDRAW_H_
#define NPGLDRAW_H_

#include "../../ANTzCore.h"

#include "../../npdata.h"

//------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

void npInitGLDraw (void* dataRef);
void npCloseGLDraw (void* dataRef);

ANTZCORE_API void npDrawNodes(void* dataRef);

void SetIndexColor (NPubyteRGBA *color, int *colorIndex);

ANTZCORE_API void npIDtoRGB(int id, int* r, int* g, int* b);			//MB-PICK
ANTZCORE_API int npRGBtoID(int r, int g, int b);						//MB-PICK

float npDistance (NPfloatXYZ vecA, NPfloatXYZ vecB);		//MB-TRANS

ANTZCORE_API void npDrawSelect(void* dataRef);

#ifdef __cplusplus
}
#endif

#endif