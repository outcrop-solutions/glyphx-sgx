/* -----------------------------------------------------------------------------
*
*  npfile.c
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

#include "npfile.h"

#include "npch.h"
#include "npconsole.h"
#include "../os/npos.h"
#include "../npctrl.h"
#include "../io/npgl.h"		//used for npPostTool()						zz, debug
#include "../io/gl/nptags.h"

//-----------------------------------------------------------------------------
FILE* npGetFileRef (int index)
{
	static FILE* file[kNPfileMax];
	static FILE* fileCount = 0;

	return file[index];
}


//-----------------------------------------------------------------------------
ANTZCORE_API void npInitFile(void* dataRef)
{
	int result = 0;

	// builds soft-coded defualt routing maps using mapItem descriptors
	npMapTypeInit(dataRef);									//zz debug, move to npmap.c

	result = npOpenGlobalsCSV ("antzglobals.csv", 1, 15, dataRef);

	//if no globals csv file then create one
	if (!result)
		npSaveMapToCSV ("antzglobals.csv", kNPtableGlobals, dataRef);

//	result = npOpenMapCSV ("antzglobals.csv", kNPtableMapOSC, dataRef);

	//if no globals csv file then create one
	if (!result)
		npSaveMapToCSV ("antzosc.csv", kNPtableMapOSC, dataRef);

	return;
}

//-----------------------------------------------------------------------------
ANTZCORE_API void npCloseFile(void* dataRef)
{
//	terminate all file io threads gracefully then kill if needed
//	iterate through and close all open files
	return;
}

//-----------------------------------------------------------------------------
// mode usage: 3 parameters "r/w/a" followed by optional "b" and optional "+"
// example mode = "rb+"
// r open file for reading
// w open file for overwriting. Create new file if doesn't already exist
// a open file for appending data. 
// b open file as a binary file
// + open file for read and writing
//-----------------------------------------------------------------------------
FILE* npFileOpen( const char* fileName, const char* mode, void* dataRef )
{
	int length = 0;

	FILE* file = NULL;
	char* msg = NULL;

	// ascert fileName is valid string
	length = strnlen( fileName, 261 );
	if(length < 1 || length >= 260 )
	{
		npPostMsg( "warn 7249 - invalid fileName", kNPmsgFile, dataRef );
		return NULL;
	}

	// open the file based on mode passed in
	file = fopen( fileName, mode ); 

	if ( file == NULL )
	{
		msg = (char*) npMalloc( 0, 512, dataRef );
		sprintf( msg, "warn 7888 - could not open file: %.261s", fileName );
		npPostMsg( msg, kNPmsgFile, dataRef );
		npFree( msg,dataRef );
		return NULL;
	}

	// add code to add the file ptr to the data->map							debug, zz

	return file;
}

//-----------------------------------------------------------------------------
FILE* npFileNew (const char* fileName, const char* mode, void* dataRef)
{
	char* file[1024];

	if (fileName == NULL)
	{
		*file = "antz0001.csv";
	// get the 
	// get CWD
	// check if "antz0001.csv" exists then increment until
	// put CWD together with fileName
	//	npFileDialogNew ("antz0001.csv", );
	}

	return npFileOpen (fileName, "w+", dataRef);
}
//-----------------------------------------------------------------------------

// if not saved then opens SaveAs dialog before reseting app
int npFileClose (FILE* file, void* dataRef)
{
	int result = 0;
	
	result = fclose (file);

	if (result)
		printf(" err 4286 - file close failed, result: %d\n", result);

	return result;
}


/*
* The total number of elements successfully read is returned.
* If this number differs from the count parameter, either a reading err occurred
* or the end-of-file was reached while reading. In both cases, the proper 
* indicator is set, which can be checked with ferror and feof, respectively.
* If either size or count is zero, the function returns zero and both the 
* stream state and the content pointed by ptr remain unchanged.
*/
//-----------------------------------------------------------------------------
int npFileRead (void* readBuffer, int elementSize, int elementCount, 
				 FILE* file, void* dataRef)
{
	int count = 0;

	count = fread (readBuffer, elementSize, elementCount, file);
	
	if ( count <= 0 )
		printf(" warn 7218 - zero bytes read\n", count);

	return count;
}

//-----------------------------------------------------------------------------
void npFileRewind (FILE* file)
{
	rewind(file);
}

//-----------------------------------------------------------------------------
int npFileSeek (FILE* file, long offSet, int origin)
{
	// fseek(pFile, 0, SEEK_END);
	
	return fseek(file, offSet, origin);
}

//-----------------------------------------------------------------------------
long npFileTell (FILE* file)
{
	return ftell(file);			// returns the file index position
}


//-----------------------------------------------------------------------------
int npFileWrite (const void* str, int wordSize, int size, FILE* file, void* dataRef)
{
	if (size <= 0)
		return 0;

	return fwrite( str, wordSize, size, file );

	//return fwrite( str, 1, strlen(str), file );
}


