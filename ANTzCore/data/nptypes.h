/* -----------------------------------------------------------------------------
*
*  nptypes.h
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

#ifndef NPTYPES_H_
#define NPTYPES_H_

#include <time.h>
#include <stdio.h>

#ifndef __cplusplus
#include "stdbool.h"
#endif

#ifndef WIN32
#define TRUE 1
#define FALSE 0
#endif

#define kPI			 3.141593f
#define k2PI		 6.283185f
#define kNeg2PI		-6.283185f
#define kRADtoDEG	57.29578f

#define	kNPtextureCountMax	2000

#define kNPkeyMapSize		256				// keyboard map
#define kNPkeyEventTypeSize 8				// 2^3 SHIFT, CTRL and ALT combos
						  //67108864 = 64MB 2^26	
#define kNPmapFileBufferMax	16000000		//512MB = 536870912, was 128MB current file size limit  //zzhp
#define kNPfileBlockSize	4194304			//65535 4MB 2^22 = 4194304 //65535 //

#define	kNPnodeMax			4194304			// 2097152 8MB with 32bit ptr, 16MB if 64bit	//zzhp
#define kNPnodeRootMax		4194304			// 1048576 4MB with 32bit ptr, 8MB if 64bit	//zzhp 262144
#define kNPnodeChildMax		266				// 1KB RAM each node with 32bit OS	//zzhpa
											// C99 max fixed array size is 16383
											// 266 fills a sphere at 15 deg, possibly switch data structure to GTK.org zz

#define kNPbranchLevelMax	28				// max branch depth for child nodes, zz debug add error handling
											// limit of GL matrix stack max 32

#define kNPcameraCount		4				// default number of camera views

#define kNPfacetCube		6				// cube facet count

#define kNPcubeLength		1.4142136f		// cube edge length is sqrt(2)
#define kNPsphereRadius		1.0f			// default radius of a sphere
#define kNPcylinderRadius	1.0f			// default radius of a cylinder
#define kNPtorusRadius		1.5f			// default radius of a torus
#define kNPdefaultRatio		0.1f			// default innerRadius of a torus

#define kNPoffsetUnit		1.0f			// default surface offset
#define kNPoffsetCube		0.7071068f //(0.5f * kNPcubeLength)	// cube center height
#define kNPoffsetTorus		0.75f //(0.5f * kNPtorusRadius)	// half torus radius
#define kNPoffsetPin		5.0f			// cone point to center of dome
#define kNPoffsetRod		10.0f			// rod length

#define	kNPscaleDefault		0.5f;
#define	kNPscaleRod			1.0f;			// parent rod, child not scaled
#define	kNPscalePin			1.0f;			// parent pin, non-pin child
#define	kNPscalePinPin		0.5f;			// parent and child pin scale 1/2
#define kNPscaleCube		0.5f;			// parent cube, child is 1/3
#define kNPscaleSphere		0.25f;			// parent sphere, child is 1/4
#define kNPscaleCylinder	0.25f;			// parent cylinder, child is 1/4
#define	kNPscaleTorus		0.25f;			// parent torus, child is 1/4
#define	kNPscaleTorusTorus	0.5f;			// parent and child torus scale 1/2

#define kNPgridSpacing		30.0f			// defualt grid spacing, was 10.0
#define kNPgridSegmentsX	12				// default grid 16x16, was 16
#define kNPgridSegmentsY	6				// default grid 16x16, was 16

#define kNPpaletteSize		20				// index mapped color pallete

#define	kNPmaxPath			4096			// max file path, msw supports 260
#define	kNPurlMax			4096			// maximum length of URL accepted

#define kNPdataTypeMax		4096			//used for CSV header field names
#define kNPbaseItemCount	18
#define kNPnodeItemCount	52
#define kNPcameraItemCount	17
#define kNPgridItemCount	5
#define kNPpinItemCount		12				//important to keep these up-to-date

#define kNPtagItemCount		5				// debug db //zzsql
#define kNPChMapItemCount	7				// debug db //zzsql

#define kNPmenuPerPageMax			20
#define kNPconsoleLineMax		40			//default is 80x40 character layout
#define kNPconsoleCharPerLine	80
#define kNPconsoleHistoryMax	800			//max entries to store in RAM
#define kNPconsoleInputMax		4096		//max user input string
#define kNPmessageQueMax		500			//max messages stored in que
#define kNPmessageLengthMax		127			//max message length

#define kNPtitleSizeMax			63			// max length of the tag title
#define kNPdescSizeMax			255		// max length of the tag description	//zzhpa
#define	kNPtagDrawMax			16383		// max number tags allowed to draw
#define	kNPrecordTagListMax		kNPnodeMax	// max number of record Tags
#define kNPlinkQueMax			kNPnodeMax	// max in draw que, not total nodes

#define kNPsingleClickTime		0.45		//max duration considered to be a click
#define kNPdoubleClickTime		0.45		//max duration for double click 

#define kNPinputStrMax			4096		//console max length user input

#define kNPmaxTrackToAttributeMappings 50000 //JJ maximum number records in chmap file, of mappings supported (fixed size array)
#define kNPattributeNameOffset	3			//JJ attribute name column in the ChMap file:  id, ch, track, attribute name, id1, id2, id3
#define kNPchannelOffset		1			//JJ channel offset column in the ChMap file:   0   1    2         3           4    5    6
#define kNPtrackOffset			2			//JJ track offset column in the ChMap file:     0   1    2         3           4    5    6

#define kNPchannelMax			128			// max number of channels
#define kNPtrackBufferSize		65535		//JJ number of samples possible in the track buffer, NOTE: this is a circular
											// buffer, the maximum size available to antz at any moment
#define kNPmaxTracks			1000		//JJ maximum number of tracks supported on a channel
#define kNPmaxFloatProperties	500000		//JJ maximum number of float properties fed by channel tracks
#define kNPmaxUcharProperties	500000		//JJ maximum number of unsigned char properties fed by channel tracks
#define kNPmaxLineLength		10000		//zz-JJ move to nptypes.h
#define kNPmaxTokens			1000		//zz-JJ move to nptypes.h
#define kNPmaxPropertiesMapped	500000		//JJ maximum number of node properties (one set for each data type) fed by channel tracks

#define kNPwindowWidth 800
#define kNPwindowHeight 450
#define kNPwindowPositionX 40
#define kNPwindowPositionY 40

//------------------------------------------------------------------------------
// Base Types - designed to be directly compatible with OpenGL

struct NPfloatXY {
	float x,y;
};
typedef struct NPfloatXY NPfloatXY;
typedef struct NPfloatXY *pNPfloatXY;

struct NPfloatXYZ {
	float x,y,z;
};
typedef struct NPfloatXYZ NPfloatXYZ;
typedef struct NPfloatXYZ * pNPfloatXYZ;

struct NPfloatXYZA {
	float x,y,z,angle;
};
typedef struct NPfloatXYZA NPfloatXYZA;
typedef struct NPfloatXYZA *pNPfloatXYZA;

struct NPfloatXYZS {
	float x,y,z,scaler;
};
typedef struct NPfloatXYZS NPfloatXYZS;
typedef struct NPfloatXYZS *pNPfloatXYZS;

struct NPfloatRGB {
	float r,g,b;
};
typedef struct NPfloatRGB NPfloatRGB;
typedef struct NPfloatRGB *pNPfloatRGB;

struct NPfloatRGBA {
	float r,g,b,a;
};
typedef struct NPfloatRGBA NPfloatRGBA;
typedef struct NPfloatRGBA *pNPfloatRGBA;

struct NPintXY {
	int x,y;
};
typedef struct NPintXY NPintXY;
typedef struct NPintXY *pNPintXY;

struct NPintXYZ {
	int x,y,z;
};
typedef struct NPintXYZ NPintXYZ;
typedef struct NPintXYZ *pNPintXYZ;

struct NPintXYZS {
	int x,y,z,s;
};
typedef struct NPintXYZS NPintXYZS;
typedef struct NPintXYZS *pNPintXYZS;

typedef unsigned char NPubyte;

struct NPubyteRGBA {
	unsigned char r,g,b,a;
};
typedef struct NPubyteRGBA NPubyteRGBA;
typedef struct NPubyteRGBA *pNPubyteRGBA;

struct NPboolXYZ {
	bool x,y,z;
};
typedef struct NPboolXYZ NPboolXYZ;
typedef struct NPboolXYZ *pNPboolXYZ;

struct NPboolXYZS {
	bool x,y,z,s;
};
typedef struct NPboolXYZS NPboolXYZS;
typedef struct NPboolXYZS *pNPboolXYZS;

typedef void (*NPfuncPtr)(int menuItem, void* dataRef);
//------------------------------------------------------------------------------
// Custom Types - multiple instances -------------------------------------------

typedef void (*voidFnPtr)(void*);	//zz-JJ


struct NPmapType
{
	int		id;
	int		type;
	char*	name;
	char*	desc;
};
typedef struct NPmapType NPmapType;
typedef struct NPmapType *NPmapTypePtr;


//zz-JJ
//zz-osc debug, dynamic would be better, but for now fixed size limit
#define kNPnameMax 261
#define kNPelementMax 1024

#define kNPoscListMax 256

//approx 3KB record
//zz may allow mapPtr to also be a relative pointer offset... or something like that
//idea is to support dynamically creating a new C-struct and schema mapping
struct NPmapLink
{
	// C-struct mapPtr to the data in memory with ID and type mapping
	void*	mapPtr;	 //element ptr		//pointer to an element of the C data map structure	
//	int		idA;
	int		typeA;
	int		elementID;	//typeA		//mapPtr = npGetMapID(mapID); //and vice versa

//	void*	mapPtrB;		//element ptr	//add this for channel ptr-pair updates? or separate list...	
//	int		idListB;		//id lists with range {22, 23, 44, [55:65]}
//	int		typeB;			//element base type
//	void**	elementListB;	//element type, could be a pointer offset, but a list is faster to process

//	int		typeA;			//base data type //mapPtr (c-struct) data type
	// human-readable Descriptor 

	int		direction;		//input output direction +1 = A->B, 0 = A<->B, -1 = B->A

	char	mapPathA[kNPnameMax];		//db table path or RAM ptr by name
	int		itemA;						//itemID = np_map_row_id = array[item]
	char	elementA[kNPelementMax];		//name of the sub-member object
	char	typeTagA[kNPnameMax];

	// external schema mapping and formatting for OSC, CSV, JSON, MySQL...
	char	mapPathB[kNPnameMax];		//formatID by name, 'osc_mrmr'
	int		itemB;						//item ID, array index, osc_id
	char	elementB[kNPelementMax];	//OSC address, URL, table field name
	char	typeTagB[kNPnameMax];			//OSC type tag or data type

	char	name[kNPnameMax];
	char	desc[kNPnameMax];

	char	value[kNPelementMax];
	};
typedef struct NPmapLink NPmapLink;
typedef struct NPmapLink *pNPmapLink;
// approx 3KB human-readable Descriptor Record
// zz may allow mapPtr to also be a relative pointer offset... or something like that
// idea is to support dynamically creating a new C-struct and schema mapping
// auto-convert typeID <-> OSC Type Tag style, and other 3rd party tag types
// unknown incoming auto-create new type tag record and refer to it
struct NPmapPair
{
	// C-struct mapPtr to the data in memory with ID and type mapping
	void*	mapPtrA;		//pointer to internal map element c-struct

	char	mapPathA[kNPelementMax];	//mapPtr = npGetMapID(mapID); //and vice versa
	int		itemA;						//item ID = np_map_row_id = array[item]
	char	elementA[kNPelementMax];	//element and type are associated
	int		typeA;						//mapPtr type <-auto-> OSC typeTag

	// external schema mapping and formatting for OSC, CSV, JSON, MySQL...
	char	mapPathB[kNPnameMax];		//db table path or RAM ptr by name  //formatID by name, 'osc_mrmr'
	int		itemB;						//item ID, array index, osc_id
	char	elementB[kNPelementMax];	//name of the sub-member object //OSC address, URL, table field name
	char	typeTagB[kNPnameMax];		//OSC type tags can be converted to type_id

//	char*	value;						//data contents as str or binary bit stream

	//char	title[kNPnameMax]			//human readable text displayed in UI
	//char	desc[kNPnameMax];			//desc, notes, aux data... no size limit

	//void** valueList;					//each base type component
	};
typedef struct NPmapPair NPmapPair;
typedef struct NPmapPair *pNPmapPair;

struct NPrecordSet						//generic list struct
{
	void**	list;
	int		count;
	int		type;

	int		size;
};
typedef struct NPrecordSet NPrecordSet;
typedef struct NPrecordSet* pNPrecordSet;

struct NPoscItem {
	int		id;

	char	txIP[kNPurlMax];	//supports ipv4 ipv6 or URL
	int		txPort;

	char	rxIP[kNPurlMax];
	int		rxPort;

	int		size;
};
typedef struct NPoscItem NPoscItem;
typedef struct NPoscItem* pNPoscItem;

struct NPosc {
	int			id;

	NPoscItem**	list;			//list of que items or commands
	int			count;			//number of items in list
	int			max;		//max size of que

	int			msgMode;		//0 = quiet, 1 = system, 2 = GUI
	int			logMode;		//0 = none, 1 = err only, 2 = all

	int			size;			//active memory used
};
typedef struct NPosc NPosc;
typedef struct NPosc* pNPosc;


//zz-q que and queList could be merged and made a recursive method, perhaps confusing?
enum {
	kNPfifoNull = 0,
	kNPfifoCmd,			//for command buffers that contain NPqueCmd(s)
	kNPfifoMapItem,		//designates buffer has NPqueItem(s)
	kNPfifoCount
};
/* basic concepts at http://en.wikipedia.org/wiki/Queue_(abstract_data_type)
* our high-performance queue uses double-buffered fixed memory arrays
* it does NOT use dynamically linked lists to reduce ptr overhead
* if overflow occurs it either overwrites oldest data or resizes the queue
* this depends on the 'resize' flag, if true then overflow will only occur
* when memory is capped by the apps total allocatable RAM.
* npRealloc() is called to resize, it uses runtime optimized memory pools
* in certain cases there is no data copy cost as the memory pool
* may already be configured with additional free space at end of the array
* A & B buffers are sequentially re-allocated over two program synce cycles
* avoids data copy by syncing the resize to the AB swap when the que is clear
*/
struct NPqueCmd {
	void*	funcPtr;
	void*	data;
	int		type;			//function ptr type
};
typedef struct NPqueCmd NPqueCmd;
typedef struct NPqueCmd* pNPqueCmda;

											// may combine these types	//zz streamline
