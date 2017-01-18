/* -----------------------------------------------------------------------------
*
*  nptag.c
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

#include "nptags.h"

#ifdef WIN32
#include <Windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include "npgldraw.h"


//------------------------------------------------------------------------------
ANTZCORE_API void npInitTags(void* dataRef)
{
	return;
}

//------------------------------------------------------------------------------
void npCloseTags (void* dataRef)		//clean-up
{
	npDeleteAllTags (dataRef);
}

//------------------------------------------------------------------------------
void npUpdateTags (void* dataRef)							//add to ctrl loop, debug zz
{
	return;
}

//------------------------------------------------------------------------------
void npImportTagsFromCSV (char* buffer, int size, int wordSize, void* dataRef)
{
	return;
}


//allocates buffer and fills with CSV formatted tags, includes header
//------------------------------------------------------------------------------
void npExportTagsToCSV (char* buffer, int* size, int* wordSize, void* dataRef)
{
	return;
}

//deletes entire tag list
//------------------------------------------------------------------------------
ANTZCORE_API void npDeleteAllTags(void* dataRef)
{
	int i = 0;
	pData data = (pData)dataRef;
	pNPtags tags = &data->io.gl.hud.tags;

	for (i = 0; i < tags->count; ++i) {

		free(tags->list[i]);
		tags->list[i] = NULL;
	}
	tags->count = 0;

	for (i = 0; i < tags->recordCount; ++i) {

		free(tags->recordList[i]);
		tags->recordList[i] = NULL;
	}
	tags->recordCount = 0;

	for (i = 0; i < tags->sortCount; ++i) {

		//free(tags->sort[i]);
		tags->sort[i] = NULL;
	}
	tags->sortCount = 0;

	//perhaps should lock list during this process for thread safety
	return;
}

//------------------------------------------------------------------------------
void npDeleteTag (int recordID, int tableID, void* dataRef)
{
	//remove tag from list and compact the gap
	//perhaps should lock list during this process for thread safety
	return;
}


//------------------------------------------------------------------------------
void npAddTag (pNPtag tag, void* dataRef)
{
	//insert tag in the list sorted first by tableID then recordID
	//perhaps should lock list during this process for thread safety
	return;
}

//------------------------------------------------------------------------------
pNPtag npGetTag (int recordID, int tableID, void* dataRef)
{
	int i = 0;
	pNPtag tag = NULL;
	pData data = (pData) dataRef;

	//using a simple loop for now, replace with HASH method, debug zz
//	for (i=0; i < data->map.tags.count; i++)

	return tag;
}

int tagsIndex;	//zzhp debug, move this to the global data struct
//------------------------------------------------------------------------------
void npSetNodeTag (pNPnode node, void* dataRef)
{
	pData data = (pData) dataRef;

	static int count = 0;	//zz debug, this method assumes the record_id is ordered, add sort flag for when note ordered
	int lastCount = count;
	int i = 0;

	pNPrecordTag recordTag = NULL;
	pNPtags tags = &data->io.gl.hud.tags;


	//using a simple loop for now, replace with HASH method,			//zz debug, was i=0
	//starts searching at the last location accessed, rolls over if not found
	//this procedure is fast when nodes processed in order of record_id
	//and tags are pre-sorted by record_id and rouped by table_id
	//method is very slow for missing tags... loops entire list looking	//zz debug
//	for (i = 0; i < tags->recordCount; i++)
	for (i = tagsIndex; i < tags->recordCount; i++)
	//	for (i=count; i < data->io.gl.hud.tags.recordCount; i++)
	{
		recordTag = tags->recordList[i];
		if (recordTag == NULL)
		{
			printf("err 2985 - npSetNodeTag null tag in recordList\n");
			continue;
		}

		//if we find a matching record and table then replace contents
		if ( node->recordID == recordTag->recordID
			&& node->tableID == recordTag->tableID )
		{
			//strcpy (node->tag->title, recordTag->title);
			node->tag->title = recordTag->title;
			//strcpy (node->tag->desc, recordTag->desc);
			node->tag->desc = recordTag->desc;
			node->tag->titleSize = recordTag->titleSize;
			node->tag->descSize = recordTag->descSize;

			i = tags->recordCount;	//exit loop
		}
		else
		{
			tagsIndex++;			//zzhp keeps track of our current position
			recordTag = NULL;
		}

		//zz debug, could add err checking if out of order then sort and re-process
		count++;
	}

/*
	//if not found in last half of list then check first half
	if (recordTag == NULL)
	{
		for (i=0; i < lastCount; i++)
		{
			recordTag = data->io.gl.hud.tags.recordList[i];

			//if we find a matching record and table then replace contents
			if (node->recordID == recordTag->recordID
				&& node->tableID == recordTag->tableID )
			{
				strcpy (node->tag->title, recordTag->title);
				strcpy (node->tag->desc, recordTag->desc);
				node->tag->titleSize = recordTag->titleSize;
				node->tag->descSize = recordTag->descSize;

				i = lastCount;	//exit loop
			}
			else
				recordTag = NULL;
			//zz debug, could add err checking if out of order then sort and re-process
		}
		count = i;
	}
*/
	
	//if no tag matches the recordID and tableID
	if (recordTag == NULL)
	{
		node->tag->titleSize = sprintf (node->tag->title, "record_id: %d", 
										node->recordID);
		node->tag->desc[0] = '\0';
		node->tag->descSize = 0;
	}
}


