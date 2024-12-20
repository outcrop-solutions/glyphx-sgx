/* -----------------------------------------------------------------------------
*
*  npmapfile.c
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

#include "npmapfile.h"


#include "../npdata.h"
#include "../data/npnode.h"
#include "../os/npos.h"
#include "../io/npfile.h"
#include "../io/npch.h"

#include "../io/gl/nptags.h"	//move the necessary functions to npdata, debug zz


void npLoadChannelFile (char* buffer, int wordSize, int size, void* dataRef);


//-----------------------------------------------------------------------------
void npInitMapFile (void* dataRef)
{
	return;
}

//-----------------------------------------------------------------------------
void npCloseMapFile (void* dataRef)
{
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
FILE* npOpenMapFile (const char* fileName, const char* mode, void* dataRef)
{
	FILE* file = NULL;

	file = fopen(fileName, mode); 

	if ( file == NULL )
	{
		printf("err 4244 - failed to open file\n");
		return NULL;
	}

	// add code to add the file ptr to the data->map							debug, zz

	return file;
}

//-----------------------------------------------------------------------------
FILE* npNewMapFile (const char* fileName, const char* mode, void* dataRef)
{
	return NULL;
}


//-----------------------------------------------------------------------------
int npReadMapFile (void* readBuffer, int elementSize, int elementCount, 
				 FILE* file, void* dataRef)
{
	size_t bytes = 0;

	bytes = fread (readBuffer, elementSize, elementCount, file);
	
	if ( bytes == 0 )
		printf("Failed Reading\n");
	else
		printf("Success Reading\n");

	return (int)bytes;
}


//-----------------------------------------------------------------------------
FILE* npDialogMapFile (const char* fileName, int dialogType, void* dataRef)
{	
	
	switch (dialogType)
	{
		case kNPfileDialogNew : ; break;

		case kNPfileDialogOpen : ; break;
		case kNPfileDialogClose : ; break;

		case kNPfileDialogSave : ; break;
		case kNPfileDialogSaveAs : ; break;

		case kNPfileDialogImport : ; break;
		case kNPfileDialogExport : ; break;

		default : break;
	}

	return NULL;
}


//-----------------------------------------------------------------------------
int npWriteMapFile (const void* str, int wordSize, int size, FILE* file, void* dataRef)
{
	return (int)fwrite( str, wordSize, strlen(str), file );
}


void npSaveMapFile (FILE* file, void* dataRef)
{
	return;
}


//-----------------------------------------------------------------------------
int npImportMapFile (FILE* file, int type, void* dataRef)
{
	int size = 0;
	return size;
}


//-----------------------------------------------------------------------------
// write the header structure
int npWriteMapHeader (char* bufferIndex, int count, int type, void* dataRef)
{
	int i = 0;
	int itemCount = 0;

	NPmapTypePtr map = NULL;

	pData data = (pData) dataRef;
	
	map = data->map.typeMapNode;		//debug, zz

	if (bufferIndex == NULL || count < 5)
		return 0;

	//set map and item count based on type, could be passed in, debug zz
	switch (type)
	{
		case kNPnode: 
			map = data->map.typeMapNode;
			itemCount = kNPnodeItemCount;
			break;
		case kNPcamera :
			map = data->map.typeMapCamera;
			itemCount = kNPcameraItemCount;
			break;
		case kNPgrid :
			map = data->map.typeMapGrid;
			itemCount = kNPgridItemCount;
			break;
		case kNPpin :
			map = data->map.typeMapPin;
			itemCount = kNPpinItemCount;
			break;
		default : 
			printf ("err 4381 - unknown type, cant write header field names");
			return 0;	
			break;
	}

	count = 0;
	for (i=0; i < itemCount; i++)
	{
		// insert a comma for CSV and JSON files
		//first field name to lead without a comma one without comma
		if (i > 0)
			count += sprintf ((bufferIndex + count), ",");

		switch (map[i].type)
		{
			case 0 :				// same as default
				count += sprintf ((bufferIndex + count), "%s", map[i].name );
				break;
			case kNPfloatXYZ : 
				count += sprintf ((bufferIndex + count), "%s_x,%s_y,%s_z",
									map[i].name, map[i].name, map[i].name );
				break;
			case kNPintXYZ : 
				count += sprintf ((bufferIndex + count), "%s_x,%s_y,%s_z",
									map[i].name, map[i].name, map[i].name );
				break;
			case kNPboolXYZ : 
				count += sprintf ((bufferIndex + count), "%s_x,%s_y,%s_z",
									map[i].name, map[i].name, map[i].name );	
				break;
			case kNPfloatXYZA : 
				count += sprintf ((bufferIndex + count), "%s_x,%s_y,%s_z,%s_a",
						map[i].name, map[i].name, map[i].name, map[i].name );
			case kNPfloatXYZS : 
				count += sprintf ((bufferIndex + count), "%s_x,%s_y,%s_z,%s_s",
						map[i].name, map[i].name, map[i].name, map[i].name );
				break;
			case kNPboolXYZS : 
				count += sprintf ((bufferIndex + count), "%s_x,%s_y,%s_z,%s_s",
						map[i].name, map[i].name, map[i].name, map[i].name );
				break;
			case kNPubyteRGBA : 
				count += sprintf ((bufferIndex + count), "%s_r,%s_g,%s_b,%s_a",
						map[i].name, map[i].name, map[i].name, map[i].name );
				break;
			case kNPubyteRGB : 
				count += sprintf ((bufferIndex + count), "%s_r,%s_g,%s_b",
									map[i].name, map[i].name, map[i].name );
				break;
			case kNPfloatRGBA : 
				count += sprintf ((bufferIndex + count), "%s_r,%s_g,%s_b,%s_a",
						map[i].name, map[i].name, map[i].name, map[i].name );
				break;
			case kNPfloatRGB : 
				count += sprintf ((bufferIndex + count), "%s_r,%s_g,%s_b", 
									map[i].name, map[i].name, map[i].name );
				break;

			default :				// same as case 0
				count += sprintf ((bufferIndex + count), "%s", map[i].name );
				break;
		}
	}

	count += sprintf ((bufferIndex + count), "\n");

	return count;
}


//-----------------------------------------------------------------------------
int npMapNodesToCSV (char* buffer, int size, int* rootIndex, void* dataRef)
{
	int i = 0, rootCount = 0, count = 0;

	pNPnode node = NULL;
	pData data = (pData) dataRef;

//	char* nodePtr = data->io.write;					//zz debug, pull this out from npWriteNode()

	data->io.write = buffer;

	//write the header only if it is the start of the root node array
	if (*rootIndex == 0)
	{
		count += npWriteMapHeader (buffer, size, kNPnode, dataRef);
		data->io.write += count;
	}

	// iterate through data map and write out each node
	//for (i=0; i < data->map.nodeRootCount; i++)
	rootCount = data->map.nodeRootCount;

	for (i = *rootIndex; i < rootCount; i++)
	{
		node = data->map.node[i];

		if (node->type != kNodeHUD )				//zz debug 
		{	
			//zzf
			if ( data->io.file.saveSelect )
			{
				if ( node->selected )
				{
					count += npWriteNode ((buffer + count), node, kNPmapNodeCSV, data);

					if (node->childCount)
						count += npMapTraverseTree ((buffer + count), node, kNPmapNodeCSV, data);
				}
			}
			else
			{
				count += npWriteNode ((buffer + count), node, kNPmapNodeCSV, data);

				if (node->childCount)
					count += npMapTraverseTree ((buffer + count), node, kNPmapNodeCSV, data);
			}
			//zzf end
		}

		if (count > kNPfileBlockSize)
			break;
	}

	*rootIndex = i + 1;	//i + 1	for next pass
	
//increment the buffer
//	data->io.write += count;	//zz debug

	return count;
}

// returns file version based on buffer contents
//-----------------------------------------------------------------------------
int npFileFormat(char* buffer, int wordSize, int size)
{
	int fileVer = 0;
	

	if (buffer == NULL)
		return 0;

	//original CSV version started with a newline, check for di
	if ( buffer[0] == '\n' || buffer[0] == '\r' )
		fileVer = kNPmapNodeCSVvOne;		//original CSV version 1
	else
		fileVer = kNPmapNodeCSV;		//otherwise assume ver 2 or later

	//update to make more robust, future proof, detailed comparison, debug zz

	return fileVer;
}

//-----------------------------------------------------------------------------
void npMapCSVvOne(pNPnode node)
{
	NPgridPtr gridData = NULL;

	node->recordID = (int)node->proximity.x;	//re-mapped

	node->lineWidth = 1.0f;				//original line width

	if (node->type == kNodePin)
	{
		//what was rotateRad.y is now rotate.z
		node->rotate.z = -node->rotate.y * kRADtoDEG;
		node->rotateRate.z = -node->rotateRate.y * kRADtoDEG;
		node->rotate.y = 0.0f;
		node->rotateRate.y = 0.0f;

		//for branchLevel 2 and above, translate.z offset is now translate.x
		//rotate.x is converted to translate.x
		if (node->branchLevel >= 2)	//child node rotation different from root
		{
			node->translate.z = node->translate.x;

			if (node->branchLevel == 2)	//map orientation, 0 deg is N y-axis
				node->translate.x = node->rotate.x * kRADtoDEG - 90.0f;
			else
				node->translate.x = node->rotate.x * kRADtoDEG;// - 90.0f;

			node->translateRate.x = node->rotateRate.x * kRADtoDEG;
			node->rotate.x = 0.0f;
			node->rotateRate.x = 0.0f;
		}
	}
	else if (node->type == kNodeCamera)
	{
		node->rotate.x = -node->rotate.x * kRADtoDEG + 90.0f;
		node->rotate.y = node->rotate.y * kRADtoDEG;

		node->rotateRate.x = -node->rotateRate.x * kRADtoDEG;
		node->rotateRate.y = node->rotateRate.y * kRADtoDEG;

		node->rotate.z = 0.0f;
		node->rotateRate.z = 0.0f;
	}
	else if (node->type == kNodeGrid)
	{
		gridData = node->data;

		node->textureID = 1;
				
		node->segments.x = 16;
		node->segments.y = 16;
		node->segments.z = 1;

		node->scale.x = 1.0f;			//changes 30 unit spacing to 10
		node->scale.y = 1.0f;
		node->scale.z = 1.0f;			//original grid had z = 1.5

		gridData->spacing.x	= 10.0f;
		gridData->spacing.y	= 10.0f;
		gridData->spacing.z	= 15.0f;	//original grid had z = 1.5
	}
	
	node->rotateVec.x = 0.0f;			//cam default is assigned to all
	node->rotateVec.y = 0.0f;
	node->rotateVec.z = 1.0f;
	node->rotateVec.angle = 0.0f;

}


// iterate through nodes update translation rotation and scale
//-----------------------------------------------------------------------------
int npExportMapFile (FILE* file, int type, void* dataRef)
{
	int size = 0;

	return size;
}

//build ptr map based on field names in the first line csv header
//-----------------------------------------------------------------------------
//npStrToNode
//converts a single CSV node record string and returns a pointer to the node
//void* npStrToNode (const char* buffer, char** endPtr, int size, void* dataRef)
void* npReadMapNodeCSV (const char* buffer, int wordSize, int size, 
						int format, int* scanNumRet, char** endPtr, 
						void* dataRef)
{
	char* cursor = (char*)buffer;
	int nodeCount = 0;
	int numExpected = 0;		//Expected number for scanNumRet returned from sscanf

	//Group #1
	int id, type, dataID, selected,	parentID, branchLevel, 
		childID, childIndex, childCount, nodeSize;


	//node tree pointers
	pNPnode node = NULL;
	pNPnode parent = NULL;
	pData data = (pData) dataRef;

	//replaces entire hard coded procedure
//	for (i=0; i < numOfElements; i++)
	{
		//where node[i] is this nodes current sub element
//		if(nodeStruct->type[i] == int)
//			(int)node[i] = npstrtoi(&cursor);
//		else
//			(float)node[i] = npstrtof(&cursor);
	}
	//then post process node for data tree attachment
		//add support for 			//zz debug


	//start processing buffer, these values updated during node creation
	id			= npstrtoi(&cursor);
	type		= npstrtoi(&cursor);
	dataID		= npstrtoi(&cursor);
	selected	= npstrtoi(&cursor);
	parentID	= npstrtoi(&cursor);
	branchLevel = npstrtoi(&cursor);
	childID		= npstrtoi(&cursor);
	childIndex  = npstrtoi(&cursor);	
	childCount	= npstrtoi(&cursor);

	//support for CSV ver 1
	if (format == kNPmapNodeCSVvOne && type == 8)	//workaround for 7-11 to 7-21 CSV files, debug zz
			type = kNodePin;

//zzhp debug, ONLY add the node to the map once finished reading in
	//add the node to the data map and map sort list, returns node ptr
	node = npMapNodeAdd (id, type, branchLevel, parentID, format, dataRef);
	if (node == NULL)
		return NULL;

	//update values already converted
	node->selected		= selected;
	node->childIndex	= childIndex;

	//convert the rest of string to the node values
	node->chInputID			= npstrtoi(&cursor);
	node->chOutputID		= npstrtoi(&cursor);
	node->chLastUpdated		= npstrtoi(&cursor);
	
	int junk0			= npstrtoi(&cursor);
	int junk1			= npstrtoi(&cursor);
	
	node->auxA.x			= npstrtof(&cursor);
	node->auxA.y			= npstrtof(&cursor);
	node->auxA.z			= npstrtof(&cursor);
	node->auxB.x			= npstrtof(&cursor);
	node->auxB.y			= npstrtof(&cursor);
	node->auxB.z			= npstrtof(&cursor);

	node->colorShift		= npstrtof(&cursor);

	node->rotateVec.x		= npstrtof(&cursor);
	node->rotateVec.y		= npstrtof(&cursor);
	node->rotateVec.z		= npstrtof(&cursor);
	node->rotateVec.angle	= npstrtof(&cursor);	
	
	node->scale.x			= npstrtof(&cursor);
	node->scale.y			= npstrtof(&cursor);
	node->scale.z			= npstrtof(&cursor);

	node->translate.x		= npstrtof(&cursor);
	node->translate.y		= npstrtof(&cursor);
	node->translate.z		= npstrtof(&cursor);

	node->tagOffset.x		= npstrtof(&cursor);
	node->tagOffset.y		= npstrtof(&cursor);
	node->tagOffset.z		= npstrtof(&cursor);
	
	node->rotateRate.x		= npstrtof(&cursor);
	node->rotateRate.y		= npstrtof(&cursor);
	node->rotateRate.z		= npstrtof(&cursor);

	node->rotate.x			= npstrtof(&cursor);
	node->rotate.y			= npstrtof(&cursor);
	node->rotate.z			= npstrtof(&cursor);
	
	node->scaleRate.x		= npstrtof(&cursor);
	node->scaleRate.y		= npstrtof(&cursor);
	node->scaleRate.z		= npstrtof(&cursor);

	node->translateRate.x	= npstrtof(&cursor);
	node->translateRate.y	= npstrtof(&cursor);
	node->translateRate.z	= npstrtof(&cursor);

	node->translateVec.x	= npstrtof(&cursor);
	node->translateVec.y	= npstrtof(&cursor);
	node->translateVec.z	= npstrtof(&cursor);
	
	node->shader			= npstrtoi(&cursor);
	node->geometry			= npstrtoi(&cursor);

	node->lineWidth			= npstrtof(&cursor);
	node->pointSize			= npstrtof(&cursor);
	node->ratio				= npstrtof(&cursor);

	node->colorIndex		= npstrtoi(&cursor);
	
	node->color.r			= npstrtoi(&cursor);
	node->color.g			= npstrtoi(&cursor);
	node->color.b			= npstrtoi(&cursor);
	node->color.a			= npstrtoi(&cursor);

	node->colorFade			= npstrtoi(&cursor);
	node->textureID			= npstrtoi(&cursor);

	node->hide				= npstrtoi(&cursor);
	node->freeze			= npstrtoi(&cursor);	

	node->topo				= npstrtoi(&cursor);
	node->facet				= npstrtoi(&cursor);

	node->autoZoom.x		= npstrtoi(&cursor);
	node->autoZoom.y		= npstrtoi(&cursor);
	node->autoZoom.z		= npstrtoi(&cursor);

	node->triggerHi.x		= npstrtoi(&cursor);
	node->triggerHi.y		= npstrtoi(&cursor);
	node->triggerHi.z		= npstrtoi(&cursor);

	node->triggerLo.x		= npstrtoi(&cursor);
	node->triggerLo.y		= npstrtoi(&cursor);
	node->triggerLo.z		= npstrtoi(&cursor);
	
	node->setHi.x			= npstrtof(&cursor);
	node->setHi.y			= npstrtof(&cursor);
	node->setHi.z			= npstrtof(&cursor);

	node->setLo.x			= npstrtof(&cursor);
	node->setLo.y			= npstrtof(&cursor);
	node->setLo.z			= npstrtof(&cursor);

	node->proximity.x		= npstrtof(&cursor);
	node->proximity.y		= npstrtof(&cursor);
	node->proximity.z		= npstrtof(&cursor);

	node->proximityMode.x	= (int)npstrtof(&cursor);	//int cast supports CSV ver 1
	node->proximityMode.y	= (int)npstrtof(&cursor);
	node->proximityMode.z	= (int)npstrtof(&cursor);

	node->segments.x		= (int)npstrtof(&cursor);	//grid segments were stored in node->data,
	node->segments.y		= (int)npstrtof(&cursor);	//was node->data->segments.x
	node->segments.z		= (int)npstrtof(&cursor);	//now node->segments.x

	node->tagMode			= npstrtoi(&cursor);
	node->formatID			= npstrtoi(&cursor);
	node->tableID			= npstrtoi(&cursor);
	node->recordID			= npstrtoi(&cursor);

	//node->size			handled during node creation
	nodeSize				= npstrtoi(&cursor);	//perhaps don't increment, debug zzs
	
	//return the endpoint
	*endPtr = cursor;

	if( nodeSize != 420 )						//possibly add sizes, debug zz
	{
		printf( "err 4257 - unexpected end line size: %d\n", size );

		*scanNumRet = -1;	//zzs handles end of file... will abort on one bad node
							//add ability to skip over bade nodes in wrapping function
		return node;
	}

	//support for first version CSV
	if (format == kNPmapNodeCSVvOne)
		npMapCSVvOne (node);

	//support for ver 2 CSV, compatability prior to 2012-04-22
	//we now require that pin nodes have a topo type
	if (node->topo == 0 && node->type == kNodePin)
	{
		//printf ("topo = 0   id: %d\n", node->id);
		if (node->branchLevel == 0)
			node->topo = kNPtopoPin;	//set root topo to a pin
		else if (node->parent != NULL)  //orhpan child methods in npMapSort
		{
			parent = node->parent;
			if ( parent->topo == kNPtopoPin || parent->topo == 0
				|| parent->topo == kNPtopoTorus )
				node->topo = kNPtopoTorus;
			else
				node->topo = kNPtopoPin;
		}
	}

	//workaround for keeping track of link end B, processed by orphan list
	if (node->type == kNodeLink)
		node->childIndex = childID;

	//for sorting orphan nodes
	nodeCount = ++data->map.sortCount;

	if (node->type == kNodeGrid)
	{
		if (node->auxA.x == 0.0f)
			node->auxA.x = kNPgridSpacing;
		if (node->auxA.y == 0.0f)
			node->auxA.y = kNPgridSpacing;
		if (node->auxA.z == 0.0f)
			node->auxA.z = kNPgridSpacing;
	}

	return node;
}

//-----------------------------------------------------------------------------
void npLoadMapFile (char* buffer, int wordSize, int size, void* dataRef)
{
	char* endPtr = NULL;
	int i = 0, count = 0, curs = 0;				//Cursor position in Buffer parsing source
	int scanNumRet = 0;			//sscanf return value. Number of successfuly scanned elements
	int nodeCount = 0;
	int format = 0;
	
	pNPnode node = NULL;
	pData data = (pData) dataRef;


	// CSV version 1, starts with a '\n' so line 1 is blank, line 2 is field names
	// and data starts on 3rd line
	// CSV ver 2 starts with field names and data on 2nd line
	format = npFileFormat (buffer, wordSize, size);
	if ( format == kNPmapNodeCSVvOne )							
		count += curs = npSeekToNextLine( buffer ); buffer = &buffer[curs];	//skip blank line

	// print first two lines
	printf ("File Version %d Contents:\n", format);	
	for( i = 0; i < 80; i++ )			//first 80 chars of line one
		printf( "%c", buffer[i]);

	curs = npSeekToNextLine( buffer );	//find next line, but dont advance buffer
	for( i = 0; i < 80; i++ )			//second line
		printf( "%c", buffer[curs+i]);

	//for sorting parent and child nodes that are out of order in the file
	npMapSortInit (data->map.sortID, data);

	printf ("Processing Nodes...\n");

	//scan in all lines and create nodes
	while( scanNumRet != -1 )
	{
		//advance buffer to next line
		count += curs = npSeekToNextLine (buffer); buffer = &buffer[curs];

		// if end of buffer then exit loop
		if (count >= size)
		{
			scanNumRet = -1;
			continue;								// end of file, exit loop
		}

		//maps a single line CSV formatted to a node
		node = npReadMapNodeCSV (buffer, wordSize, size, 
								format, &scanNumRet, &endPtr, dataRef);
	//	buffer = endPtr;

		if (node != NULL)
		{
			nodeCount++;
			
			//print part of the first few lines of data
			if ( nodeCount <= 3)
			{
				printf("id: %d   type: %d   branchLevel: %d   colorIndex: %d   recordID: %d\n", 
					node->id, node->type, node->branchLevel,  node->colorIndex, node->recordID);
	
				if ( nodeCount == 3)
					printf("... ");
			}
			else if ( nodeCount == (nodeCount / 1000) * 1000 )	//print id from file every 100 nodes
				printf("%d ", nodeCount);
		}
		else
			printf("err 2440 - npLoadMapFile node is null\n");

	}	// end loop

	// print last node info
	if (node != NULL )
	{
		printf("\nid: %d   type: %d   branchLevel: %d   colorIndex: %d   recordID: %d\n", 
			node->id, node->type, node->branchLevel,  node->colorIndex, node->recordID);
	}
	else
		npPostMsg("err 2350 - last node loaded from file is NULL", kNPmsgErr, data);

	//attach orphan child nodes to parent nodes
	npMapSort (data);

	printf ("\nNode Count: %d\n", nodeCount);

	//update the selected and active node to be root of last node created
	npSelectNode (data->map.node[data->map.nodeRootCount - 1], data);

	//if old original CSV then set the active cam to the proper child
	if ( format == kNPmapNodeCSVvOne )
	{
		node = data->map.node[kNPnodeRootCamera];
		data->map.currentCam = node->child[node->childIndex];
		npSelectNode (data->map.node[kNPnodeRootGrid], data);
	}

//	printf ("Synchronizing Text Tags\n");
//	npSyncTags (data);						//make this smarter, debug zz

	printf ("Synchronizing Channel Map\n");
	npSyncChMap (data);							//zz-JJ
}
//MB-End//



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
ANTZCORE_API int npFileOpenMap (const char* filePath, int wordSize, int size, void* dataRef)
{
	FILE* file = NULL;
	size_t count = 0;
	int i = 0;
	int byteCount = 0;
	char* bufferIndex = 0;
	char* buffer;

	pData data = (pData) dataRef;


	printf("\nOpen File: %s\n", filePath);
	file = npFileOpen (filePath, "r", dataRef);	// open file in read mode "r"

	if( file != NULL )   // check to see if file exists
	{	
		buffer = (char*) malloc(kNPmapFileBufferMax);
		if (buffer == NULL)
		{
			printf ("err 4285 - malloc failed, cannot write file");
			return 0;
		}

		count = npFileRead (buffer, 1, kNPmapFileBufferMax, file, dataRef);

		npFileClose (file, dataRef);	//done reading, close file
		
		if (count > 0)
		{
			npLoadMapFile (buffer, wordSize, (int)count, dataRef);	//process data
		}
		else
			printf("warn 7287 - zero bytes read\n");

		free (buffer);
	}
	else
		printf("warn 7288 - cannot open file\n");


	return 1;
}


//-----------------------------------------------------------------------------
ANTZCORE_API int npFileSaveMap(const char* filePath, int wordSize, int size, void* dataRef)
{
	int i = 0;
	int err = 0;
	size_t total = 0;
	int rootIndex = 0;
	size_t count = 0;
	FILE* file = NULL;

	pData data = (pData) dataRef;
	pNPnode node = NULL;

	char* buffer = (char*) malloc(kNPmapFileBufferMax);
	if (buffer == NULL)
	{
		printf ("4214 error - malloc failed, cannot write file\n");
		free (buffer);
		return 0;
	}

	//zzf
	//if 'Alt' key held then only save current selection
	if (data->io.key.modAlt)
		data->io.file.saveSelect = true;
	else
		data->io.file.saveSelect = false;
	//zzf end

	// open the file, "w+" overwrites existing files
	printf ("\nSave File: %s\n", filePath);
	file = npFileOpen (filePath, "w+", dataRef);

	if (file == NULL)
	{
		printf("err 4215 - file open failed, cannot write file\n");							//debug, add err reporting, zz
		free (buffer);
		return 0;
	}
	
	// copies current scene state to the write buffer, formats as CSV
	printf("Processing...\n");
	count = npMapNodesToCSV (buffer, kNPmapFileBufferMax, &rootIndex, dataRef);

	while (count > 0)
	{
		printf("Writing...\n");
		total += count = (size_t)npFileWrite (buffer, 1, (int)count, file, dataRef);

		if (count >= kNPfileBlockSize)
		{
			printf("Processing...\n");
			count = npMapNodesToCSV (buffer, kNPmapFileBufferMax, &rootIndex, dataRef);
		}
		else
			count = 0;
	}

	// print first line of file contents
	if (total > 0)
	{
		npFileRewind(file);
		size = npFileRead (buffer, 1, 79, file, dataRef);
		printf("File Contents:\n");
		for( i = 0; i < size; i++ )
			printf("%c", buffer[i]);
		printf("\n");
	}
	else
		printf("err 4216 - file write failure, zero bytes written\n");

	err = npFileClose(file, dataRef);
	if (err)
		printf("err 4217 - file close failed, return: %d\n", i);

	printf("Done\n\n");

	// separate into CSV files and equivalent db tables, debug, zz
	// file tables include pin data, child lists, camera data

	free (buffer);

	data->io.file.saveSelect = false;			//zzf end

	return 1;
}


//-----------------------------------------------------------------------------
// if not saved then opens SaveAs dialog before reseting app
void npCloseFileMapFile (int size, void* dataRef)
{
	return;
}


//-----------------------------------------------------------------------------
int npGetMapFileAttrib (int attrib, void* dataRef)
{
	switch (attrib)
	{
	case kNPversion: ; break;
	case kNPtableCount: ; break;	// number of struct equivalent data tables 
	case kNPrecordCount: ; break;	// total number of records
	case kNPnodeRootCount: ; break;
	case kNPsize: ; break;			// RAM used by map file

	default : break;
	}

	return 0;
}

//-----------------------------------------------------------------------------
void npFileOpenMapAttrib (int attrib, void* dataRef)
{
	return;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

struct NPbuffer			//circular buffer
{
	char** buffer;		//pointer to the beginning of the buffer
	int writeIndex;		//most current data entry, 
	int readIndex;		//write lock from index to the lock, holds until processed
	int size;			//end of the buffer, starts out at zero, stops at max
};
typedef struct NPbuffer NPbuffer;
typedef struct NPbuffer *pNPbuffer;


#define kNPtagFieldCount 5

int npLoadNodesCSV (const char* buffer, int size, int type, void* dataRef);
//-----------------------------------------------------------------------------
//zz debug, use a func ptr to npLoadCSV() from genreic parent file IO process	
int npLoadNodesCSV (const char* buffer, int size, int type, void* dataRef)
{
//	printf("npLoadNodesCSV Test!\n");
//	npLoadMapFile ((char*)buffer, 1, size, dataRef);

	char* endPtr = NULL;
	int i = 0, count = 0, curs = 0;				//Cursor position in Buffer parsing source
	int scanNumRet = 0;			//sscanf return value. Number of successfuly scanned elements
	int nodeCount = 0;
	int format = type;
	
	pNPnode node = NULL;
	pData data = (pData) dataRef;


	// CSV version 1, starts with a '\n' so line 1 is blank, line 2 is field names
	// and data starts on 3rd line
	// CSV ver 2 starts with field names and data on 2nd line
//	format = kNPmapNodeCSV;

	//if exists, skip over leading line return
	if (*buffer == '\r' || *buffer == '\n')
	{
		count += curs = npSeekToNextLineLimit(buffer, size);
		buffer = &buffer[curs];
	}

	// print first two lines
	printf ("File Version %d Contents: ", format);	
	printf( "%.40s\n", buffer);

	//for sorting parent and child nodes that are out of order in the file
//	npMapSortInit (data->map.sortID, data);

	printf ("Processing Nodes...\n");

	//scan in all lines and create nodes
	while( scanNumRet != -1 )
	{
		//maps a single line CSV formatted to a node
		node = npReadMapNodeCSV (buffer, 1, size, 
								format, &scanNumRet, &endPtr, dataRef);

		//advance buffer to next line
		count += curs = npSeekToNextLineLimit (buffer, (size - count));
		buffer = &buffer[curs];

		//print out the first few records
		if (node != NULL)
		{
			nodeCount++;
			
			//print part of the first few lines of data
			if ( nodeCount <= 3)
			{
//				printf("count: %d   size: %d\n",count,size);				//zzhp
				printf("id: %d   type: %d   branchLevel: %d   colorIndex: %d   recordID: %d\n", 
					node->id, node->type, node->branchLevel,  node->colorIndex, node->recordID);
	
				if ( nodeCount == 3)
					printf("... ");
			}
			else if ( nodeCount == (nodeCount / 1000) * 1000 )	//print id from file every 100 nodes
				printf("%d ", nodeCount);
		}
		else
			printf("err 2440 - npLoadMapFile node is null\n");

		// if end of buffer then exit loop
		if (count >= size)
			scanNumRet = -1;
	}	// end loop

	// print last node info
	if (node != NULL)
	{
		if(nodeCount > 3)
			printf("\nid: %d   type: %d   branchLevel: %d   colorIndex: %d   recordID: %d\n", 
				node->id, node->type, node->branchLevel,  node->colorIndex, node->recordID);
	}
	else
		npPostMsg("err 2350 - last node loaded from file is NULL", kNPmsgErr, data);

	return nodeCount;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//zz debug
//zz debug
enum{
	kNPfileNull = 0,

	kNPfileTIFF,	//image formats
	kNPfileRAW,
	kNPfileJ2K,		//JPEG 2000
	kNPfileJPG,

	kNPfileCSV,		//data table formats
	kNPfileTXT,
	kNPfileXML,
	kNPfileJSON,	//data tree schema

	kNPfileKML,		//GIS standard
	kNPfileCollada,	//3D model inter-change

	kNPfileAIFF,	//audio
	kNPfileWAV,

	kNPfileMP3,
	kNPfileM4A,

	kNPfileMPG,		//MPEG 1 and 2 video
	kNPfileMP4,		//MPEG 4

	kNPfileMJ2,		//Motion JPEG 2000
	kNPfileDCP,		//Digital Cinema Package

	kNPfileMXF,		//AV formats, open standard and proprietary
	kNPfileDNX,
	kNPfileAVI,
	kNPfileMOV
};

//currently based soley on extension name
//zz debug, add checking header contents
//------------------------------------------------------------------------------
int npGetFileType(const char* filePath, void* dataRef)
{
	int i = 0;
	int period = 0;
	char ext[256];

	//get extension, CSV, XML, JSON...
	while (filePath[i] != '\0')
	{
		i++;
		if (filePath[i] == '.')
			period = i + 1;
		if (period)
			ext[i - period] = filePath[i];
	}

	//null terminate the ext string
	ext[i - period + 1] = '\0';
	
	//force the file extension to be all lower case
	while (ext[i] != '\0')
	{
		if (ext[i] >= 'A' && ext[i] <= 'Z')
			ext[i] += 33;			//ASCII ('a' - 'A') = 33
		i++;
	}


	//return the extension type
//	if (strncmp(ext, "tiff", 4)) return kNPfileTIFF;	//redundant
	if (strncmp(ext, "tif", 3)) return kNPfileTIFF;		//catches .tif and .tiff
	if (strncmp(ext, "raw", 3)) return kNPfileRAW;

	if (strncmp(ext, "jp2", 3)) return kNPfileJ2K;
	if (strncmp(ext, "jpg", 3)) return kNPfileJPG;
	if (strncmp(ext, "jpeg", 4)) return kNPfileJPG;
	
	if (strncmp(ext, "csv", 3)) return kNPfileCSV;
	if (strncmp(ext, "txt", 3)) return kNPfileTXT;
	if (strncmp(ext, "xml", 3)) return kNPfileXML;
	if (strncmp(ext, "json", 4)) return kNPfileJSON;

	if (strncmp(ext, "kml", 3)) return kNPfileKML;
	if (strncmp(ext, "dae", 4)) return kNPfileCollada;

//	if (strncmp(ext, "aiff", 4)) return kNPfileAIFF;
	if (strncmp(ext, "aif", 3)) return kNPfileAIFF;

	if (strncmp(ext, "mxf", 3)) return kNPfileMXF;

	return 0;	//unknown file type
}

int npGetFileFormat(const char* buffer, void* dataRef)
{
	int format = kNPformatNull;
	int fileType = 0;

	//get file type
//	fileType = npGetFileType (filePath, dataRef);

	//check header contents
	//nodes, tags, chmap, chtracks, globals...
	switch (fileType)
	{
		case kNPfileCSV : format = kNPformatCSV; break;
		default : break;
	}

	return format;
}

//-----------------------------------------------------------------------------
int npFileReadBlocks (const char* filePath, char* buffer, void* dataRef)
{
	FILE* file = NULL;

	int lock = false; //data->io.file.read.lock;

	int count = 0;		//number of bytes read
	int total = 0;
	int reading = true;

	char msg[256];

	// open file in read mode "r"
	sprintf(msg,"File Path: %s",filePath);
	npPostMsg(msg, kNPmsgCtrl, dataRef);
	file = npFileOpen(filePath, "r", dataRef);

	if( file == NULL )   // if NULL then failed to open the file 
	{
		npPostMsg("err 4988 - cannot open file", kNPmsgErr, dataRef);
		return 0;
	}

	//fill buffer with file contents, wait for thread lock to overwrite
	npPostMsg("Reading File...", kNPmsgCtrl, dataRef);
	while (reading)
	{
		count = npFileRead (buffer, 1, kNPfileBlockSize, file, dataRef);

		if (!count)
			reading = false;	//done, exit loop
		else
		{
			buffer += count;
			total += count;
			while (lock) nposSleep(0.001);	//sleep while buffer locked
		}
	}

	//if processing stops handshaking then we close the file and exit thread
	//	npPostMsg("err 7384 - file read halted", kNPmsgErr, dataRef);

	npFileClose (file, dataRef);
	npPostMsg("Done Reading, File Closed", kNPmsgCtrl, dataRef);

	return total;
}


const char* gThreadFilePath;
bool gThreadTagFileLock;
//-----------------------------------------------------------------------------
void npThreadFileOpenMap (void* dataRef)
{
	int result = 0;

//	char msg[256];

	pData data = (pData) dataRef;

	char filePath[4096]; // = data->io.file.currentOpenPath;

//	pNPbuffer read = malloc(sizeof(NPbuffer));

	strcpy (filePath, data->io.file.currentOpenPath);

	result = npFileOpenMap (filePath, 1, (int)strlen(filePath), dataRef);

	if (!result)
		npPostMsg ("err 4981 - npThreadFileOpenMap failed", kNPmsgErr, data);
//	else
//		npPostMsg ("Done Loading Nodes", kNPmsgErr, data);

	if (data->map.syncTagsReady)
	{
		printf ("Sync Tags...\n");
		npSyncTags (dataRef);
		npPostMsg ("Sync Done!", kNPmsgCtrl, dataRef);
	}
	else
		data->map.syncNodes =  true;		//sync when both are ready
/*	
	strcpy (filePath, data->io.file.csvPath);
				strcat (filePath, "antztag0001.csv");
				sprintf(msg, "Loading: %s", filePath);
				npPostMsg (msg, kNPmsgCtrl, dataRef);
				gThreadFilePath = msg;
				//result = npFileOpenTag (filePath, 1, strlen(filePath), dataRef);
				npFileOpenTagThread (dataRef);
*/
	nposEndThread();