struct NPqueItem {
	void*	mapPtr;
	void*	data;
	int		type;			//mapPtr type... either data or cmd funcPtr //zz streamline
};
typedef struct NPqueItem NPqueItem;
typedef struct NPqueItem* pNPqueItem;


struct NPque {
	int			id;
	int			type;			//command or data (NP)type

	NPqueItem**	list;			//list of que items or commands
	int			enque;			//back enque write index cannot pass deque
	int			deque;			//front deque read index chases enque
	int			max;			//max size of que

	bool		dynamic;		//flag triggers resize on overflow

	int			size;			//active memory used
};
typedef struct NPque NPque;
typedef struct NPque* pNPque;

struct NPqueList {
	int			id;
	int			type;			//command or data (np type)

	NPque**		list;			//list of que items or commands
	int			count;			//number of items in list
	int			max;			//max size of que

	int			size;			//active memory used
};
typedef struct NPqueList NPqueList;
typedef struct NPqueList* pNPqueList;


struct NPthreadFile
{
	FILE* file;
	char* filePath;
	void* dataRef;
};
typedef struct NPthreadFile NPthreadFile;
typedef struct NPthreadFile *pNPthreadFile;

struct NPrecordTag
{
	int			id;					//the first column is table id key

	int			recordID;			//recordID of the node
	int			tableID;			//tableID of the node, may not need these, debug zz

	char		title[kNPtitleSizeMax + 1];	//one line with max width of 63 characters
	char		desc[kNPdescSizeMax + 1];	//allows for newline in desc
	
	int			titleSize;			//number of characters in title
	int			descSize;			//number of characters in description

	int			size;
};
typedef struct NPrecordTag NPrecordTag;
typedef struct NPrecordTag *pNPrecordTag;

struct NPtag
{
	int			tableID;			//tableID of the node, may not need these, debug zz
	int			recordID;			//recordID of the node

	char		title[kNPtitleSizeMax + 1];	//one line with max width of 63 characters
	char		desc[kNPdescSizeMax + 1];	//allows for newline in desc
	
	int			titleSize;		//number of characters in title
	int			descSize;		//number of characters in description

	void*		font;

	int			mode;			//2D, 3D billboard, fixed in model space
	float		lineWidth;		//1 line title only, 3 lines, max lines

	NPfloatXY	boxSize;

	NPubyteRGBA color;			//used for the text and box outline
	NPubyteRGBA boxColor;		//background color of box
	NPubyteRGBA lineColor;		//box outline

	int			size;
};
typedef struct NPtag NPtag;
typedef struct NPtag *pNPtag;


// nodes are organized as a branched tree, supports complexity of linked objects
// ie a recNode of type nodeCamera may have a multiple depth daughter tree that
// supports several cameras or videoNodes for circle cam rigs etc...
//-----------------------------------------------------------------------------
typedef struct NPnode NPnode;
typedef struct NPnode *pNPnode;
struct NPnode
{
	int			id;							//local node ID 
	int			type;						//node type, point, pin, cam, video

	void*		data;						//node type specific data
	
	int			selected;					//true if node currently selected

	pNPnode		parent;						//parent node, binary tree of nodes
	int			branchLevel;				//0 is the trunk, 1 is 1st branch
											
	pNPnode		child[kNPnodeChildMax];		//children attached to this one	//zz debug, update to use a circular linked list
	pNPnode		next;						//next sibling node in linked list, N
	pNPnode		prev;						//previous sibling in ring
	int			childIndex;					//the currently selected child
	int			childCount;					//current number of children

	int			chInputID;					//track to node channel_id map
	int			chOutputID;					//node to track record channel_id
	int			chLastUpdated;				//cycle of last channel update

	int			average;					//averaging type applied to data
	int			interval;					//the interval to be averaged

	NPfloatXYZ	auxA;						//reserved for future use
	NPfloatXYZ	auxB;						//reserved for future use

	float		colorShift;					//pseudo Hue shift, -180 to 180

	NPfloatXYZA	rotateVec;					//orientation
	NPfloatXYZ	scale;						//scale node XYZ
	NPfloatXYZ	translate;					//location relative to origin
	NPfloatXYZ	tagOffset;					//origin relative to parent origin

	NPfloatXYZ	rotateRate;					//rotation rate
	NPfloatXYZ	rotate;						//rotation angle in polar radians
	NPfloatXYZ	scaleRate;					//scale rate 
	NPfloatXYZ	translateRate;				//node velocity
	NPfloatXYZ	translateVec;				//velocity unit vector

