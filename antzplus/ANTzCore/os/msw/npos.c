/* -----------------------------------------------------------------------------
*
*  msw/npos.c
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

/* -----------------------------------------------------------------------------
*
*  THERE are TWO separate npos.c files!!!
* 
*  THIS IS msw/npos.c and NOT posix/npos.c
*
* --------------------------------------------------------------------------- */

//avoid non-WIN32 environments trying to compile this file
#ifdef _WIN32

//zz alternate method to report an err
//
// throw a compiler err if trying to compile for wrong OS
// #ifdef _WIN32
//	#error 8686 - tried to compile "os/msw/npos.c" for wrong OS environmnet
// #endif

#include "../npos.h"		//single header used by all operating systems

#include "../../npdata.h"

#include <windows.h>
#include <process.h>
#include <io.h>
#include <wchar.h>
#include <conio.h>		//console io keys and interaction

//#include "wglext.h"
//#include "winbase.h"


//-----------------------------------------------------------------------------
void nposGetAppPath (char* buffer, int* size)
{
	static bool firstTimeCalled = true;
	static char appPath[kNPmaxPath];

	//if first time then set the app path using the CWD method
	//should replace with actual get app path OS function, zz debug
	if (firstTimeCalled)
	{
		nposGetCWD (appPath, size);
		firstTimeCalled = false;
	}
	
	//copy the app path to the passed in buffer
	strcpy (buffer, appPath);

	//set the length
	*size = strlen(buffer); 
}

// current working directory is OS specific
//-----------------------------------------------------------------------------
void nposGetCWD (char* buffer, int* size)
{
	int value = 0;
	int i = 0;
	DWORD length = 256;
	HINSTANCE hInstance = GetModuleHandle(NULL);
	*size = GetModuleFileNameA(hInstance, (LPCH)buffer, length);
//	char *chPtr = buffer;

//	*size *= 2;   // debug, ll	//debug, zz //stereo 3D

//	printf("Size is %d", (*size)/2); // debug, ll
	if (*size > 1)
		if (buffer[1] == 0)						// remove the spaces from UTF16,   debug zz
		{
			for(i = 0; 2 * i < *size; i++)
				buffer[i] = buffer[i*2];			// remove the spaces from UTF16,   debug zz

			*size /= 2;
		}

		// ll, Need to do some string processing, remove current running application filename from path
		for(i = (*size)-1; i >= 0; i--)
		{	
	//		printf("\nbuffer[%d] : %c\n", i, buffer[i]);

			if ( buffer[i] == '.' || buffer[i] == '\\' )
			{
	//			printf("Found %c", buffer[i]);
			
				if ( buffer[i] == '.' )
					buffer[i] = '\0';
				else
					break;

			}
			else
				buffer[i] = '\0';

		}
		// ll , End

	//	printf("File Path Size : %d\n", strlen(str)); 

	printf("CWD Path: ");
	for(i = 0; i < *size; i++)
		printf ("%c", buffer[i]);
	printf("\n");
}

//-----------------------------------------------------------------------------
void nposSetCWD (char* buffer)
{
	printf ("SetCWD: %d\n", SetCurrentDirectoryA((LPCSTR)buffer));
}

//-----------------------------------------------------------------------------
void nposGetOpenFilePath (char* buffer, int* size, void* dataRef)
{
//	getcwd (buffer, size);
	return;
}

//-----------------------------------------------------------------------------
void nposSetOpenFilePath (char* buffer, void* dataRef)
{
	return;
}


/*
//-----------------------------------------------------------------------------
FILE* nposFileDialog (const char* fileName, int dialogType, void* dataRef)
{

}
*/




/*
Todo :
XML Formatting
Delete Directory
Delete File
Get/Set File Attributes
Save / Save As File Name
*/
/*
void importFile(char *filename)
{
	char file[2000];
	int returnedValue;
	int index = 0;

	fopen(filename, "r");



	while( returnedValue = fgetc() != EOF )
	{
		file[index] = returnedValue;
		printf("%c", file[index]);
		index++;
	}

}
*/

//-----------------------------------------------------------------------------
int nposGetKey (void)
{
	return _getch();
}

//-----------------------------------------------------------------------------
ANTZCORE_API double nposGetTime(void)
{	
	LARGE_INTEGER frequency;		// ticks per second
	LARGE_INTEGER ticks;			// ticks
	double seconds;
	
	// get ticks per second
	QueryPerformanceFrequency(&frequency);
	
	// get the count in ticks
	QueryPerformanceCounter(&ticks);
	
	// convert ticks to seconds
	if (frequency.QuadPart)
		seconds = (double) ((double)ticks.QuadPart / (double)frequency.QuadPart);

	return seconds;
}

//-----------------------------------------------------------------------------
ANTZCORE_API void nposUpdateTime(void* dataRef)
{
	pData data = (pData) dataRef;
	
	LARGE_INTEGER frequency;		// ticks per second
	LARGE_INTEGER ticks;			// ticks
	double seconds;
	
	// get ticks per second
	QueryPerformanceFrequency(&frequency);
	
	// get the count in ticks
	QueryPerformanceCounter(&ticks);
	
	// convert ticks to seconds
	if (frequency.QuadPart)
		seconds = (double) ((double)ticks.QuadPart / (double)frequency.QuadPart);

	if (data->io.timeStart == 0.0)
		data->io.timeStart = seconds;
	else
		data->io.cycleDelta = seconds - data->io.time;
	
	data->io.time = seconds;
}

//-----------------------------------------------------------------------------
ANTZCORE_API void nposSleep(double seconds)
{
	Sleep((int)(seconds * 1000.0));
}


//------------------------------------------------------------------------------
void nposTimeStampCSV (char* fileName)
{
	SYSTEMTIME time;

	GetLocalTime(&time);
	sprintf (fileName, "antz%02d%02d%02d%02d%02d%02d.csv",
			(time.wYear - 2000),	//we subtract 2000 to just get the last 2 digits
			time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
}

//zz debug, add support for platform specific GL extension methods
//------------------------------------------------------------------------------
bool WGLExtensionSupported (const char *extension_name)
{
/*  
	http://stackoverflow.com/questions/589064/how-to-enable-vertical-sync-in-opengl
	
	// this is pointer to function which returns pointer to string with list of all wgl extensions
    PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;

    // determine pointer to wglGetExtensionsStringEXT function
    _wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) wglGetProcAddress("wglGetExtensionsStringEXT");

    if (strstr(_wglGetExtensionsString(), extension_name) == NULL)
    {
        // string was not found
        return false;
    }
*/
    // extension is supported 
	return true;
}

//zz-JJ---------------------------------------------------------------------------
void nposBeginThread (voidFnPtr vfp, void *dataRef)
{
	_beginthread (vfp, 0, dataRef);
}


//zz-JJ---------------------------------------------------------------------------
void nposEndThread (void)
{
	_endthread();
}

bool nposSupportsAntzThreads (void)
{
	return 1;
}

//JJ-end

#endif


