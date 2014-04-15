/* -----------------------------------------------------------------------------
*
*  npctrl.h
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

#ifndef npctrl_H_
#define npctrl_H_

#include "ANTzCore.h"

#ifdef __cplusplus
	extern "C" {
#endif

/* -----------------------------------------------------------------------------
*
* input event command mapping to target context and flow control by npctrl.h
* control command actions processed by npcmd.h
*
* --------------------------------------------------------------------------- */

//------------------------------------------------------------------------------

ANTZCORE_API void npInitCtrl(void* dataRef);

ANTZCORE_API void npCloseCtrl(void* dataRef);

ANTZCORE_API void npUpdateCtrl(void* dataRef);					//typically once per frame

ANTZCORE_API void npCtrlCommand(int command, void* dataRef);   //apply to all selected nodes

void npCtrlCommandSet (int command, void* node, void* dataRef);	// specific node

void npCmdOpen( char* filePath, void* dataRef);

#ifdef __cplusplus
	}
#endif
#endif