	int			shader;						//the texture ID or procedural shader type
	int			geometry;					//cube, sphere, pin, FFT mesh...

	float		lineWidth;					//draws lines, zero for off
	float		pointSize;					//draws dots, zero for off
	float		ratio;						//geometry ratio, torus innerRadius

	int			colorIndex;
	NPubyteRGBA	color;						//color assigned to new data

	int			colorFade;					//cycles to fade color alpha, 0=off
	int			textureID;					//GL texture ID

	bool		hide;						//hide the node, data stays active
	bool		freeze;						//freezes data and physics updates

	int			topo;						//topography, cube, sphere, torus
	int			facet;						//topo face number

	NPintXYZ	autoZoom;					//scales node to fit screen, scroll, center to node origin

	NPintXYZ	triggerHi;					//triggers are multipurpose
	NPintXYZ	triggerLo;
	NPfloatXYZ	setHi;						//triggers
	NPfloatXYZ	setLo;

	NPfloatXYZ	proximity;					//collision engine boundary extent
	NPintXYZ	proximityMode;				//particle, wave and matter interactions
		
	NPintXYZ	segments;					//geometry segments, facets, grid lines

	int			tagMode;					//billboard tag
	int			formatID;					//DB translation to antz field, label
	int			tableID;					//DB table
	int			recordID;					//DB recordID

	int			size;

	//internal references and draw calculations not needed in CSV or DB file
	pNPtag		tag;						//ptrr to the nodes own text tag
	NPfloatXYZ	screen;						//MB-LABEL screen pixel coordinates
	NPfloatXYZ	world;						//child node world coordinates
	float		distFromCamera;				//MB-Transp					//zz debug
	int			hudType;					//removethis, use existing param //zz debug
	bool		linkFlag;					//for establishing link nodes
};



//------------------------------------------------------------------------------
// Global Data Structure -------------------------------------------------------

struct NPkey {
	bool	modAlphaUpper;		//combined shift and caps lock result

	bool	modCapsLock;		//true when light on,		//zz debug currently not testing the capslock state

	bool	modShift;			//combined event state
	bool	modCtrl;
	bool	modAlt;
	bool	modCommand;			//only on OSX

	bool	modShiftLeft;		//raw key states
	bool	modShiftRight;
	bool	modCtrlLeft;
	bool	modCtrlRight;
	bool	modAltLeft;
	bool	modAltRight;
	bool	modCommandLeft;
	bool	modCommandRight;
	
	int		map[kNPkeyEventTypeSize][kNPkeyMapSize];	//key command map

	int		size;
};
typedef struct NPkey NPkey;
typedef struct NPkey * pNPkey;

struct NPtool {
	int type;					//same as mouse->tool
	int mode;					//orbitXY, fly, translate... set by button state
	int active;					//targeting, dragging, actively using

	NPfloatXYZ	translate;		//current position... relative to world local or screen coords?
	NPfloatXYZA	rotate;			//current angle

	NPfloatXYZ  prevT;			//previous position
	NPfloatXYZA prevR;			//previous angle

	NPfloatXYZ  deltaT;			//current - previous
	NPfloatXYZA deltaR;

	NPfloatXYZ	coordA;			//start point, mouse down
	NPfloatXYZ	coordB;			//second point to define region
	
	NPfloatXYZA	rotateA;		//support for 6DOF devices
	NPfloatXYZA rotateB;

//	int selectionSetID;
//	void* NPselect;
};
typedef struct NPtool NPtool;
typedef struct NPtool * pNPtool;

struct NPmouse {
	int			x;						//raw coordinate
	int			y;
	int			z;						//typically the scroll wheel

	int			mode;
	int			pickMode;				//locks the mouse into cam mode
	int			tool;

	bool		buttonL;				//true when pressed
	bool		buttonC;
	bool		buttonR;

	bool		singleClick;
	bool		doubleClick;

	NPintXYZ	window;					//window position in pixels

	NPintXYZ	previous;

	float		cmDX;
	float		cmDY;

	NPfloatXYZ	delta;					//mouse vector
	NPfloatXYZ	deltaSum;				//sum of deltas, clears each NPE period

	NPfloatXYZ	target;
	NPfloatXYZ	targetDest;
	float		targetRadius;
	float		targetRadiusDest;
	int			targeting;

	bool		pinSelected;

	bool		createEvent;			//used to create click, but not drag so cam works

	pNPnode		linkA;					//used by link tool to store first pick

	int			size;					// memory used, add/del should modify this, debug zz
};
typedef struct NPmouse NPmouse;
typedef struct NPmouse * pNPmouse;

struct NPmessage
{
	int			queIndex;					//the most recent message

	char		que[kNPmessageQueMax][kNPmessageLengthMax + 1]; //+1 for null

	int			size;
};
typedef struct NPmessage NPmessage;
typedef struct NPmessage * pNPmessage;

//struct pNPdatabases { //zzsql
struct NPdatabases {
	char** list;		//list of databases by name
	int size;			//number of items in the list
};
typedef struct NPdatabases NPdatabases;
typedef struct NPdatabases * pNPdatabases;

struct NPmenu {
	char** list;		//list of databases by name
	char* name;
	char* details;
	int index;			//default or currently selected menu item
	int count;			//number of items in the list
};
typedef struct NPmenu NPmenu;
typedef struct NPmenu * pNPmenu;

struct NPconsole {	
	int			selected;					//true if node currently selected

	NPfloatXYZ	translate;					//location relative to origin

	float		lineWidth;					//draws lines, zero for off

	int			colorIndex;
	NPubyteRGBA	color;						//color assigned to new data

	int			boxColorIndex;
	NPubyteRGBA	boxColor;					//color assigned to new data

	int			colorFade;					//color alpha fade with time
	int			textureID;					//GL texture ID used for box background

	bool		hide;						//hide the node, data stays active
	bool		freeze;						//freezes data and physics updates

	NPfloatXYZ	screen;
	NPintXY		position;

	int			level;
	int			mode;						//1 line, 3 lines, max lines
	NPfloatXY	box;						//background box size

	void*		font;
	int			justify;

	int			lineMax;					//default is 40 lines
	int			charPerLine;				//default is 80 characters per line

	int			lineIndex;					//the currently displayed line
	int			lineCount;
	int			historyMax;					//max number of lines stored

	NPfuncPtr   pMenuCallback;			//menu item selected callback
	pNPmenu		menu;						//menu list, menubar

	int			page;						//active page, page = 0 for current
	int			selectIndex;				//user selected line

	int			cursorLine;					//current cursor line number
	int			cursorColumn;				//current cursor column number

	int			cursorType;
	bool		cursorShow;					//display a blinking cursor

	int			inputIndex;
	char		inputStr[kNPinputStrMax];	//key input typed by user

	char		line[kNPconsoleLineMax][kNPconsoleCharPerLine+1]; //+1 for null char

	//char*		history[kNPconsoleHistoryMax];	//debug, zz

	int			size;
};
typedef struct NPconsole NPconsole;
typedef struct NPconsole * pNPconsole;

struct NPtags {
	void**		list;
	int			count;	//zz debug, change to listCount

	
	void**		recordList;		//zz debug, change name?
	int			recordCount;

	void**		sort;
	int			sortCount;

	int			size;
};
typedef struct NPtags NPtags;
typedef struct NPtags * pNPtags;

struct NPhud {
	pNPnode		root;

	bool		drawTags;
	bool		drawCompass;
	bool		drawCoord;
	bool		drawFPS;
	bool		drawConsole;
	bool		drawAxes;
	bool		drawMouseMode;
	bool		drawSelectMode;

	NPtags		tags;
	NPconsole	console;
	NPtag		fps;

	int			size;
};
typedef struct NPhud NPhud;
typedef struct NPhud * pNPhud;

struct NPgl {
	int			id;				//the node ID used to store this GL item 

	int			windowID;		//multiple GL contexts can share a windowID
	int			glContext;		//multiple windows can share a GL context
								//multi-GPU support
	char		name[kNPtitleSizeMax];			//name used for window title

	int			width;			//current size both window and fullscreen
	int			height;

	NPintXY		windowSize;		//non-fullscreen window size and position
	NPintXY		position;

	int			pixelDepth;
	float		refreshRate;

	int			fullscreen;
	int			stereo;

	int			normal;
	int			shade;
	int			alphaMode;

	int			textureCount;

	int			subsample;		//zzhp

	int			pickPass;
	int			pickID;

	NPhud		hud;			//one HUD per window

	pNPnode		linkQue[kNPnodeMax];	//zz debug move this to data->map
	int			linkQueCount;

	int			size;
};
typedef struct NPgl NPgl;
typedef struct NPgl * pNPgl;


struct NPmap {
	void**	node;					//root node array, size of kNPnodeRootMax
	void**	sort;					//used for z-sort during GL draw

	int		sortSwap;				//0 for sortA and 1 for sortB			//zzhp
	int		sortSwapFlag;			//sort is done and ready for swap
	void**	sortA;					//double buffered async sort
	void**	sortB;					//double buffered async sort			//zzhp

	void**	nodeID;					//maps nodeID to pNPnode, kNPnodeMax
	void**	sortID;					//maps nodeID for sorting nodes

	int*	parentID;				//maps node ID to parentID
	int*	orphanList;				//list of orphans by node ID
	int		orphanCount;
	int		sortCount;														//zzhp
	int		sortCountA;
	int		sortCountB;														//zzhp

	bool	syncNodes;
	bool	syncTagsReady;
									// 2^24 max for picking algorithm
	int		nodeCount;				//total for root and children nodes
	int		nodeRootCount;			//number of root nodes
	int		nodeRootIndex;			//the active node root
	
