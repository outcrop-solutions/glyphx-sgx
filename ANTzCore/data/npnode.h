/* -----------------------------------------------------------------------------
*
*  npnode.h
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

#ifndef npnode_H_
#define npnode_H_


#include "../npdata.h"

#ifdef __cplusplus
extern "C" {
#endif

//--- Functions ---------------------------------------------------------------

void npInitNode (void* dataRef);
void npCloseNode (void* dataRef);

void npInitNodeDefault (pNPnode node);
void npInitNodeCamera (pNPnode node);

int	npNewNodeID();

ANTZCORE_API pNPnode npNodeNew(int nodeType, pNPnode nodeParent, void* dataRef);
ANTZCORE_API pNPnode npNodeNewLink(pNPnode linkA, pNPnode linkB, void* dataRef);
ANTZCORE_API bool npNodeAttach(pNPnode node, pNPnode parent, void* dataRef);
ANTZCORE_API void npNodeDelete(pNPnode node, void* dataRef);
ANTZCORE_API void npNodeRemove(bool freeNode, pNPnode node, void* dataRef);

void npSetTagOffset (pNPnode node);

ANTZCORE_API pNPnode npGetActiveCam(void* dataRef);

#ifdef __cplusplus
}
#endif

#endif