//	return;
}


//update to recognize all our file types and binaray data structures			debug zz
//also recognize the apps own code and foreign code...
char* npGetType(int* type, int* format, const char* str, int size, void* dataRef);
//-----------------------------------------------------------------------------
char* npGetType(int* type, int* format, const char* str, int size, void* dataRef)
{
	int curs = 0;

	if (size < 40) 
	{
		npPostMsg("err 3459 - unknown file type, expected a CSV tag file", kNPmsgErr, dataRef);
		*type = 0;
		*format = 0;
		return (char*)str;
	}

	//first field should be 'id' the following fields vary
	if (strncmp(str, "id", 2) != 0)
	{
		//add checking for CSV ver 1
		curs = npSeekToNextLineLimit(str, size);
		if (strncmp(&str[curs], "id,type,data,selected,parent_id,branch_level", 44) == 0)
		{
			printf ("CSV Node File Ver: 2\n");	//add file version, debug zz
			printf( "%.39s\n", str);
			*type = kNPfileCSV;
			*format = kNPtableNode;
		}
		else
		{
			npPostMsg("err 3460 - unknown file type, expected a CSV file", kNPmsgErr, dataRef);
			return (char*)str;
		}
	}
	else if (strncmp(str, "id,type,data,selected,parent_id,branch_level", 44) == 0)
	{
		printf ("CSV Node File Ver: 2\n");
		printf( "%.39s\n", str);
		*type = kNPfileCSV;
		*format = kNPtableNode;
	}
	else if (strncmp(str, "id,record_id,table_id,title,description", 39) == 0)
	{
		printf ("CSV Tag File Ver: 1\n");
		printf( "%.39s\n", str);
		*type = kNPfileCSV;
		*format = kNPtableTag;
	}
	else if (strncmp(str, "id,channel_id,track_id,attribute,track_table_id", 47) == 0)
	{
		printf ("CSV Channel Map File Ver: 1\n");
		printf( "%.39s\n", str);
		*type = kNPfileCSV;
		*format = kNPtableChMap;
	}
	else if (strncmp(str, "track_id,", 9) == 0)
	{
		printf ("CSV Tracks File Ver: 1\n");
		printf( "%.39s\n", str);
		*type = kNPfileCSV;
		*format = kNPtableTracks;
	}
	else
		return (char*)str;

	//add kNPtableGlobals, kNPtablePathMap, kNPtable...						//zz debug

	curs = npSeekToNextLineLimit(str, size);		
	return (char*)&str[curs];
}

