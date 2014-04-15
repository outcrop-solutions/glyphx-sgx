/* -----------------------------------------------------------------------------
*
*  nppio.c
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

#include "npio.h"

#include "io/npfile.h"
#include "io/npch.h"
#include "npmouse.h"
#include "io/npconsole.h"
#include "io/npdb.h"
//#include "io/net/nposc.h"


//-----------------------------------------------------------------------------
void npInitIO (void* dataRef)
{

	//init the local IO devices
//	npInitKeyboard (dataRef);		//zz
	npInitMouse (dataRef);
//	npInitSerial (dataRef);			//zz support terminal based system boot-up

	//launch file services and updates hard-coded global variables from file
	npInitFile (dataRef);

	//IO channels use both local drives and networking
	npInitCh (dataRef);													//zz-JJ

	//audio video input and output
//  npInitAV (dataRef);				//zz

	//start network connections which in turn can further update init state
	//npInitOSC (dataRef);			//zz-osc

	//npInitDB (dataRef);				//zzsql
}


//-----------------------------------------------------------------------------
void npCloseIO (void* dataRef)
{
	npCloseFile (dataRef);
	npCloseMouse (dataRef);
	npCloseCh (dataRef);	//zz-JJ
}

//view tree of files catalogued
//------------------------------------------------------------------------------
void npViewFiles (void* dataRef)
{
	FILE* file = NULL;

	pData data = (pData) dataRef;

	//display names and paths of indexed files

	//let user find and choose the files
//	npFileBrowser (dataRef);
}

//------------------------------------------------------------------------------
void npConsole (void* dataRef)
{
	pData data = (pData) dataRef;
	pNPconsole console = &data->io.gl.hud.console;

	//activate command console
	npConsoleCmd( console, dataRef );
}