	pNPnode	currentNode;			//active node, commands, traversing tree
	pNPnode	currentCam;				//active camera used for zsort distance
	pNPnode	selectedGrid;			//selected grid
	pNPnode	selectedPinNode;		//current node selection, can be a child branch
	int		selectedPinIndex;		//helpful to know which tree we are on
	pNPnode selectedHUD;			//the currently selected HUD item

	NPboolXYZ	selectSet;			//current selection set to add too
	int			selectAll;			//true when all nodes selected

	NPmapTypePtr	typeMap[kNPdataTypeMax];

	NPmapTypePtr	typeMapBase;
	NPmapTypePtr	typeMapNode;
	NPmapTypePtr	typeMapCamera;
	NPmapTypePtr	typeMapPin;
	NPmapTypePtr	typeMapGrid;
		
	NPmapTypePtr	typeMapTag;		// debug db //zzssql 
	NPmapTypePtr	typeMapChMap;	// debug db //zzssql

	pNPmapLink		typeMapGlobals;
	pNPmapLink		typeMapOSC;
	int				globalsCount;
	int				oscCount;

	int		size;					// memory used, add/del should modify this, debug zz
};
typedef struct NPmap NPmap;
typedef struct NPmap * pNPmap;

//JJ-zz
//JJ - currently using track data column naming convention for mapping track-to-node-attribute (bottom part of structure will be unnecessary when channel file exists)
struct NPxref {
	int			channelNumber;
	int			trackOffset;
	char*		attributeName;
};
typedef struct NPxref NPxref;
typedef struct NPxref * pNPxref;

// the pointer logic is same in all cases, processing requires different casting for the "void*" and "void**"
// for example, when processing float properties, these have to be cast to "float*" and "float**"
struct NPnodePropertyTracks {
	// for mapping track-to-node-attribute -- set up when a Node subscribes to a channel 
	void*		nodePropertyMemoryLocations[kNPmaxFloatProperties];		// memory address of each node property fed from track data
	void**		nodePropertyMemoryNextLocation;							// next available location for a float property memory address
	int			propertyTracks[kNPmaxFloatProperties];					// offset of the tracks that feed each peroperty
	int			propertyTrackIndex;										// next available propertyTrack array offset
};
typedef struct NPnodePropertyTracks NPnodePropertyTracks;
typedef struct NPnodePropertyTracks * pNPnodePropertyTracks;

#define kNPchannelMapHeader "id,channel_id,track,attribute,track_table_id,ch_map_table_id,record_id"

// NPtrackDataSource.dataSourceType constants
enum
{
	kDataSourceUndefined,
	kDataFromFile,
	kDataFromUDP
};

#define kTracksDefaultUDPserverPort 1010		// default UDP port is JJ abcdefghij, selected when UDP port not specified
#define kSocketTypeUnspecified -1		// initial value for channel socket trackSocketTypeId

struct NPtrackDataSource {
	//
	//  kDataFromFile -- load data from Files
	//  kDataFromUDP -- load data from UDP
	//
	int			dataSourceType;

	// when dataSourceType is kDataFromFile
	FILE*		trackFP;		// for reading *Ch* files, used in a separate thread, and may remain open
	char*		fullFilePath;
	FILE*		channelFP;

	// when dataSourceType is kDataFromUDP, we use the oscPackListener in NPio as the data source, the message itself 
	// then routes data to the tracks
};
typedef struct NPtrackDataSource NPtrackDataSource;
typedef struct NPtrackDataSource * pNPtrackDataSource;

struct NPch {
	int			setNumberToLoad;

	//
	//  LOADED FROM CHANNEL FILE
	//
	NPxref		xref[kNPmaxTrackToAttributeMappings];
	int			xrefIndex;
	char		channelFileTokenBuffer[kNPmaxLineLength];
	char*		channelFileTokens[kNPmaxTokens];			

	//
	// LOADED FROM TRACKS FILES
	//
	// data source for the tracks
	NPtrackDataSource trackDataSource;
	char		trackFileTokenBuffer[kNPmaxLineLength];
	char*		trackFileTokens[kNPmaxTokens];			

	// properties apply to all tracks on the channel
	bool		updateData;				// true when we need to update data (either read from track to update node, or vice versa)
	int			channelReadIndex;		// offset in all tracks, next data to read
	int			channelWriteIndex;		// offset in all tracks, next data to read
	int			channelDataSize;		// amount of data in each track, any value between 0 and kNPtrackBufferSize
	int			channelSampleRate;		// sample rate for this channel, # of milliseconds to keep same channel data, e.g. 50 === 20 changes per second (1000 ms/50 ms)
	long long	channelNextDataChangeTime;	// next time to change data (by updating channelReadIndex) on this channel, used with sample rate to determine when to change data
	
	// track specific data, data loaded from the track file
	int			numberTracks;			// number of tracks in this channel
	char**		trackNames;				// each track has a name, name indicates node attribute (multiple allowed, separated by anything except ",")

	// all data types created for each track (until we have way of knowing which is needed)
	float**		trackFloatData;			// list of pointers to "float" values, track data as "float"
	unsigned char** trackUcharData;		// list of pointers to "unsigned char" values, track data as "unsigned char"
	int**		trackIntData;			// list of pointers to "int" values, track data as "int"

	NPnodePropertyTracks	floatTracks;// tracks associated with "float" Node properties
	NPnodePropertyTracks	ucharTracks;// tracks associated with "unsigned char" Node properties
	NPnodePropertyTracks	intTracks;	// tracks associated with "int" Node properties

	bool		pause;

	int			size;
};
typedef struct NPch NPch;
typedef struct NPch* pNPch;
//JJ-zz end


struct NPfile
{
	int			loading;				//used to signal processes to pause
	bool		saveSelect;

	char		appPath[kNPmaxPath];
	char		csvPath[kNPmaxPath];
	char		mapPath[kNPmaxPath];
	char		currentOpenPath[kNPmaxPath];
//	char		cwdPath[4096];			//zz debug, maybe better to use GetCWD
	char		userSelectedPath[kNPmaxPath];

	int			size;
};
typedef struct NPfile NPfile;
typedef struct NPfile* pNPfile;

#define kNPqueMax 256

struct NPio {
	int			argc;
	char**		argv;

	NPmessage	message;
	NPkey		key;
	NPmouse		mouse;
	NPgl		gl;
	NPch		ch;
	NPfile		file;

	struct	dbNewConnect *connect;	//zzsql									//zz debug

//	NPoscPackListener oscListener;	//JJ-zz
	NPosc		osc;				//OSC stuff uses io que for thread safety

	NPqueList	fifo;				//io buffering for data and command sync
	//zz should que be part of npmap? ...its a 'io' (data) buffer

	double		time;				//system time in seconds
	double		timeStart;			//time at app start
	double		cycleDelta;			//the actual duration of the last cycle
	double		cycleRateHz;		//target rate, not necessarily the actual
	double		cyclePeriod;		//period calculated from rate
	int			cycleCount;			//number of cycles since app startup

	bool		blinkState;			//blinking cursor state

	bool		clearFrameBuffer;	//1 to clear screen each draw, 0 accumulates
	int			clearIndex;			//background color index, black, white...
	NPfloatRGBA clear;				//background color for clearing the screen //move to gl

	int			drawMenu;

	NPboolXYZ	axes;					//selects active x,y,z, axes			debug, zz

	char*		write;

	char		url[kNPurlMax];

	int			size;
};
typedef struct NPio NPio;
typedef struct NPio* pNPio;

struct NPcpu {
	int multiPartition;		//multi-partition NUMA HPC systems, 16384+ sockets
	int systemNodes;		//number of system nodes in this partition
	int sockets;			//number of CPU sockets
	int cores;				//NUMA systems have up to 2048+ cores per partition
	int threads;			//total threads, hyper-threads

	int socketsPerNode;		//the number CPUs on a single board, shared RAM
	int coresPerCPU;		//1,2,4,6,8,12,16...
	int threadsPerCore;		//typically 1 or 2
	int threadsPerCPU;		//typically either twice the core count or same as

	//memory
	long long cacheL1;		//L1 cache per thread
	long long cacheL2;		//L2 cache per core
	long long cacheL3;		//L3 cache per CPU

	long long multiRAM;		//total RAM across all NUMA partitions
	long long systemRAM;	//total RAM in this NUMA partition

	long long RAM;			//RAM per motherboard or compute blade
	long long appRAM;		//per CPU core

	int		size;
};
typedef struct NPcpu NPcpu;
typedef struct NPcpu* pNPcpu;

#define kNPuserMax 256					//zz select
struct NPuser{							//zz select
	pNPnode	tool;
};
typedef struct NPuser NPuser;
typedef struct NPuser* pNPuser;

struct NPctrl {
	pNPuser user[kNPuserMax];			//zz select

	float	slow;						// slow velocity, shift not pressed
	float	fast;						// fast velocity, shift key pressed 

	NPcpu	cpu;

	void**	cmdFunc;

	int		size;
};
typedef struct NPctrl NPctrl;
typedef struct NPctrl* pNPctrl;

struct Data {
	NPmap	map;
	NPio	io;
	NPctrl	ctrl;

	int		size;
};
typedef struct Data Data;
typedef struct Data* pData;


// for displaying labels related to node parameters
// displays the name at the top, followed by X,Y,Z values in a column
// unitType determines display format and appends the unit label, ie 9.8 m/s^2
// the nodeField (ie, translate.x) is first multiplied by the factor then offset
// displayValue = nodeField * factor + offset
//------------------------------------------------------------------------------
struct NPlabel
{
	char		name[64];			//author, city, population...