//------------------------------------------------------------------------------
void npTagNode (pNPnode node, void* dataRef)
{
	//if tag is null then allocate memory and init a tag
	if (node->tag == NULL)
	{
		node->tag = malloc(sizeof(NPtag));
		npInitTextTag (node->tag, dataRef);
	}

	//if no recordID then set the tag title to the nodeID
	if (node->recordID == 0){
		sprintf(node->tag->title, "id: %d", node->id);
	}
	else
		npSetNodeTag (node, dataRef);

	npUpdateTextTag (node->tag); 
}

//------------------------------------------------------------------------------
void npNodeTraverseTree ( void (*nodeFunc)(pNPnode node, void* dataRef), 
						  pNPnode node, void* dataRef)
{
	int i = 0;

	//process root node last
	nodeFunc(node, dataRef);

	for (i=0; i < node->childCount; i++)
	{
		// recursively call this function
		npNodeTraverseTree (nodeFunc, node->child[i], dataRef);
		
		// apply passed in function operation to node
		nodeFunc(node->child[i], dataRef);
	}
}

//calculate the box size based on text strings and font
//upgrade to calculate live values such as channel mapped attributes, zz
//for example coordinates of a moving object
//------------------------------------------------------------------------------
ANTZCORE_API void npUpdateTextTag(pNPtag tag)
{
	int lineCount = 1;
	float charWidth = 9.0f;		//add procedure based on font type
	float charHeight = 15.0f;

//	if(tag->titleSize == 0)											//zz debug
		tag->titleSize = (int)strlen(tag->title);

	//add procedure to count lines and width length
	//strlen(tag->desc);
	tag->boxSize.x = 10.0f + charWidth * (float)tag->titleSize;
	tag->boxSize.y = 6.0f + charHeight * (float)lineCount;
}

//------------------------------------------------------------------------------
int npCompareNodes (const void* a, const void* b)
{
//	const pNPnode nodeA = a;
//	const pNPnode nodeB = b;
	pNPnode nodeA = *(pNPnode*)a;
	pNPnode nodeB = *(pNPnode*)b;

	if (nodeA->tableID != nodeB->tableID)
	{
		if (nodeA->tableID > nodeB->tableID)
			return 1;
	}
	else
	{
		if (nodeA->recordID > nodeB->recordID)
			return 1;

		if (nodeA->recordID == nodeB->recordID)
			return 0;
	}

	//if none of the abover then b > a
	return -1;
}

//------------------------------------------------------------------------------
int npCompareTags (const void* a, const void* b)
{
	pNPtag tagA = *(pNPtag*)a;
	pNPtag tagB = *(pNPtag*)b;

	if (tagA->tableID != tagB->tableID)
	{
		if (tagA->tableID > tagB->tableID)
			return 1;
	}
	else
	{
		if (tagA->recordID > tagB->recordID)
			return 1;

		if (tagA->recordID == tagB->recordID)
			return 0;
	}

	//if none of the abover then b > a
	return -1;
}

//called after loading a new state file or tag list file, performance counts //zzhp
//------------------------------------------------------------------------------
ANTZCORE_API void npSyncTags(void* dataRef)
{
	pData data = (pData) dataRef;

	int i = 0, j = 0;
	int count = 0;
	void** nodes;
	void** tags;

	data->map.syncNodes = false;		//reset sync flags
	data->map.syncTagsReady = false;

						
	//allocate a pair of destination buffers for the nodes and tags
	nodes = npMalloc(0, sizeof(pNPnode) * kNPnodeMax, data);			//data->map.nodeCount
	if (!nodes) return;
	tags = npMalloc(0, sizeof(pNPtag) * kNPnodeMax, data);			//update to use tag count //zz debug
	if (!tags) return;

	//populate the sort lists, skip over null nodes
	for (i=0; i < kNPnodeMax; i++)	//data->map.nodeCount; i++)
		if (data->map.nodeID[i] != NULL)
			nodes[j++] = data->map.nodeID[i];

	//sort nodes and tags grouped first by table_id and second by record_id
	npSort(nodes, j, sizeof(void*), npCompareNodes, data);
	npSort(data->io.gl.hud.tags.list,
			data->io.gl.hud.tags.count,
			sizeof(void*), npCompareTags, data);	//update to use tag count //zz debug

	//loop through all nodes and update with tag
	tagsIndex = 0;	//reset the index, takes advantage of sorted lists, zzhp
	for (i = 0; i < j; i++){
		npTagNode(nodes[i], data);
	}

	//loop through all nodes and attach tagPtr based on recordID and tableID
//	for (i=0; i < data->map.nodeRootCount; i++)
//		npNodeTraverseTree (npTagNode, data->map.node[i], dataRef);
}