void npSyncRecords (pNPrecordSet recordSet, void* dataRef);
//-----------------------------------------------------------------------------
void npSyncRecords (pNPrecordSet recordSet, void* dataRef)
{
	pData data = (pData) dataRef;

	npTagSort(dataRef);	//sort prior to sync

	//flag ctrl that tags are ready for sync	//zz debug, move this logic to the main ctrl loop to sync datamap
	if (data->map.syncNodes)
	{
		printf ("Sync Tags...\n");
		npSyncTags (dataRef);								//upgrade this, debug zz
		npPostMsg ("Sync Done!", kNPmsgCtrl, dataRef);

		npFree (recordSet, dataRef);
	}
	else
	{
		data->map.syncTagsReady = true;
		//store(recordSet);
	}
}

void npPreLoadInit(pNPrecordSet recSet, void* dataRef);
//-----------------------------------------------------------------------------
void npPreLoadInit(pNPrecordSet recSet, void* dataRef)
{
	pData data = (pData) dataRef;

//for sorting parent and child nodes that are out of order in the file	//zz debug
	if (recSet->type == kNPtableNode)
		npMapSortInit (data->map.sortID, data);
}

void npPostMap (pNPrecordSet recSet, void* dataRef);
//-----------------------------------------------------------------------------
void npPostMap (pNPrecordSet recSet, void* dataRef)
{
	pData data = (pData) dataRef;
		
	//npMapSync(data);

	//post process for node attachment and selection
	if (recSet->type == kNPtableNode)
	{
		//update the selected and active node to be root of last node created
		npSelectNode (data->map.node[data->map.nodeRootCount - 1], data);

			//if old original CSV then set the active cam to the proper child
		//if ( recordSet->type == kNPmapNodeCSVvOne )
		{
		//	node = data->map.node[kNPnodeRootCamera];
		//	data->map.currentCam = node->child[node->childIndex];
		//	npSelectNode (data->map.node[kNPnodeRootGrid], data);
		}

		printf ("Map Sort...\n");	//attach orphan nodes
		npMapSort (data);
	
		data->io.file.loading = false;											//zzhp
		npSelectNode(data->map.node[kNPnodeRootPin], data);			//zzhp

		data->map.syncNodes = true;	//flag that nodes are ready for tag sync

		printf ("Sync Channel Map...\n");
		npSyncChMap (data);							//zz-JJ
	}

	//sync the nodes and tags
	if (recSet->count)
		npSyncRecords(recSet, data);
}

