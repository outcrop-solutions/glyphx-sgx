/* -----------------------------------------------------------------------------
*
*  npmap.h
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

#ifndef NPMAP_H_
#define NPMAP_H_

#include "../npdata.h"


//--- Functions ---------------------------------------------------------------

void npInitMap (void* dataRef);
void npCloseMap (void* dataRef);

void* npMapNew (int mapType, void* dataRef);
void npMapDelete (NPmapTypePtr map, void* dataRef);

void npGetMaps (void* dataRef);

void npGetMapIO (void* dataRef); //Ports, GL (Sort Visible, Sort-Z, screen 1/2/3 )
void npGetMapNPE (void* dataRef); // collision, rotation, translation...

ANTZCORE_API int npGetRootIndex(pNPnode node, void* dataRef);
ANTZCORE_API void npSelectNode(pNPnode node, void* dataRef);

void* npGetNodeByID (int id, void* dataRef);
void npSelectNodeByID (int id, void* dataRef);

void npTraverseTree (void (*nodeFunc)(pNPnode node, void* dataRef), 
					 pNPnode node, void* dataRef);
void npTraverseMap (void (*nodeFunc)(pNPnode node, void* dataRef),
				   void* dataRef);

//------------------------------------------------------------------------------
//zzhp
void npInitMalloc(void* dataRef);
void npCloseMalloc(void* dataRef);
void npUpdateMalloc(void* dataRef);
void* npMalloc(int type, int size, void* dataRef);
void* npCalloc(int type, int size, void* dataRef);
void npFree(void* memory, void* dataRef);

//moved from npmapfile, reconcile with methods above
void npMapSortInit (void** mapRef, void* dataRef);
void npMapSort(void* dataRef);
void* npMapNodeAdd (int id, int type, int branchLevel, int parentID, 
					 int format, void* dataRef);

void npSort (void* base, int count, int elementSize, 
			 int(*pCompareFunc)(const void* a, const void* b), void* dataRef );

pNPrecordTag npNewTag(void* dataRef);
void npTagSort (void* dataRef);
void npTagSortAdd (pNPrecordTag recordTag, void* dataRef);

//zz-q
void npSetMapItem( char* mapPath, char* item, char* element, char* typeTag,
				  char* value, void* dataRef );

#endif

