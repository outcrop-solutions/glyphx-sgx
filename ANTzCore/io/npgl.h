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

#ifndef NPGL_H_
#define NPGL_H_

#include "../ANTzCore.h"

#include "../npdata.h"

#include "gl/npgldraw.h"
#include "gl/npgeometry.h"
#include "gl/npcolor.h"

#ifdef __cplusplus
extern "C" {
#endif

//MB//
//--- Data -----
int selectedID;
//MB-end//

//--- Functions ----------------------------------------------------------------

ANTZCORE_API void npInitGL(void* dataRef);
ANTZCORE_API void npCloseGL(void* dataRef);

ANTZCORE_API void npSetLookAtFromCamera(void* dataRef);

ANTZCORE_API void npGLDrawScene(void* dataRef);

ANTZCORE_API void npGLResizeScene(int width, int height);

ANTZCORE_API void npPick(int x, int y, void* dataRef);
ANTZCORE_API int npPickPin(int x, int y, void* dataRef);

void npResizeConsole (void* dataRef);

void npPostMode (pNPnode node, void* dataRef);
ANTZCORE_API void npPostTool(pNPnode node, void* dataRef);

ANTZCORE_API void npPickTool(pNPnode node, void* dataRef);

ANTZCORE_API void npGLLighting (void* dataRef);
ANTZCORE_API void npGLShading(void* dataRef);

#ifdef __cplusplus
}
#endif

#endif