	NPfloatXYZS	factor;				//multiplies the value for scaling range
	NPfloatXYZS	offset;				//constant to offset value by

	NPintXYZS	unitType;			//ft, m, F, C, kg/m^3, m/s^2, R,G,B...

//	NPintXYZS	formatType;			//specifies conversion math, y = mx + c

	NPintXYZS	nodeField;			//which node parameter to display
};
typedef struct NPlabel NPlabel;
typedef struct NPlabel *NPlabelPtr;


// cameras can be GL scene cameras or real-world cameras
// video is attached as a daughter of the recNode
//-----------------------------------------------------------------------------
struct NPcamera
{
	int			id;							//node ID
	int			type;

	int			format;						//320p, 480i, 720p, 4K full app
	int			interlaced;					//interlaced field order
	int			stereo;						//stereoscopic 3D
	float		aspectRatio;				//1.0, 1.333, 1.777, 1.85, 2.25... 
	float		fps;						// 15, 24, 29.97, 30, 59.94, 120...
	int			colorSpace;					//8, 12bit, YUV, RGBA, XYZ, CMYK...
	int			width;						//res in pixels
	int			height;

	float		fov;						//FOV 35mm, 70mm...
	float		clipNear;
	float		clipFar;
	
	// real world camera parameters
	float		aperture;					//F stop
	float		exposure;					//in seconds
	int			sensorType;					//3CCD, Debayer pattern...

	float		matrix[16];					//for local to world coord convert
	float		inverseMatrix[16];			//for local to world coord convert

	int			size;
};
typedef struct NPcamera NPcamera;
typedef struct NPcamera *NPcameraPtr;


//-----------------------------------------------------------------------------
struct NPvideo
{
	int			id;							//node ID
	int			type;

	int			format;						//320p, 480i, 720p, 4K full app
	int			interlaced;					//interlaced field order
	int			stereo;						//stereoscopic 3D
	float		aspectRatio;				//1.0, 1.333, 1.777, 1.85, 2.25... 
	float		fps;						// 15, 24, 29.97, 30, 59.94, 120...
	int			colorSpace;					//8, 12bit, YUV, RGBA, XYZ, CMYK...
	int			width;						//res in pixels
	int			height;

	float		brightness;
	float		contrast;
	float		hue;
	float		saturation;
	float		blackLevel;

	NPfloatRGBA	bias;						//color shift
	NPfloatRGBA	gain;						//color multiplier

	int			size;
};
typedef struct NPvideo NPvideo;
typedef struct NPvideo *NPvideoPtr;

//-----------------------------------------------------------------------------
struct NPgis
{
	int			id;							//node ID
	int			type;

	float		*vertexArray;				//the array of vertices
	float		*normalArray;				//the array of normals
	unsigned char	*colorArray;				//each point gets color assigned
	int			vertexCount;				//
	int			normalCount;				//
	int			arrayIndex;					//current vertex index in the array
	//			polyLine
	//			polyStrip

	int			size;
};
typedef struct NPgis NPgis;
typedef struct NPgis *NPgisPtr;



//-----------------------------------------------------------------------------
struct NPsurface
{
	int			id;							//node ID
	int			type;

	float		*vertexArray;				//the array of vertices
	float		*normalArray;				//the array of normals
	unsigned char	*colorArray;				//each point gets color assigned
	int			vertexCount;				//
	int			normalCount;				//
	int			arrayIndex;					//current vertex index in the array

	int			size;
};
typedef struct NPsurface NPsurface;
typedef struct NPsurface *NPsurfacePtr;


//-----------------------------------------------------------------------------
struct NPpoints
{
	int			id;							//node ID
	int			type;
	float**			vertexArray;				//the array of data points
	unsigned char**	colorArray;				//each point gets color assigned
	int			vertexCount;				//number of points in the node
	int			arrayIndex;					//current vertex index in the array

	int			size;
};
typedef struct NPpoints NPpoints;
typedef struct NPpoints *NPpointsPtr;


//-----------------------------------------------------------------------------
struct NPpin
{
	int			id;							//node ID
	int			type;

	float		innerRadius;				//inner radius of the toroid
	float		outerRadius;				//outer radius of the toroid

	int			radiusRatioIndex;			//sorts pre-loaded GPU toriods
	int			screenSizeIndex;			//same as above for detail level
	
	int			slices;						//calculated from radiusRatioIndex
	int			stacks;						//...and from the screenSizeIndex

	NPfloatXYZ	translateTex;
	NPfloatXYZ	scaleTex;
	NPfloatXYZA	rotateTex;					//orientation of the texture map

	int			size;
};
typedef struct NPpin NPpin;
typedef struct NPpin *NPpinPtr;


//-----------------------------------------------------------------------------
struct NPgrid
{
	int			id;							//node ID
	int			type;

	NPfloatXYZ	spacing;
	bool		overlay;					//overlays relative to screen coord

	int			size;
};
typedef struct NPgrid NPgrid;
typedef struct NPgrid * NPgridPtr;


//-----------------------------------------------------------------------------
struct NPnodeData
{
	int			id;							//node ID
	int			type;

	int			index;
	void*		a;			//points to an array of data pointers
	int*		dataType;		//points to an int of the data type,

	int			size;
};
typedef struct NPnodeData NPnodeData;
typedef struct NPnodeData * pNPnodeData;


//-----------------------------------------------------------------------------
struct NPdataItem
{
	int			type;
	void*		item;
};
typedef struct NPdataItem NPdataItem;
typedef struct NPdataItem * NPdataItemPtr;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// constants ------------------------------------------------------------------

enum
{
	kNodeDefault = 0,

	kNodeCamera,			//for both real-world and virtual GL cameras
	kNodeVideo,				//video from live/stream, internal GL, or file
	kNodeSurface,			//ground, sky... background objects drawn first
	kNodePoints,			//for X/XY/XYZ line, eeg/ecg, temp, movement...
	kNodePin,				//combo of cones, spheres, toroids, etc...
	kNodeGrid,				//grid in 1D, 2D or 3D
	kNodeLink,				//links any two nodes
	kNodeHUD,				//HUD elements and tools
	kNodeCount
};

/*
enum
{
	kSurfaceNull = 0,
	kSurfacePlane,
	kSurfaceCylinder,
	kSurfaceCone,
	kSurfaceSphere,
	kSurfaceTorus,
	kSurfacePin,							// ice-cream cone shape
	kSurfaceMesh,							// terrain...
	kSurfaceLayers,							// ie stack of FFT images, MRI...
	kSurfaceCamera,
	kSurfaceTypeCount
};
*/

enum
{
	kSelectNULL = 0,
	kSelectAll,
	kSelectNone,
	kSelectAdd,
	kSelectRemove,
	kSelectColor,
	kSelectParent,
	kSelectSiblings,
	kSelectChildren,
	kSelectProperty
};

enum
{
	kGetNull = 0,
	kNodeColor,
	kNodeChannel,
};

enum
{
	kNPmapNull = 0,
	
	kNPmapRoot,				//kNPmapUniverses
	kNPmapUniverse,			//kNPmapGalaxyCluster,
	kNPmapGalaxy,
	kNPmapSolar,

	kNPmapWorld,			// Earth is 100ms typical, 10ms theoretical
	kNPmapRegion,			// < 30 ms latency, < 4000km  kNPmapLocation,			// < 3 ms latency, < 400km   kNPmapCluster,			// < 1 ms latency, < 40km
	kNPmapNode,				// < 1 ms latency, // < 4m, CPU node, not scene node

	kNPmapSystem, 
	kNPmapIO,				// typically a bus device if a node
						
	kNPmapBUS,				// Node maps
	kNPmapCPU,
	kNPmapRAM,
	kNPmapGPU,

	kNPmapDrive,
	kNPmapNetwork,
	kNPmapVideo,
	kNPmapAudio,

	kNPmapHaptic,
	kNPmapKey,
	kNPmapMouse,
	kNPmapJoystick,
	kNPmapTablet,
	kNPmapTouchScreen,
	kNPmapHID,

	kNPmapNPE,
	kNPmapDraw,				//a list of objects to draw in a GL context 
	kNPmapQuadsort,			//break scene into quadrants, cluster and NPE
	kNPmapZsort,				//GL draw order, for correct alpha rendering

	kNPmapTree,
	kNPmapList,
	kNPmapArray,
	
//	kNPmapNodeNull,
	kNPmapGlobals,

	kNPmapTag,
	kNPmapGL,
	
	kNPmapCSV,			//generic table/row/field/type
	kNPmapOSC,			//default 3rd party OSC mapping
	kNPmapDB,			//native DB format is MySQL
	kNPmapJSON,			//likely the most descriptive
	kNPmapSNMP,			//can bridge SNMP-JSON-OSC
	kNPmapXML,			//for 3rd party data support
	kNPmapKML,			//google KML, related to COLLADA

	kNPmapNP,				//default native handler

	kNPmapNodeCSVvOne,	//zz debug... change these
	kNPmapNodeCSV,		//zz

	kNPmapCount
};

// kNPmapOSC vs kNPformatOSC ...formatJSON ...DB ..SNMP
// kNPmapOSC specifies an OSC map, A-B map pairs for mapping data and commands
// kNPformatOSC specifies an OSC formatted bitstream, could be any contents
enum
{
	kNPformatNull = 0,
	kNPformatCSV,			//primary native file format
	kNPformatDB,			//native DB format is MySQL
	kNPformatOSC,			//primary network protocol
	kNPformatJSON,			//likely the most descriptive
	kNPformatSNMP,			//can bridge SNMP-JSON-OSC
	kNPformatXML,			//for 3rd party data support
	kNPformatKML,			//google KML, related to COLLADA