/*
	struct {
		void* dataRef;
		char** buffer;
		int writeIndex;
		int readIndex;			//current point read up to
		int blockSize;			//file IO is 4KB to 16MB
		int	threadID;			//threads divide up the block based on count
		int workerID;			//id and count determine buffer slot
		int workerCount;		//number of threads to divide up task 
	}
*/
//------------------------------------------------------------------------------
ANTZCORE_API int npFileOpenAuto(const char* filePath, FILE* file, void* dataRef)
{
	pData data = (pData) dataRef;

	//pData data = (pData) dataRef;
	pNPthreadFile threadFile = NULL;

	threadFile = npMalloc (0, sizeof(NPthreadFile), dataRef);

	threadFile->filePath = npNewStrcpy(filePath, dataRef);
	threadFile->file = file;
	threadFile->dataRef = dataRef;
	//store the filePath for the thread process to retrieve
	//	strcpy (data->io.file.currentOpenPath, msg);
//	while (gThreadTagFileLock) sleep(0.001);
//	gThreadTagFileLock = true;
//	gThreadFilePath = filePath;
	
	//thread the file block read process
	// npFileOpenThread(dataRef);
	//nposBeginThread(npFileOpenThread, threadFile);

//	read->buffer = buffer;

//	read->index = 0;
//	read->lock = 0;
//	read->size = kNPfileBufferSize;

	//data->io.file.read.buffer = buffer;
	//data->io.file.read.size = kNPfileBufferSize;
	//
	// data->io.file.read.index = buffer;
	// data->io.file.read.lock = false;

	//reads file blocks into buffer
	//if file size > buffer then waits on buffer lock for processing 
	//then overwrites un-locked region of the oldest data
	//close file upon reaching EOF, buffer contents retained, exits thread
//	npThread (npFileReadBlocks(file, buffer), dataRef);

	//launch thread(s) to process file buffer contents
	//sets buffer lock position based on blocks processed
	//upon finishing buffer contents it then post-processess sorts and syncs
//	npMultiThread (npThreadImportCSV(buffer, data), data);

	//app loop continues while file processed in background

//	return 1;

/*
	int format = 0;
	format = kNPcsvTags;	//zz debug 
	switch (format)
	{
		case kNPformatAuto : break;
		case kNPformatNodes : break;
		case kNPformatTags : npMultiThread (npParseCSV(), buffer, data); break;
		case kNPformatChMap : break;
		case kNPformatChTracks : break;
		case kNPformatGlobals : break;
		default :
			npPostMsg("err 2844 - unknown file format", kNPmsgErr, dataRef);	 
			break;
	}
*/

	return 1;
}