//-----------------------------------------------------------------------------
void npGetMap (int mapType, int format, void* dataRef)
{
	return;
}


//-----------------------------------------------------------------------------
void npSave (FILE* file, void* dataRef)
{
//	FILE* file = NULL;

	if (file != NULL)
	{
		// save current state to open file
		// default file is compatible with csv, tdt, xml, and json
	}
	// create new default file and save state
	else 
	{
//		file = npFileNew ((const char*)"antz0001.xml", "w+", dataRef);
//		npSaveCSV (npGetMap (kNPmapAll, kNPtypeCSV, dataRef), dataRef);
	}
}



//-----------------------------------------------------------------------------
void npSaveAs (int size, void* dataRef)
{
//	char* filePath = NULL;

	size = 16;	// strlength 

//	file = npGetOpenFilePath(file, size, dataRef);

	//npFileDialog (NULL, kNPfileDialogSaveAs, dataRef);

	// if file invalid call err dialog
	// save current state to file named by user
	// if quitting close file
	// else, 
	// open new copy of file and increment the file name 

}

//-----------------------------------------------------------------------------
void npFileImport (const char* fileName, void* dataRef)
{
	return;
}

//-----------------------------------------------------------------------------
void npFileExport (int size, void* dataRef)
{
	return;
}


//-----------------------------------------------------------------------------
// file and directory management
// current working directory is OS specific
//-----------------------------------------------------------------------------
void npGetOpenFilePath (char* buffer, int* size, void* dataRef)
{
	return;
}

//-----------------------------------------------------------------------------
void npGetCWD (char* buffer, int* size)					
{	
	nposGetCWD (buffer, size);
	
	//size = strlen(buffer);
}

//-----------------------------------------------------------------------------
void npMakeDir (int size, void* dataRef)
{
	return;
}

//-----------------------------------------------------------------------------
void npDelDir (int size, void* dataRef)
{
	return;
}

//-----------------------------------------------------------------------------
void npListDir (int size, void* dataRef)
{
	return;
}

//-----------------------------------------------------------------------------
void npGetFileAttrib (int size, void* dataRef)
{
	return;
}

//-----------------------------------------------------------------------------
void npSetFileAttrib (int size, void* dataRef)
{
	return;
}


//-----------------------------------------------------------------------------
void npOpenURL (const char* command, void* dataRef)
{
	pData data = (pData) dataRef;

	char* str = (char*)malloc(8192);	//generally 2048 is max URL length

	// compose string using the recordID & URL passed in from the command line
	// or the hard coded default in npdata.c,
	// MSW uses 'start' to call browser
	sprintf (str, "start %s?id=%d", data->io.url, data->map.currentNode->recordID);

	// call browser with the composed URL
	system (str);

	free (str);
} 

//-----------------------------------------------------------------------------
void npOpenApp (const char* command, void* dataRef)
{
	return;
}

//-----------------------------------------------------------------------------
void npOpenAntz (const char* command, void* dataRef)
{
	return;
}

//zz-JJ
// track metadata, sample rate, data type, etc...
//------------------------------------------------------------------------------
void npFileOpenChMeta (const char* filePath, void* dataRef)
{
	npLoadChMeta (filePath, dataRef);
}

//------------------------------------------------------------------------------
void npFileOpenChTracks (const char* filePath, void* dataRef)
{
	npLoadChTracks (filePath, dataRef);
}

//------------------------------------------------------------------------------
void npFileOpenChMap (const char* filePath, void* dataRef)
{
	npLoadChMap (filePath, dataRef);
}

//------------------------------------------------------------------------------
void npChSetBuildPath(char* path, char* csvPath, char* filePrefix, 
					  char* setNumber, char* extension)
{
	strcpy (path, csvPath);
	strcat (path, filePrefix);
	strcat (path, setNumber);
	strcat (path, extension);
}

//------------------------------------------------------------------------------
void npFileOpenChSet (const char* filePath, void* dataRef)
{
	char path[kNPmaxPath];
	char setNumberCharacters[5];

	pData data = (pData) dataRef;
	sprintf (&setNumberCharacters[0], "%04d", data->io.ch.setNumberToLoad);		//zz debug 

	npChSetBuildPath(path, data->io.file.csvPath, "antzchmeta", &setNumberCharacters[0], ".csv");	//zz debug 
	npFileOpenChMeta (path, dataRef);			//metadata

	npChSetBuildPath(path, data->io.file.csvPath, "antzch", &setNumberCharacters[0], ".csv");
	npFileOpenChTracks (path, dataRef);			//tracks

	npChSetBuildPath(path, data->io.file.csvPath, "antzchmap", &setNumberCharacters[0], ".csv");
	npFileOpenChMap (path, dataRef);			//ch mapping

	//subscribe existing nodes with channels to the channel map
	npSyncChMap (dataRef);
}
//zz-JJ-End