	kNPformatJPG,			//perhaps should be in  separate list... //zz debug
	kNPformatJ2K,			//...
	kNPformatPNG,			//..
	kNPformatGIF,			//.

	kNPformatAIFF,			//..ditto...
	kNPformatWAV,

	kNPformatCount
};

enum
{
	kShadingNull = 0,
	kShadingWire,
	kShadingFlat,
	kShadingGouraud,
	kShadingPhong,
	kShadingReflection,
	kShadingRaytrace,
	kShadingCount
};

enum
{
	kCameraSensorNull = 0,					//don't know the sensor type
	kCameraSensorMonoCCD,					//monochrome CCD such as BW, IR
	kCameraSensorDebayerCCD,				//single color CCD debayer pattern
	kCameraSensorThreeCCD,					//3CCD color sensor
	kCameraSensorDepthCCD					//BW or color with Z depth data
};

enum										//add DPX format support, zz
{
	kVideoFormatNull = 0,
	kVideoFormatNTSC,
	kVideoFormatPAL,
	kVideoFormat720p,
	kVideoFormat1080i,
	kVideoFormat1080p,
	kVideoFormat2K,
	kVideoFormat4k,
	kVideoFormat8k,
	kVideoFormat12k,
	kVideoFormat16k,
	kVideoFormat24k,
	kVideoFormat32k,
	kVideoFormat48k,
	kVideoFormat64k
};

enum										//add DPX format support, zz
{
	kVideoFPS_Null = 0,
	kVideoFPS_23_976HZ,		//Film on Video
	kVideoFPS_24HZ,			//Film
	kVideoFPS_25HZ,			//PAL
	kVideoFPS_29_97HZ,		//NTSC
	kVideoFPS_30HZ,			
	kVideoFPS_59_94HZ,		//1080i, 60i, 60
	kVideoFPS_60HZ,
	kVideoFPS_72HZ,			//experimental format
	kVideoFPS_119_88HZ,
	kVideoFPS_120HZ
};

enum
{
	kVideoFieldNull = 0,
	kVideoFieldOdd,
	kVideoFieldEven
};

enum
{
	kVideoStereoNull,
	kVideoStereoLeft,
	kVideoStereoRight,
	kVideoStereoSequential
};

enum
{
	kColorSpaceNull = 0,
	kColorSpaceRGB_8,			//bits per channel
	kColorSpaceRGB_10,
	kColorSpaceRGB_12,
	kColorSpaceRGB_16,
	kColorSpaceRGB_24,
	kColorSpaceRGB_32,	
	kColorSpaceRGBA_8,			//bits per channel
	kColorSpaceRGBA_10,
	kColorSpaceRGBA_12,
	kColorSpaceRGBA_16,
	kColorSpaceRGBA_24,
	kColorSpaceRGBA_32,
	kColorSpaceYUV_422_8,		//bits per channel
	kColorSpaceYUV_422_10,
	kColorSpaceYUV_422_12,
	kColorSpaceYUVA_422_8,		//bits per channel
	kColorSpaceYUVA_422_10,
	kColorSpaceYUVA_422_12,
	kColorSpaceYUV_444_8,		//bits per channel
	kColorSpaceYUV_444_10,
	kColorSpaceYUV_444_12,
	kColorSpaceYUV_444_16,
	kColorSpaceYUVA_444_8,		//bits per channel
	kColorSpaceYUVA_444_10,
	kColorSpaceYUVA_444_12,
	kColorSpaceYUVA_444_16,
	kColorSpaceCMYK_8,			//bits per channel
	kColorSpaceCMYK_16,
};

enum
{
	kNPalphaModeNull = 0,
	kNPalphaModeSubtractive,	//default alphaMode
	kNPalphaModeAdditive,
	kNPalphaModeDark,
	kNPalphaModeCount
};

enum {
	kAverageNull = 0,
	kAverage,
	kAverageRootMeanSquare,
	kAverageSmoothed,
	kAverageRunning
};

enum {
	kLeftJustify = 0,
	kRightJustify,
	kCenterJustify
};

enum {
	kSmallFont = 0,
	kMediumFont,
	kLargeFont
};

enum
{
	kChannelNull = 0,
	kChannelCycleCount,
	kChannelOscilloscope
};

enum{
	kNPfileDialogNULL = 0,

	kNPfileDialogNew,
	kNPfileDialogOpen,

	kNPfileDialogSave,
	kNPfileDialogSaveAs,

	kNPfileDialogImport,
	kNPfileDialogExport,

	kNPfileDialogClose
};


// 120Hz timer, 60, 30, 24
// 119.88Hz timer, 59.94, 29.97, 23.976


//---- Ctrl Command Constants ----

enum {
	//global ctrl commands
	kCtrlCommandNull = 0,

	kNPfileNew,
	kNPfileOpen,
	kNPfileClose,
	kNPfileSave,
	kNPfileSaveAs,
	kNPfileImport,
	kNPfileExport,

	kNPfileMapOne,		//used for quick loading and saving of state files
	kNPfileMapTwo,
	kNPfileMapThree,

	kNPcmdConsole,
	kNPcmdViewer,

	//global graph commands
	kNPcmdMenu = 4242,
	kNPcmdTagMode,
	
	kNPcmdNew,
	kNPcmdDelete,

	kNPcmdNext,
	kNPcmdPrev,
	kNPcmdNextBranch,
	kNPcmdPrevBranch,

	kNPcmdCamera,
	kNPcmdGrid,
	kNPcmdLight,
	kNPcmdPin,
	kNPcmdHUD,

	kNPcmdSelect,
	kNPcmdSelectOff,
	kNPcmdSelectToggle,

	kNPcmdSelectAll,
//	kNPcmdSelectAllOn,
//	kNPcmdSelectAllOff,
	kNPcmdSelectNone,
	kNPcmdSelectAllInvert,

	kNPcmdSelectOne,
	kNPcmdSelectTwo,
	kNPcmdSelectThree,
	kNPcmdSelectSetNone,

	kNPcmdPresetOne,
	kNPcmdPresetTwo,
	kNPcmdPresetThree,
	kNPcmdPresetFour,
	kNPcmdPresetFive,
	kNPcmdPresetSix,
	kNPcmdPresetSeven,
	kNPcmdPresetEight,

	//translate, rotate and scale selected objects
	kNPcmdSelectAxes,
	
	kNPcmdXincrease,
	kNPcmdXincreaseOff,
	kNPcmdXdecrease,
	kNPcmdXdecreaseOff,
	
	kNPcmdYincrease,
	kNPcmdYincreaseOff,
	kNPcmdYdecrease,
	kNPcmdYdecreaseOff,
	
	kNPcmdZincrease,
	kNPcmdZincreaseOff,
	kNPcmdZdecrease,
	kNPcmdZdecreaseOff,
	
	kNPcmdRotateLeft,
	kNPcmdRotateLeftOff,
	kNPcmdRotateRight,
	kNPcmdRotateRightOff,

	kNPcmdRotateUp,
	kNPcmdRotateUpOff,
	kNPcmdRotateDown,
	kNPcmdRotateDownOff,

	kNPcmdRotateCCW,
	kNPcmdRotateCCWOff,
	kNPcmdRotateCW,
	kNPcmdRotateCWOff,

	kNPcmdZoomOn,
	kNPcmdZoomOff,
	
	kNPcmdClearFrameBuffer,
	kNPcmdBackground,
	
	//node specific graph commands
	kNPcmdSampleInterval,
	kNPcmdChannelDown,
	kNPcmdChannelUp,
	kNPcmdPause,

	kNPcmdNormalMode,
	kNPcmdShaderMode,
	kNPcmdAlphaMode,

	kNPcmdColorUp,
	kNPcmdColorDown,
	kNPcmdColorFade,
	kNPcmdAltColor,
	kNPcmdColorPallete,
	
	kNPcmdAlphaUp,
	kNPcmdAlphaDown,
	kNPcmdBiasUp,
	kNPcmdBiasDown,
	kNPcmdGainUp,
	kNPcmdGainDown,

	kNPcmdTexture,
	kNPcmdTextureDown,

	kNPcmdFreeze,
	kNPcmdHide,
	kNPcmdClear,
	
	kNPcmdCenter,
	kNPcmdScroll,

	kNPcmdPoints,	
	kNPcmdLines,
	kNPcmdSegments,
	kNPcmdShader,
	kNPcmdGeometry,
	kNPcmdPrimitiveDown,

	kNPcmdTopo,
	kNPcmdTopoDown,
	kNPcmdMesh,

	kNPcmdSetpointLo,											// add kNPcmd..,  debug zz
	kNPcmdSetpointLoOff,
	kNPcmdSetpointHi,
	kNPcmdSetpointHiOff,

	kNPcmdFullscreen,

	kNPcmdRatio,

	kNPcmdTool,
	kNPcmdToolDown,

	kNPcmdSubsample,			//zzhp

	kNPcmdTrigger,

	kNPcmdContext,			//next active (multi-user) ctrl context
	kNPcmdContextID,		//set context by ID, console = 0, main window = 1
	kNPcmdUserFocus,		//set user focus, will bring window to front

	kNPcmdCount
};



enum {	kKeyDown = 1,
		kKeyRepeat,
		kKeyUp,
		kKeyModifier,
		kKeyDownASCII,
		kKeyUpASCII,
		kKeyDownSpecial,
		kKeyUpSpecial
};