//-----------------------------------------------------------------------------
int npFileSaveTag (const char* filePath, int wordSize, int size, void* dataRef)
{
	printf ("err 4339 - failed to save CSV Tag file\n");
	return 0;
}

//recursively traverses tree for all child branches
//------------------------------------------------------------------------------
int npMapTraverseTree (char* buffer, pNPnode node, int format, void* dataRef)
{
	int i = 0;
	int count = 0;
	pNPnode nodeChild = NULL;

	//zzf
	pData data = (pData) dataRef;
	if ( data->io.file.saveSelect && !node->selected )
		return count;

	// if not root then write the data, root has already been written
	if (node->branchLevel > 0)
		count += npWriteNode (buffer, node, format, dataRef);

	//recursively calls this function for all children
	for (i=0; i < node->childCount; i++)//node->childCount; j++)
	{
		nodeChild = node->child[i];
		//zzf
		if ( data->io.file.saveSelect && !nodeChild->selected )
			continue;
	//	else
		{
			if (node->child[i] != NULL				//zzsql debug, this is bad logic method
				&& node->child[i]->child[0] != node)	//don't write twice, this is link B so skip
				count += npMapTraverseTree (buffer+count, node->child[i], format, data);	
		}
	}

	return count;
}
//zzsql node write traversal
int npMapTraverseTreeLL(char* buffer, int* index, pNPnode node, int format, void* dataRef)
{
	pData data = (pData) dataRef;
	int i = 0;
	int count = 0;
	
	//	printf("\nIndex is %d :: node->branchLevel : %d :: node->childCount : %d", *index, node->branchLevel, node->childCount);
	if (node->branchLevel > 0)
	{
		count = npWriteNode(buffer, node, format, dataRef);
		buffer[count] = '\0';
		//	printf("\nIndex : %d", *index);
		//	printf("\nchild : %s", buffer[*index]);
		//getch();
		(*index)++;
		data->io.write = buffer;			//zzsql does not seem to make sense...
	}
	
	//recursively calls this function for all children
	for (i=0; i < node->childCount; i++)//node->childCount; j++)
	{
		if (node->child[i] != NULL					//zzsql debug, this is bad logic method
			&& node->child[i]->child[0] != node)	//don't write twice, this is link B so skip
		{
			count += npMapTraverseTreeLL(buffer, index, node->child[i], format, dataRef);
		}
	}
	//	printf("npMapTraverseTreeLL totalCount :: %d", count);
	
	
	//return count;
	return count;
}
//zzsql

