/* -----------------------------------------------------------------------------
*
*  npfile.h
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

#ifndef NPFILE_H_
#define NPFILE_H_

#include "../ANTzCore.h"

#include "../npdata.h"
#include "../data/npmapfile.h"
#include "file/npcsv.h"

#ifdef __cplusplus
extern "C" {
#endif

#define kNPfileMax 4096

//------------------------------------------------------------------------------
ANTZCORE_API void npInitFile(void* dataRef);	// init the file io system
ANTZCORE_API void npCloseFile(void* dataRef);	// for init and shutdown, not closing a file

// these functions wrap the standard C file methods on Linux, MSW and OSX
// opens or creates the file in the specified mode "rb+" read/write binary..
FILE* npFileOpen (const char* fileName, const char* mode, void* dataRef);
FILE* npFileNew (const char* fileName, const char* mode, void* dataRef);


// if not saved then opens SaveAs dialog before reseting app
int npFileClose (FILE* file, void* dataRef);

// wrappers for fread(), fwrite() and rewind()
//void npFileRead (void* writeBuffer, int wordSize, int size, void* dataRef);
int npFileRead (void* readBuffer, int elementSize, int elementCount, 
				 FILE* file, void* dataRef);
int npFileWrite (const void* str, int wordSize, int size, FILE* file, void* dataRef);

void npFileRewind (FILE* file);
int npFileSeek (FILE* file, long offSet, int origin);
long npFileTell (FILE* file);

// 
void npSave (FILE* file, void* dataRef);
void npSaveAs (int size, void* dataRef);

void npImport (const char* fileName, void* dataRef);
void npExport (int size, void* dataRef);

// file and directory management

void npGetOpenFilePath (char* buffer, int* size, void* dataRef);
void npGetCWD (char* buffer, int* size);			//get Current Working Directory

void npListDir (int size, void* dataRef);

void npMakeDir (int size, void* dataRef);
void npDelDir (int size, void* dataRef);

void npGetFileAttrib (int size, void* dataRef);
void npSetFileAttrib (int size, void* dataRef);

void npOpenURL (const char* url, void* dataRef);
void npOpenApp (const char* fileName, void* dataRef);
void npOpenAntz (const char* fileName, void* dataRef);



//zz-JJ
//-----------------------------------------------------------------------------
//loads ANTzCh(tracks), ANTzChMeta(metadata) and ANTzChMap (track to atrrib)
//loads file set based on name, ie antzch000x.csv also loads antzchmap000x.csv
void npFileOpenChSet (const char* filePath, void* dataRef);

void npFileOpenChMeta (const char* filePath, void* dataRef);
void npFileOpenChTracks (const char* filePath, void* dataRef);
void npFileOpenChMap (const char* filePath, void* dataRef);

//closes the file set, typically antzch000x.csv would be the only open file
void npFileCloseChTracks (const char* filePath, void* dataRef);

#ifdef __cplusplus
}
#endif

#endif