enum {	kGlutKeyDown = 1,
		kGlutKeyUp,
		kGlutKeyDownSpecial,
		kGlutKeyUpSpecial
};

//OSX keycodes, are non-ASCII
enum {		kKeyCodeA = 0x00,
				kKeyCodeS,
				kKeyCodeD,
				kKeyCodeF,
				kKeyCodeH,
				kKeyCodeG,
				kKeyCodeZ,
				kKeyCodeX,
				kKeyCodeC,
				kKeyCodeV = 0x09,
				kKeyCodeB = 0x0b,
				kKeyCodeQ,
				kKeyCodeW,
				kKeyCodeE,
				kKeyCodeR,
				kKeyCodeY,
				kKeyCodeT,
				kKeyCode1,
				kKeyCode2,
				kKeyCode3,
				kKeyCode4,
				kKeyCode6,
				kKeyCode5,
				kKeyCodeEqual,
				kKeyCode9,
				kKeyCode7,
				kKeyCodeMinus,
				kKeyCode8,
				kKeyCodeZero,
				kKeyCodeRightBracket,
				kKeyCodeO,
				kKeyCodeU,
				kKeyCodeLeftBracket,
				kKeyCodeI,
				kKeyCodeP,
				kKeyCodeReturn,
				kKeyCodeL,
				kKeyCodeJ,
				kKeyCodeRightQuote,
				kKeyCodeK,
				kKeyCodeSemiColon,
				kKeyCodeBackSlash,
				kKeyCodeComma,
				kKeyCodeSlash,
				kKeyCodeN,
				kKeyCodeM,
				kKeyCodePeriod,
				kKeyCodeTab,
				kKeyCodeSpace,
				kKeyCodeTilda,
				kKeyCodeDelete = 0x33,
				kKeyCodeBackSpace,
				kKeyCodeESC = 0x35,
				
				kKeyCodeF1 = 0x7A,
				kKeyCodeF2 = 0x78,
				kKeyCodeF3 = 0x63,
				kKeyCodeF4 = 0x76,
				kKeyCodeF5 = 0x60,
				kKeyCodeF6 = 0x61,
				kKeyCodeF7 = 0x62,
				kKeyCodeF8 = 0x64,
				kKeyCodeF13 = 0x69,
				kKeyCodeF16 = 0x6A,
				
				kKeyCodeHelp = 0x72,
				kKeyCodeHome = 0x73,
				kKeyCodePageUp = 0x74,
				kKeyCodeDeleteRight = 0x75,
				kKeyCodeEnd = 0x77,
				kKeyCodePageDown = 0x79,
				
				kKeyCodeLeft = 0x7B,
				kKeyCodeRight = 0x7C, 
				kKeyCodeDown = 0x7D,
				kKeyCodeUp = 0x7E,
				
				kKeyCodeNumPadPeriod = 0x41,
				kKeyCodeNumPadStar = 0x43,
				kKeyCodeNumPadPlus = 0x45,
				kKeyCodeNumPadClear = 0x47, 
				kKeyCodeNumPadEnter = 0x4C,
				kKeyCodeNumPadSlash = 0x4B,
				kKeyCodeNumPadMinus = 0x4E,
				kKeyCodeNumPadEqual = 0x51,
				kKeyCodeNumPadZero = 0x52,
				kKeyCodeNumPad1 = 0x53,
				kKeyCodeNumPad2 = 0x54,
				kKeyCodeNumPad3 = 0x55,
				kKeyCodeNumPad4 = 0x56,
				kKeyCodeNumPad5 = 0x57,
				kKeyCodeNumPad6 = 0x58,
				kKeyCodeNumPad7 = 0x59,
				kKeyCodeNumPad8 = 0x5B,
				kKeyCodeNumPad9 = 0x5C
};


// OpenGL types as standard  ---------------------------------------------------------

enum {
	kNPnull = 0,

	//function Pointers
	kNPcmdFuncPtr,
	kNPthreadFuncPtr,

	// data structures
	kNPvoidPtr,
	kNPdataRef, //kNPdata,
	kNPbase,
	kNPmap,
	kNPio,
	KNPctrl,
	kNPglobal,
	kNPnode,
	kNPtag,
	kNPfileBlock,
	kNPlist,

	kNPtagID,
	kNPtagName,
	kNPtagDesc,

	kNPmapItem,

	kNPque,
	kNPqueItem,
	kNPqueCmd,

	kNPcmd,
	kNPnodeData,
	kNPcamera,
	kNPgrid,
	kNPpin,
	kNPchMap,		//remove this, should not be here //zzsql 

	// fundamental C types
	kNPfloat,
	kNPint,
	kNPbyte,
	kNPbool,
	kNPuint,
	kNPubyte,

//	kNPstring,		//char* ASCII null terminated cstr


	kNPcharArray, //zzsql ok here

	// C structures
	kNPboolXYZ,
	kNPboolXYZS,
	kNPintXY,
	kNPintXYZ,
	kNPintXYZS,

	kNPfloatXY,
	kNPfloatXYZ,
	kNPfloatXYZA,
	kNPfloatXYZS,

	kNPubyteRGB,
	kNPubyteRGBA,

	kNPfloatRGB,
	kNPfloatRGBA,

	kNPcstrPtr,
	
	// GL types
	kNPGLboolean,
	kNPGLubyte,
	kNPGLint,
	kNPGLfloat,

	// node element types
	kNPid,
	kNPtype,
	kNPdata,

	kNPselected,
	kNPparent,
	kNPbranchLevel,
	kNPchild,

	kNPchildIndex,
	kNPchildCount,

	kNPchInputID,
	kNPchOutputID,
	kNPchLastUpdated,

	kNPaverage,
	kNPinterval,

	kNPauxA,
	kNPauxB,

	kNPcolorShift,

	kNProtateVec,
	kNPscale,
	kNPtranslate,
	kNPtagOffset,

	kNProtateRate,
	kNProtate,
	kNPscaleRate,
	kNPtranslateRate,
	kNPtranslateVec,

	kNPshader,
	kNPgeometry,
	
	kNPlineWidth,
	kNPpointSize,
	kNPratio,
	
	kNPcolorIndex,
	kNPcolor,
	kNPcolorFade,
	kNPtextureID,
	
	kNPhide,
	kNPfreeze,
	
	kNPtopo,
	kNPfacet,

	kNPautoZoom,

	kNPtriggerHi,
	kNPtriggerLo,
	kNPsetHi,
	kNPsetLo,
	
	kNPproximity,
	kNPproximityMode,

	kNPsegments,

	kNPtagMode,
	kNPformatID,
	kNPtableID,
	kNPrecordID,

	// camera
	kNPformat,
	kNPinterlaced,
	kNPstereo,
	kNPaspectRatio,
	kNPfps,
	kNPcolorSpace,
	kNPwidth,
	kNPheight,
	kNPfov,
	kNPclipNear,
	kNPclipFar,
	kNPaperture,
	kNPexposure,
	kNPsensorType,
	kNPoverlay,

	// pin
	kNPinnerRadius,
	kNPouterRadius,
	kNPradiusRatioIndex,
	kNPscreenSizeIndex,
	kNPslices,
	kNPstacks,
	kNPtranslateTex,
	kNPscaleTex,
	kNProtateTex,

	// grid

	//tags
	kNPtitle,
	kNPdesc,

	//chmap
	kNPchannelID,
	kNPtrackID,
	kNPattribute,
	kNPtrackTableID,
	kNPchMapTableID,	

	// map file
	kNPversion,
	kNPtableTypeCount,
	kNPrecordCount,
	kNPnodeRootCount,

	// launch external app
	kNPopenURL,
	kNPopenApp,
	kNPopenAntz,

	// global types
	kNPgNull,
	kNPgAlphaMode,
	kNPfullscreen,

	kNPgBackgroundRGBA,
	kNPgBackgroundR,
	kNPgBackgroundG,
	kNPgBackgroundB,
	kNPgBackgroundA,

	kNPsubsample,

	NPposition,			//zz-osc
	kNPpositionXY,		//zz debug or should this just be kNPposition?
	kNPpositionXYZ,
	kNPwindowSize,
	kNPwindowSizeXY,
	kNPwindowSizeXYZ,

	kNPgBrowserURL,

	kNPitemCount,	//used at end of lists to determine number of items in list

	kNPsize
};

enum
{
	// GL primitives using GLUT and GLU

	// change to all solid with shader specifying wireframes or solid		//zz debug
	// kNPgeoPoint = 0,
	// kNPgeoLine,			//length 2
	// kNPgeoTriangle,		//length 2
	// kNPgeoQuad,			//length 2
	// kNPgeoPentagon,		//
	// kNPgeoCircle,		//radius 1
	kNPgeoCubeWire = 0,
	kNPgeoCube,				//length 2, currently is something else, what? //zz debug
	kNPgeoSphereWire,
	kNPgeoSphere,			//radius 1
	kNPgeoConeWire,
	kNPgeoCone,				//radius 1, height 2

	kNPgeoTorusWire,
	kNPgeoTorus,			//ratio 0.1, radius 1.5 should we change to 1.0 ?

	kNPgeoDodecahedronWire,
	kNPgeoDodecahedron,		//size ?
	kNPgeoOctahedronWire,
	kNPgeoOctahedron,		//size ?
	kNPgeoTetrahedronWire,
	kNPgeoTetrahedron,		//should have edge length of 2, actual size ?
	kNPgeoIcosahedronWire,
	kNPgeoIcosahedron,		//should have edge length of 2, actual size ?