//zzsql
//-----------------------------------------------------------------------------
// keep track of a bufferIndex for each list
// write list of children
// write node or node list
// write node specific data to a separate list, ie pin, camera, etc
// write list of channels,						channels NOT YET IMPLEMENTED, debug, zz
// returns a pointer to the last char written to buffer
//-----------------------------------------------------------------------------
//int npWriteNode (const char* buffer, pNPnode node, int format, char** endPtr, void* dataRef)
// debug db
int npWriteNode (const char* buffer, pNPnode node, int format, void* dataRef)
{
	int n			= 0;
	int parentID	= 0;
	int childID		= 0;

	pNPnode parent = NULL;

	pData data = (pData) dataRef;

	// pointers to the node data, update to be a generic list, debug, zz
	// break into separate buffers, debug zz
	char* nodePtr = (char*) buffer;

/*
	switch (format)
	{	
		case kNPnodeCSV :	npNodeToCSV(buffer, nodeRef, format, dataRef); break;
		case kNPnodeOSC :	npNodeToOSC (buffer, nodeRef, format, dataRef); break;
		case kNPnodeMySQL : npNodeToSQL (buffer, nodeRef, format, dataRef); break;
		case kNPnodeXML :	npNodeToKML (buffer, nodeRef, format, dataRef); break;
		case kNPnodeJSON :	npNodeToJSON (buffer, nodeRef, format, dataRef); break;
		case kNPnodeKML :	npNodeToKML (buffer, nodeRef, format, dataRef); break;
		case default : break;
	}
*/
	//if parent exists then set the parentID, otherwise defaults to 0
	if (node->parent != NULL)
		parentID = node->parent->id;

	//if kNodeLink then childID is set to the link-B node id, default is 0
	if (node->type == kNodeLink)
		childID = node->child[0]->id;	//link-B node ptr stored at child[0]

	//format as CSV and fill the buffer, broken into groups for readability
	n += sprintf ((nodePtr + n), "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
		node->id,					//id is unique to this node
		node->type,		
		node->id,					// node->data->id uses this node id
		
		node->selected,
		parentID,					// parent id replaces pointer to parent
		node->branchLevel,
		childID,					// either same as the node or id of link end B
		node->childIndex,
		node->childCount,
		
		node->chInputID,
		node->chOutputID,	
		node->chLastUpdated,

		0, //node->average,
		0, //node->interval,

		// ot: this was writing floats but with %d specifier, wtf
		(int)node->auxA.x,
		(int)node->auxA.y,
		(int)node->auxA.z,

		(int)node->auxB.x,
		(int)node->auxB.y,
		(int)node->auxB.z
	);

	//  next group
	n += sprintf ((nodePtr + n), ",%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
		node->colorShift,

		node->rotateVec.angle, 
		node->rotateVec.x,
		node->rotateVec.y,
		node->rotateVec.z,
		
		node->scale.x,
		node->scale.y,
		node->scale.z,
		
		node->translate.x,
		node->translate.y,
		node->translate.z,

		node->tagOffset.x,
		node->tagOffset.y,
		node->tagOffset.z,
		
		node->rotateRate.x,
		node->rotateRate.y,
		node->rotateRate.z,

		node->rotate.x,
		node->rotate.y,
		node->rotate.z
	);

	n += sprintf ((nodePtr + n), ",%f,%f,%f,%f,%f,%f,%f,%f,%f", 
		node->scaleRate.x,
		node->scaleRate.y,
		node->scaleRate.z,

		node->translateRate.x,
		node->translateRate.y,
		node->translateRate.z,

		node->translateVec.x,
		node->translateVec.y,
		node->translateVec.z
	);

	n += sprintf ((nodePtr + n), ",%d,%d,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d",
		node->shader,
		node->geometry,

		node->lineWidth,
		node->pointSize,
		node->ratio,

		node->colorIndex,
		
		node->color.r,
		node->color.g,
		node->color.b,
		node->color.a,

		node->colorFade,
		node->textureID
	);

	n += sprintf ((nodePtr + n), ",%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
		node->hide,
		node->freeze,
		node->topo,
		node->facet,

		node->autoZoom.x,
		node->autoZoom.y,
		node->autoZoom.z,
		
		node->triggerHi.x,
		node->triggerHi.y,
		node->triggerHi.z,

		node->triggerLo.x,
		node->triggerLo.y,
		node->triggerLo.z 
	);

	n += sprintf ((nodePtr + n), ",%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d",
		node->setHi.x,
		node->setHi.y,
		node->setHi.z,

		node->setLo.x,
		node->setLo.y,
		node->setLo.z,

		node->proximity.x,
		node->proximity.y,
		node->proximity.z,

		node->proximityMode.x,
		node->proximityMode.y,
		node->proximityMode.z
	);

	//last group
	n += sprintf ((nodePtr + n), ",%d,%d,%d,%d,%d,%d,%d,%d",
		node->segments.x,
		node->segments.y,
		node->segments.z,

		node->tagMode,

		node->formatID,
		node->tableID,
		node->recordID,
		
		0 //node->size 
	);

/*
//------------------------------------------------------------------------------
void npitoa(char** buffer, const int* value)
{
	itoa(*value, *buffer, 0);
}

//------------------------------------------------------------------------------
void npftoa(char** buffer, const int* value)
{
	ftoa(*value, *buffer, 0);
}

//------------------------------------------------------------------------------
void npIntToCSV(char** buffer, const int* value)
{
	itoa(*value, *buffer, 0);
	buffer++;
	buffer* = ',';
	buffer++;
}

//------------------------------------------------------------------------------
void npFloatToCSV(char** buffer, const int* value)
{
	itoa(*value, *buffer, 0);
}

	npIntToCSV(&buffer, &node->id);
	//...
	npFloatToCSV(&buffer, &node->proximity.z);

	npIntToCSV(&buffer, &node->proximityMode.x);
	npIntToCSV(&buffer, &node->proximityMode.y);
	npIntToCSV(&buffer, &node->proximityMode.z);
	//...
	npIntToCSV&buffer, &node->recordID);

	npitoa(&buffer, &node->size);		//no comma for last field

//	buffer++;

	//after last group, end the node row using a newline
	*buffer = '\n';
*/


	//after last group, end the node row using a newline
	n += sprintf ((nodePtr + n), "\n");						//zz debug pull this out to higher level... SQL compatible

	//increment the buffer
	data->io.write += n;

	return n;
}



