/* -----------------------------------------------------------------------------
*
*  npstr.h
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

#ifndef NPSTR_H_
#define NPSTR_H_

#include "../npdata.h"

//--- atoi and atof optimized for antz ----------------------------------------
int npatoi (const char *p);				//32bit int
float npatof (const char *p);			//32bit float

long npatol (const char *p);			//64bit long is akin to atoi
double npatod (const char *p);			//64bit double is akin to atof

//based on strtod, but sets buffer to endPtr + 1
//designed to increment past the comma to next field for CSV processing
//does not increment past characters other then comma, ie stops at line returns
int	npstrtoi (char** buffer);			//32bit int
float npstrtof (char** buffer);			//32bit float

long npstrtol (char** buffer);			//64bit long is akin to strtol
double npstrtod (char** buffer);		//64bit double is akin to strtod
	
// copy a string to a new memory location and return a pointer to the new copy
char* npNewStrcpy (const char* str, void* dataRef);							

int npSeekToNextField (const char* buffer);	//zz64 need to change to long int
int npSeekToNextLine (const char* buffer);
int npSeekToNextLineLimit (const char* buffer, int size);
int npSeekLastEOL(const char* buffer, int size);

int npStrDigitIsNext( const char* str, int size );

#endif