	kNPgeoPin,				//height 5.5, 5 from tip to center of sphere
	kNPgeoPinWire,								// solid and wire reversed, debug zz

	kNPgeoCylinderWire,
	kNPgeoCylinder,			//radius 1, height 2

//	kNPglutWireTeapot,
//	kNPglutSolidTeapot,

	kNPgeoCount,	//add primitives, triangle, quad, circle, cylinder

	kNPgeoPlot,
	kNPgeoSurface,
	kNPgeoMesh,
	kNPgeoGrid
};

enum
{	
	kNPglNormalNull = 0,
	kNPglRescaleNormal,
	kNPglNormalize,

	kNPglNormalSize
};

enum
{	
	kNPglShadeNull = 0,
	kNPglShadeSmooth,
	kNPglShadeFlat,

	kNPglShadeSize
};

enum
{
	kNPmouseModeNull = 0,

	kNPmouseModeCamLook,

	kNPmouseModeCamFlyA,
	kNPmouseModeCamFlyB,

	kNPmouseModeCamExamXZ,
	kNPmouseModeCamExamXY,
	
	kNPmouseModeDragXZ,
	kNPmouseModeDragXY,
	kNPmouseModeDragWorld,

	kNPdragModeSize
};

enum
{
	kNPcenterNull = 0,
	kNPcenterPerCycle,		//center on the current data point
	kNPcenterPerGrid		//center when current point exceeds 1 grid space
};

enum
{
	kNPnodeRootNull = 0,
	kNPnodeRootCamera,			// root node index for all cameras
	kNPnodeRootGrid,			// root node index for all grids
//	kNPnodeRootLight,
	kNPnodeRootHUD,
	kNPnodeRootPin				// first root node for all pins
};

enum
{
	kNPmodeNull = 0,
//	kNPmodeCombo,			// choose a combo of modes
	kNPmodeCamera,			// pick cameras
	kNPmodeGrid,			// pick grids
	kNPmodePin,				// pick pins
//	kNPmodeHUD,				// modify HUD element
//	kNPmodeAll,				// pick anything
	kNPmodeCount
};

enum		//zz select re-ordered
{
	kNPtoolNull = 0,		// default behavior based on pickMode
	kNPtoolSelect,			// L-click picks object, R-click group, drag region
	kNPtoolCreate,			// L-click creates new node, R-click to delete
	kNPtoolLink,			// L-click to Select A then Select B, R-click to Cancel (re-select A)

	kNPtoolCombo,			// Camera move or Pin select, move, rotate, scale and ratio
							// choose a combo of tools, current default tool

//	kNPtoolPick,			// L-click to Pick an object, R-click for multiple

	kNPtoolTopo,			// L-click next Topo type, R-click reverses
	kNPtoolGeometry,		// L-click next Primitive type, R-click reverses
							// Geometry does not effect Topology
//	kNPtoolSegments,		// L-Click add or R-Click remove segments

	kNPtoolColor,			// L-click next SynGlyphX::Color index, R-click reverses
	kNPtoolTexture,			// L-click next Topo type, R-click reverses
	kNPtoolHide,			// L-click Hide branches, R-click Show branches
							// '~' key to Show ALL, '4' Hide ALL at branch level > 1
	kNPtoolInfo,			// L-click toggles Text Tag, R-click hides Tag

	kNPtoolAlpha,			// L-Click reduces Alpha color, R-Click increases

	kNPtoolMove,			// L-drag Move in XY, R-drag for XZ
	kNPtoolRotate,			// L-drag Rotate XY, R-drag for Z
	kNPtoolSize,			// L-drag Scale XYZ axes or R-drag for torus Ratio

	kNPtoolChannel,			// L-click next Channel, R-click reverses
	kNPtoolFreeze,			// L-click Freezes node, R-click un-Freezes

	kNPtoolSetpointHi,		// L-click to Set Upper Limit, R-click clears Setpoint
	kNPtoolSetpointLo,		// L-click to Set Lower Limit, R-click clears Setpoint

	kNPtoolCount
};

enum
{
	kNPcoord,
	kNPcoordLatLongX,
	kNPcoordScale,
	kNPcoordCount
};

enum
{
	kNPcompassHeading = 0,
	kNPcompassTilt,
	kNPcompassRoll,
	kNPcompassCoordX,
	kNPcompassCoordY,
	kNPcompassCoordZ,
	kNPcompassCount
};

enum
{
	kNPhudNull = 0,
	kNPhudTags,
	kNPhudSelection,
	kNPhudConsole,
	kNPhudFPS,
	kNPhudCompass,
		kNPhudAngle,
//			kNPhudTilt,
//			kNPhudRoll,
		kNPhudCoordX,		//perhaps just call kNPhudX,					zz debug
//			kNPhudTranslate
//			kNPhudScale
//			kNPhudRotate
		kNPhudCoordY,
		kNPhudCoordZ,
		
	kNPhudMode,
	kNPhudTool,
	kNPhudSave,			//zz select
	kNPhudSelect,		//zzf
	kNPhudCreate,
	kNPhudLink,
	kNPhudCombo,
	kNPhudTopo,
	kNPhudGeo,
	kNPhudColor,
	kNPhudTexture,
	kNPhudHide,
	kNPhudInfo,
//	kNPhudCursor,		//zzf
	kNPhudCount
};


//concept of default topo is in flux, think in terms of //zz debug
//cut and paste various bramches, including making a 
//sub-branch a root pin determined by parent node type
//may not be worth it and do away with topoNull pins, CSVverOne
enum
{
	kNPtopoNull = 0,		// linear 3D euclidean space
	kNPtopoCube,			//six facet coord system for each side of cube
	kNPtopoSphere,			//spherical coords compatible with KML
	kNPtopoTorus,			//default branchLevel = 1 attached to pin
	kNPtopoCylinder,
	kNPtopoPin,				//default root pin shaped as icecream cone
	kNPtopoRod,
	kNPtopoPoint,			//zero origin offset with spherical coords
kNPtopoCount,
	kNPtopoCone,
	kNPtopoPlot,			//perhaps call it a plot and not a graph
	kNPtopoSurface,			//deformable grid, FFT, color ball, sound sphere
	kNPtopoMesh			//3D mesh model mapped as surface terrain
//	kNPtopoCount
};

enum
{
	kNPtableNull = 0,
	kNPtableGlobals,
	kNPtableCamera,
	kNPtableGrid,
	kNPtableNode,
	kNPtableChMap,
	kNPtableChMeta,
	kNPtableTracks,
	kNPtableTag,
	kNPtableFormat,
	kNPtableMap,
	kNPtableMapOSC,
	kNPtableCount
};

enum
{
	kNPmsgNull = 0,
	kNPmsgErr,
	kNPmsgWarn,
	kNPmsgDebug,
	kNPmsgCtrl,
	kNPmsgView,
	kNPmsgHint,
	kNPmsgFile,
	kNPmsgNPE,
	kNPmsgGL,
	kNPmsgDB,
	kNPmsgOSC,
};

//zz debug, don't really need this, remove this and its dependencies
//simplify the funcitonality elsewhere
enum
{
	kNPconsoleLevelNull = 0,	//hide console
	kNPconsoleLevelOne,			//display 1 line
	kNPconsoleLevelThree,		//display 3 lines
	kNPconsoleLevelMax,			//display maximum number of lines possible
	kNPconsoleLevelCount
};

enum{
	kNPconsoleMessage = 0,		//default message mode
	kNPconsoleCmd,
	kNPconsoleMenu,				//menu list	
	kNPconsoleEdit,				//code editor
	kNPconsoleMySQL
};

enum
{
	kNP_GLUT_STROKE_ROMAN = 0,
	kNP_GLUT_STROKE_MONO_ROMAN,
//	kNP_GLUT_BITMAP_9_BY_15, //debug zz GLUT_BITMAP_9_BY_15 = ((void *)0x0002)
	kNP_GLUT_BITMAP_8_BY_13,
	kNP_GLUT_BITMAP_TIMES_ROMAN_10,
	kNP_GLUT_BITMAP_TIMES_ROMAN_24,
	kNP_GLUT_BITMAP_HELVETICA_10,
	kNP_GLUT_BITMAP_HELVETICA_12,
	kNP_GLUT_BITMAP_HELVETICA_18,
	kNPfontCount
};
# define  kNP_GLUT_BITMAP_9_BY_15      ((void *)0x0002)		//zz debug, workaround

enum
{
	kNPjustifyNull = 0,
	kNPjustifyLeft,
	kNPjustifyRight,
	kNPjustifyCenter,
	kNPjustifyCount
};

enum
{
	kNPpositionNull = 0,
	kNPpositionLeft,
	kNPpositionRight,
	kNPpositionCenter,			//refers to either vertical or horizontal
	kNPpositionTop,
								//kNPmiddle,
	kNPpositionBottom,
	
	kNPpositionCount
};

enum
{
	kNPtagModeNull = 0,			//fixed to object in 3D scene, not oriented
	kNPtagModeBoxOutlineHUD,	//white text on grey box, node colored outline
	kNPtagModeBoxHUD,			//node colored text on grey box, no outline
//	kNPtagModeHUD,				//fixed size font displayed on the HUD, removed as is typically too hard to read, debug zz
//	kNPtagModeBillboard,		//text in 3D scene oriented towards cam
	kNPtagModeCount
};

enum
{
	kNPflagNull = 0,
	kNPflagN,					//any native parameter, mapItem desc format
	kNPflagCmd,					//commands 
	kNPflagURL,
	kNPflagFile,
	kNPflagUDP,
	kNPflagOSC,
	kNPflagHelp,
	kNPflagVer
};


#endif

