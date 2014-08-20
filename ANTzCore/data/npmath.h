/* -----------------------------------------------------------------------------
*
*  npmath.h
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

#ifndef NPMATH_H_
#define NPMATH_H_

#include "../ANTzCore.h"
#include "../npdata.h"

#ifdef __cplusplus
extern "C" {
#endif

//--- Functions ---------------------------------------------------------------

void npCopyMatrix (float dest[], float src[]);

void npMultMatrix (float *matDest, float *matA, float *matB);

void npLocalToWorld (NPfloatXYZ *world, const float *inverseCam, const float *modelView);

ANTZCORE_API bool npInvertMatrixf (const float m[16], float invOut[16]);

#ifdef __cplusplus
}
#endif

#endif

