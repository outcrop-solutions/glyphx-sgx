/* -----------------------------------------------------------------------------
*
*  nposx.h
*
*  ANTz - realtime 3D data visualization tools for the real-world, based on NPE.
*
*  ANTz is hosted at http://openantz.com and NPE at http://neuralphysics.org
*
*  Written in 2010-2014 by Shane Saxon - saxon@openantz.com
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

#ifndef NPOSX_H_
#define NPOSX_H_

#include "../../npdata.h"

void* nposLoadLibrary( char* filePath );
void* nposGetLibSymbol(void* library, char* symbolName); // New function, lde

void* nposxGetLibSymbol(void* library, char* symbolName); // New function, lde

void npOSXGetCWD (char* buffer, int* size);

void npOSXUpdateTime (void* dataRef);
void npOSXSleep (double seconds);

FILE* npOSXFileDialog (const char* fileName, int dialogType, void* dataRef);

void npOSXTimeStampCSV (char* fileName);

void npOSXSetCursorPos (int x, int y);
void npOSXShowCursor (int hide);

//bool nposSupportsAntzThreads (void); // lde
void nposEndThread (void); // lde
void nposBeginThread (voidFnPtr vfp, void *dataRef); // lde

FILE* openFileDialog (const char* fileName, int dialogType, void* dataRef);

int nposGetKey (void); // lde

#endif

