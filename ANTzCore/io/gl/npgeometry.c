/* -----------------------------------------------------------------------------
*
*  npgeometry.c
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

#include "npgeometry.h"

#include "npgldraw.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#define kTorusScreenSizeCount 4
#define kTorusThicknessCount 4

struct NPtorus {
	NPfloatXYZ*		vArray;
	GLint			vArraySize;
	GLint*			quadArray;
	GLint			quadArraySize;
	int				slices;
	int				segments;
	GLfloat			thickness;
};
typedef struct NPtorus NPtorus;
typedef struct NPtorus * NPtorusPtr;

struct NPcircle {
	NPfloatXY*		vArray;
	int				segments;
	GLfloat			radius;
};
typedef struct NPcircle NPcircle;
typedef struct NPcircle * NPcirclePtr;


void InitTorus (void* dataRef);


static GLint pin_dl;
static GLint pin_wire_dl;
static GLint torus_dl;
static GLint torus_wire_dl;
GLint primitiveDL;

GLuint CreatePinDL();
GLuint CreatePinWireDL();
void DrawPinDL();
void DrawPinWireDL();

GLuint CreateTorusDL();
GLuint CreateTorusWireDL();
void DrawTorusDL();
void DrawTorusWireDL(); 

void npDrawTorus (int geometry, GLfloat innerRadius);

void CreateTorus (NPtorusPtr torus);
void CreateCircle (NPcirclePtr circle);
void DeleteCircle (NPcirclePtr circle);

GLuint npCreatePrimitiveDL (void);

static GLUquadricObj *npQuadObj = NULL;

static void initNPQuadObj()
{
    if (npQuadObj == NULL)
        npQuadObj = gluNewQuadric();
}


//------------------------------------------------------------------------------
ANTZCORE_API void npInitGLPrimitive(void* dataRef)
{
	pin_dl = CreatePinDL();
	pin_wire_dl = CreatePinWireDL();
	torus_dl = CreateTorusDL();
	torus_wire_dl = CreateTorusWireDL();

	primitiveDL = npCreatePrimitiveDL();

	return;
}

//------------------------------------------------------------------------------
void npCloseGLPrimitive (void* dataRef)
{
	return;
}

//used only to draw outline wireframes
//------------------------------------------------------------------------------
ANTZCORE_API void npGLPrimitive(int geometry, float ratio)
{
	glPushMatrix();									//is glPushMatrix necessary, zz debug

	//draw the object using the primitive DL offset by geometry index
	if (geometry == kNPgeoTorus || geometry == kNPgeoTorusWire)
		npDrawTorus (geometry, ratio);
	else
		glCallList (primitiveDL + geometry);

	glPopMatrix();
}

//used to draw all geometry primitive types
//------------------------------------------------------------------------------
void npGLSurface (bool texture, pNPnode node, void* dataRef)
{
	pData data = (pData) dataRef;

	glPushMatrix();									//is glPushMatrix necessary, zz debug

	glLineWidth (node->lineWidth);		//for wireframe objects

	// turn on texture coordinates specific for each primitive type
	if (texture)
		npGLTexture (node, dataRef);

	if (node->geometry == kNPgeoTorus)
		npDrawTorus (node->geometry, node->ratio);
	else
	{
		//slide down 1 unit to center cylinder								//zz debug
		if ( ( node->geometry == kNPgeoCylinder || node->geometry == kNPgeoCylinderWire
			   || node->geometry == kNPgeoCone || node->geometry == kNPgeoConeWire )
	//		&& !( node->topo == kNPtopoRod || node->type == kNodeLink ) )
				||  ( node->topo != kNPtopoPin 
						&& ( node->geometry == kNPgeoPin 
							|| node->geometry == kNPgeoPinWire ) ) )
			glTranslatef (0.0f, 0.0f, -kNPoffsetUnit);
	//	else if (node->geometry == kNPgeoPinWire)
	//		glTranslatef (0.0f, 0.0f, -kNPoffsetPin);

		//draw the object using the primitive DL offset by geometry index
		if (data->io.gl.pickPass)
		{									
			//if odd add 1 to get a solid
			//logic forces draws wireframe as solid during pickPass
			//also an exception for pin which is out of order in geometry list, zz debug
			if (node->geometry == kNPgeoPinWire)
				glCallList (primitiveDL + kNPgeoPin);
			else if (node->geometry % 2 == 0 && node->geometry != kNPgeoPin)
				glCallList (primitiveDL + node->geometry + 1);
			else
				glCallList (primitiveDL + node->geometry);
		}
		else
			glCallList (primitiveDL + node->geometry);
	}
	// turn off texturing
	if (texture && node->textureID)					//dual param for pickPass, debug zz
		glDisable ( GL_TEXTURE_2D );

	glLineWidth (1.0f);				//reset to default of 1.0f line thickness, zz debug

	glPopMatrix();
}


//MB-TEXTURE
//------------------------------------------------------------------------------
void npGLTexture (pNPnode node, void* dataRef)
{
//	const GLfloat sgenparams[] = { 1.0f, 0.0f, 0.0f, 0.0f };
//	const GLfloat tgenparams[] = { 0.0f, 1.0f, 0.0f, 0.0f };
	const GLfloat sgenparams[] = { 1.0f, 0.0f, 0.0f, 0.0f };
	const GLfloat tgenparams[] = { 0.0f, 1.0f, 0.0f, 0.0f };

	pData data = (pData) dataRef;

	//exit if no texture or pickPass, picking requires no texture mapping
	if (!node->textureID || data->io.gl.pickPass)
		return;

	glEnable ( GL_TEXTURE_2D );
	glBindTexture ( GL_TEXTURE_2D, node->textureID );

	//use different texturing for the gluSphere
	if ( node->geometry == kNPgeoSphere // || geometry == kNPgeoSphereWire		//zz debug
		|| node->geometry == kNPgeoCylinder ) // || geometry == kNPgeoCylinderWire
	{
		glDisable( GL_TEXTURE_GEN_S );	//prevents intermittent texture anomally
		glDisable( GL_TEXTURE_GEN_T );
	}
	else
	{
		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

		glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
		glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
			// metal appearance
			// glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );
			// glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP );

		glEnable( GL_TEXTURE_GEN_S );
		glEnable( GL_TEXTURE_GEN_T );

		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

			//rather sharp setting, but probably best for mapping
			//for video use GL_LINEAR_MIPMAP_NEAREST when the angle is close 
			//to perpendicular less artifacts and a bit more blurry
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);		//zz debug, add mipmapping...
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		//zz debug, add mipmapping...
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexGenfv( GL_S, GL_OBJECT_PLANE, sgenparams );
		glTexGenfv( GL_T, GL_OBJECT_PLANE, tgenparams );
	}
}


// creates a set of toroids and loads them into GPU memory
//------------------------------------------------------------------------------
void InitTorus (void* dataRef)
{
	int i = 0, j = 0;
	NPtorusPtr torus = NULL;

	torus = (NPtorusPtr) malloc (sizeof(NPtorus));
	if (torus == NULL)
	{
		printf ("\n 4220 error malloc failed cannot write file \n");
		return;
	}

	// 5x5, 7x5, 12x6, 16x6, 24x8, 32x10, 48x12, 64x16, 96x24, 128x32, 196x48
	for ( i = 0; i < kTorusScreenSizeCount; i++)
		for ( j = 0; j < kTorusThicknessCount; j++)
		{
			switch (i)
			{
				case 0 : torus->slices = 5; torus->segments = 5; break;
				case 1 : torus->slices = 5; torus->segments = 7; break;
				case 2 : torus->slices = 6; torus->segments = 12; break;
				case 3 : torus->slices = 6; torus->segments = 16; break;
				case 4 : torus->slices = 8; torus->segments = 24; break;
				case 5 : torus->slices = 10; torus->segments = 32; break;
				case 6 : torus->slices = 12; torus->segments = 48; break;
				case 7 : torus->slices = 16; torus->segments = 64; break;
				case 8 : torus->slices = 24; torus->segments = 96; break;
				case 9 : torus->slices = 32; torus->segments = 128; break;
				case 10 : torus->slices = 48; torus->segments = 196; break;
				case 11 : torus->slices = 64; torus->segments = 256; break;
			}
			
			torus->thickness =	(GLfloat) j * 100.0f / 
								(GLfloat) (kTorusThicknessCount - 1);

			CreateTorus (torus);
			// load torus to GPU and deallocate memory
			// we could save time by allocating enough memory for all torus's at once
			// or enough for largest torus then de-allocating all at once
		}
}


// creates a single torus of specified slices, stacks and thickness
//------------------------------------------------------------------------------
void CreateTorus (NPtorusPtr torus)
{
/*	int i=0, j=0;

	NPcirclePtr ring = (NPcirclePtr) malloc (sizeof(NPcircle));
	NPcirclePtr hoops = (NPcirclePtr) malloc (torus->slices * sizeof(NPcircle));

	torus->vArraySize = torus->slices * torus->segments;
	torus->vArray = (NPfloatXYZPtr) malloc (torus->vArraySize * sizeof(NPfloatXYZ));

	torus->quadArraySize = torus->vArraySize * 4;
	torus->quadArray = (GLint*) malloc (torus->quadArraySize * sizeof(GLint));

	// create our tube ring
	ring->radius = torus->thickness;		// 1.0 is no hole in the donut
	ring->segments = torus->slices;
	CreateCircle (ring);

	// create our hoops, the length segments of the tube
	for (i=0; i < torus->slices; i++)
	{
		hoops[i].radius = 1.0f + ring->vArray[i].x;
		hoops[i].segments = torus->segments;
		CreateCircle (&hoops[i]);
	}

	// build the vertex array for the torus
	for (i=0; i < torus->segments; i++)
		for (j=0; j < torus->slices; j++)
		{
			torus->vArray[i*torus->slices + j].x = hoops[j].vArray[i].x;		//the hoop provides x and y
			torus->vArray[i*torus->slices + j].y = hoops[j].vArray[i].y;
			torus->vArray[i*torus->slices + j].z = ring->vArray[j].y;		//the ring provides the z
		}

	// build the the torus polygon array
	for (i=0; i < torus->segments; i++)
		for (j=0; j < torus->slices; j++)
		{	
			if ( i == (torus->segments - 1) )		// the last torus segment
			{
				if ( j == (torus->slices - 1) )		// the last polygon
				{
					torus->quadArray[i][j][0] = (i * torus->slices) + j;
					torus->quadArray[i][j][1] = (i * torus->slices);
					torus->quadArray[i][j][2] = 0;
					torus->quadArray[i][j][3] = j;
				}
				else
				{
					torus->quadArray[i][j][0] = (i * torus->slices) + j;
					torus->quadArray[i][j][1] = (i * torus->slices) + (j + 1);
					torus->quadArray[i][j][2] = (j + 1);
					torus->quadArray[i][j][3] = j;
				}
			}
			else
			{
				if ( j == (torus->slices - 1) )		// the last polygon in a segment
				{
					torus->quadArray[i][j][0] = (i * torus->slices) + j;
					torus->quadArray[i][j][1] = (i * torus->slices);
					torus->quadArray[i][j][2] = ((i + 1) * torus->slices);
					torus->quadArray[i][j][3] = ((i + 1) * torus->slices) + j;
				}
				else
				{
					torus->quadArray[i][j][0] = (i * torus->slices) + j;
					torus->quadArray[i][j][1] = (i * torus->slices) + (j + 1);
					torus->quadArray[i][j][2] = ((i + 1) * torus->slices) + (j + 1);
					torus->quadArray[i][j][3] = ((i + 1) * torus->slices) + j;
				}
			}

	DeleteCircle (ring);
	for (i=0; i < torus->slices; i++)
		DeleteCircle (&hoops[i]);
*/
}


// creates a circle of specified number of points and radius
// centered on the origin (0,0) starting at 12 o'clock going CW
// really should be part of GL, glCircle!, lol
//------------------------------------------------------------------------------
void CreateCircle (NPcirclePtr circle)
{
/*	int i = 0;
	GLfloat angle = 0.0, deltaAngle = 0.0f;

	circle->vArray = (NPfloatXY) malloc (cirlce->segments * sizeof(NPfloatXY));


	if (circle->segments)
		deltaAngle = (2.0 * kPI) / circle->segments;
	
	// go around the circle, 
	for (angle = 0.0f; angle <= 1.99999f * kPI; angle += deltaAngle)
	{
//		circle->vArray[i]->x = circle->radius * (GLfloat)sin(angle);
//		circle->vArray[i].y = circle->radius * (GLfloat)cos(angle);
		i++;
	}
*/
}


//------------------------------------------------------------------------------
void DeleteCircle (NPcirclePtr circle)
{
	free (circle->vArray);
	free (circle);
}


// draws a torus of specified size and thickness, stacks and slices determined
// by size in pixels on screen calculated by screen size and z distance
// radius from center of the donut to the center of the tube, thickness 0-100%
//------------------------------------------------------------------------------
void npDrawTorus (int geometry, GLfloat innerRadius)
{
	int torusID = 0;

	// torusID = screenSize + (thickness / numberOfSizes);

	// necessary for backwards compatibility of CSV files prior to Nov 2011
	if (innerRadius == 0.0f)
		innerRadius = kNPdefaultRatio;

	// optimize first with display list for innerRadius using ratio
	// then for screen size based on z depth
	// torusID); debug zz update to support DL array of different ratios, both solid and wireframe
	// calls a DL if the inner radius is 0.0 or 0.1, otherwise draws in direct mode, debug zz
	if (innerRadius == kNPdefaultRatio )
		glCallList (primitiveDL + geometry);
	else
	{
		if (geometry == kNPgeoTorusWire)
			npGLWireTorus (kNPtorusRadius * innerRadius, kNPtorusRadius, 7, 16);	//	7, 16);
		else
			npGLSolidTorus(kNPtorusRadius * innerRadius, kNPtorusRadius, 7, 16);	//	7, 16);
	}
}


// Draw Pin, ice-cream cone shape, width is a 10% the cone height
// the icecream dome sticks up an additional 10% so total height is 110% height
//------------------------------------------------------------------------------
void DrawPinDL() 
{
	glPushMatrix();															//zzoff
	glTranslatef (0.0f, 0.0f, kNPoffsetPin);
	glRotatef (180.0f, 0.0f, 1.0f, 0.0f);
	npGLSolidCone (kNPoffsetPin * 0.1f, kNPoffsetPin, 10, 1);

	glRotatef (-180.0f, 0.0f, 1.0f, 0.0f);	//undo rotation n save on pushMatrix
	npGLSolidSphere (kNPoffsetPin * 0.1f, 10, 10);
	glPopMatrix();															//zzoff
}

// Draw Pin, ice-cream cone shape, width is a 10% the cone height
// the icecream dome sticks up an additional 10% so total height is 110% height
//------------------------------------------------------------------------------
void DrawPinWireDL() 
{
	glPushMatrix();															//zzoff
	glTranslatef (0.0f, 0.0f, kNPoffsetPin);
	glRotatef (180.0f, 0.0f, 1.0f, 0.0f);
	npGLWireCone (kNPoffsetPin * 0.1f, kNPoffsetPin, 10, 1);

	glRotatef (-180.0f, 0.0f, 1.0f, 0.0f);	//undo rotation n save on pushMatrix
	npGLWireSphere (kNPoffsetPin * 0.1f, 10, 10);
	glPopMatrix();															//zzoff
}


//------------------------------------------------------------------------------
void DrawTorusDL() 
{
	npGLSolidTorus (kNPtorusRadius * 0.1f, kNPtorusRadius, 7, 16); //zzhp
}


//------------------------------------------------------------------------------
void DrawTorusWireDL() 
{
	npGLWireTorus (kNPtorusRadius * 0.1f, kNPtorusRadius, 7, 16);
}


//------------------------------------------------------------------------------
GLuint CreatePinDL()
{
	GLuint displayList;

	// Create the id for the list
	displayList = glGenLists(1);

	// start list
	glNewList (displayList, GL_COMPILE);

	// call the function that contains the rendering commands
	DrawPinDL();

	// endList
	glEndList();

	return (displayList);
}


//------------------------------------------------------------------------------
GLuint CreatePinWireDL()
{
	GLuint displayList;

	// Create the id for the list
	displayList = glGenLists(1);

	// start list
	glNewList (displayList, GL_COMPILE);

	// call the function that contains the rendering commands
	DrawPinWireDL();

	// endList
	glEndList();

	return (displayList);
}


//------------------------------------------------------------------------------
GLuint CreateTorusDL()
{
	GLuint displayList;

	// Create the id for the list
	displayList = glGenLists(1);

	// start list
	glNewList (displayList, GL_COMPILE);

	// call the function that contains the rendering commands
	DrawTorusDL();

	// endList
	glEndList();

	return (displayList);
}


//------------------------------------------------------------------------------
GLuint CreateTorusWireDL()
{
	GLuint displayList;

	// Create the id for the list
	displayList = glGenLists(1);

	// start list
	glNewList (displayList, GL_COMPILE);

	// call the function that contains the rendering commands
	DrawTorusWireDL();

	// endList
	glEndList();

	return (displayList);
}

//------------------------------------------------------------------------------
void DrawSphereWireDL()
{
	GLUquadricObj *sphere=NULL;

	sphere = gluNewQuadric();
	gluQuadricDrawStyle (sphere, GLU_LINE);	//GLU_LINE
	gluQuadricTexture (sphere, TRUE);
	gluQuadricNormals (sphere, GLU_FLAT);

	//Making a display list
	// mysphereID = glGenLists(1);
	// glNewList(mysphereID, GL_COMPILE);

	glPushMatrix();							//update to not use a pushMatrix, zz debug
		glRotatef (180.0f, 0.0f, 0.0f, 1.0f);	//orient world texture for coord 0,0
		gluSphere (sphere, 1.0, 24, 12);
	glPopMatrix();

	// glEndList();		
	// gluDeleteQuadric(sphere);				``````````````//zz debug, should use this
}

//------------------------------------------------------------------------------
void DrawSphereDL()
{
	GLUquadricObj *sphere = NULL;

	sphere = gluNewQuadric();
	gluQuadricDrawStyle (sphere, GLU_FILL);
	gluQuadricTexture (sphere, TRUE);
	gluQuadricNormals (sphere, GLU_SMOOTH);

	//Making a display list
	// mysphereID = glGenLists(1);
	// glNewList(mysphereID, GL_COMPILE);

	glPushMatrix();							//update to not use a pushMatrix, zz debug
		glRotatef (180.0f, 0.0f, 0.0f, 1.0f);	//orient world texture for coord 0,0
		gluSphere (sphere, 1.0, 24, 12);		//replace glu with custom optimized
	glPopMatrix();

	// glEndList();
	// gluDeleteQuadric(sphere);						//zz debug, should use this
}

//------------------------------------------------------------------------------
void DrawCylinderWireDL()
{
	GLUquadricObj *gluObject = NULL;

	gluObject = gluNewQuadric();
	gluQuadricDrawStyle (gluObject, GLU_LINE);
	gluQuadricTexture (gluObject, TRUE);		//faster without texture coords, zz debug
	gluQuadricNormals (gluObject, GLU_SMOOTH);

	//Making a display list
	// mysphereID = glGenLists(1);
	// glNewList(mysphereID, GL_COMPILE);

	glPushMatrix();							//update to not use a pushMatrix, zz debug
		glRotatef (180.0f, 0.0f, 0.0f, 1.0f);	//orient world texture for coord 0,0
		gluCylinder (gluObject, 1.0, 1.0, 2.0, 24, 1);
	glPopMatrix();

	// glEndList();
	// gluDeleteQuadric(sphere);				``````````````//zz debug, should use this
}
//------------------------------------------------------------------------------
void DrawCylinderDL()
{
	GLUquadricObj *gluObject = NULL;

	gluObject = gluNewQuadric();
	gluQuadricDrawStyle (gluObject, GLU_FILL);
	gluQuadricTexture (gluObject, TRUE);
	gluQuadricNormals (gluObject, GLU_SMOOTH);

	//Making a display list
	// mysphereID = glGenLists(1);
	// glNewList(mysphereID, GL_COMPILE);

	glPushMatrix();								//update to not use a pushMatrix, zz debug
		glRotatef (180.0f, 0.0f, 0.0f, 1.0f);	//orient world texture for coord 0,0
		gluCylinder (gluObject, 1.0, 1.0, 2.0, 24, 1);
		glRotatef (180.0f, 1.0f, 0.0f, 0.0f);	//flips the disk over
		gluDisk (gluObject, 0.0, 1.0, 24, 1);	//draw bottom disk
		glRotatef (180.0f, 0.0f, 1.0f, 0.0f);	//flip to right-side up
		glTranslatef (0.0f, 0.0f, 2.0f);		//translate to the top
		gluDisk (gluObject, 0.0, 1.0, 24, 1);	//draw top disk
	glPopMatrix();

	// glEndList();
	// gluDeleteQuadric(sphere);				``````````````//zz debug, should use this
}

//------------------------------------------------------------------------------
void DrawConeDL()
{
	GLUquadricObj *gluObject = NULL;

	gluObject = gluNewQuadric();
	gluQuadricDrawStyle (gluObject, GLU_FILL);
	gluQuadricTexture (gluObject, TRUE);		//does not shade nor texture well for a cone, zz debug
	gluQuadricNormals (gluObject, GLU_SMOOTH);

	//Making a display list
	// mysphereID = glGenLists(1);
	// glNewList(mysphereID, GL_COMPILE);

	glPushMatrix();								//update to not use a pushMatrix, zz debug
		glRotatef (180.0f, 0.0f, 0.0f, 1.0f);	//orient world texture for coord 0,0
		gluCylinder (gluObject, 1.0, 0.0, 2.0, 24, 1);	//draw cone
		glRotatef (180.0f, 1.0f, 0.0f, 0.0f);	//flips the disk over
		gluDisk (gluObject, 0.0, 1.0, 24, 1);	//draw bottom disk
	glPopMatrix();

	// glEndList();
	// gluDeleteQuadric(sphere);				``````````````//zz debug, should use this
}

//------------------------------------------------------------------------------
GLuint npCreatePrimitiveDL(void)
{
	GLuint displayList = 0, i = 0;


	// Create the id for the list
	i = displayList = glGenLists (kNPgeoCount);

	// start list
	glNewList (i++, GL_COMPILE);
		npWireCube (1.4142f);
	glEndList();

	glNewList (i++, GL_COMPILE);
		npSolidCube (1.4142f);
	glEndList();

	glNewList (i++, GL_COMPILE);
		DrawSphereWireDL();					//using GLU for proper tex mapping
	glEndList();

	glNewList (i++, GL_COMPILE);
		DrawSphereDL();						//using GLU for proper tex mapping
	glEndList();

	glNewList (i++, GL_COMPILE);
		npGlutPrimitive (kNPgeoConeWire);
	glEndList();

	glNewList (i++, GL_COMPILE);
		npGlutPrimitive (kNPgeoCone);		//GLU not very good cone tex mapping
	glEndList();
	
	glNewList (i++, GL_COMPILE);
		npGLWireTorus (kNPtorusRadius * 0.1f, kNPtorusRadius, 7, 16);
	glEndList();

	glNewList (i++, GL_COMPILE);
		npGLSolidTorus (kNPtorusRadius * 0.1f, kNPtorusRadius, 7, 16);
	glEndList();

	glNewList (i++, GL_COMPILE);
		npGlutPrimitive (kNPgeoDodecahedronWire);
	glEndList();

	glNewList (i++, GL_COMPILE);
		npGlutPrimitive (kNPgeoDodecahedron);
	glEndList();

	glNewList (i++, GL_COMPILE);
		npGlutPrimitive (kNPgeoOctahedronWire);
	glEndList();

	glNewList (i++, GL_COMPILE);
		npGlutPrimitive (kNPgeoOctahedron);
	glEndList();

	glNewList (i++, GL_COMPILE);
		npGlutPrimitive (kNPgeoTetrahedronWire);
	glEndList();

	glNewList (i++, GL_COMPILE);
		npGlutPrimitive (kNPgeoTetrahedron);
	glEndList();

	glNewList (i++, GL_COMPILE);
		npGlutPrimitive (kNPgeoIcosahedronWire);
	glEndList();

	glNewList (i++, GL_COMPILE);
		npGlutPrimitive (kNPgeoIcosahedron);
	glEndList();

	glNewList (i++, GL_COMPILE);
		DrawPinDL();
	glEndList();

	glNewList (i++, GL_COMPILE);
		DrawPinWireDL();
	glEndList();

	glNewList (i++, GL_COMPILE);
		DrawCylinderWireDL();
	glEndList();

	glNewList (i++, GL_COMPILE);
		DrawCylinderDL();
	glEndList();
	
	return (displayList);
}

//------------------------------------------------------------------------------
void npGLSolidSphere(GLdouble radius, GLint slices, GLint stacks)
{
    drawSphere(radius, slices, stacks, GLU_FILL);
}

//------------------------------------------------------------------------------
void npGLWireSphere(GLdouble radius, GLint slices, GLint stacks)
{
    drawSphere(radius, slices, stacks, GLU_LINE);
}

/*
* Compute lookup table of cos and sin values forming a cirle
*
* Notes:
*    It is the responsibility of the caller to free these tables
*    The size of the table is (n+1) to form a connected loop
*    The last entry is exactly the same as the first
*    The sign of n can be flipped to get the reverse loop
*/

static void fghCircleTable(double **sint, double **cost, const int n)
{
	int i;

	/* Table size, the sign of n flips the circle direction */

	const int size = abs(n);

	/* Determine the angle between samples */

	const double angle = 2 * M_PI / (double)((n == 0) ? 1 : n);

	/* Allocate memory for n samples, plus duplicate of first entry at the end */

	*sint = (double *)calloc(sizeof(double), size + 1);
	*cost = (double *)calloc(sizeof(double), size + 1);

	/* Bail out if memory allocation fails, fgError never returns */

	if (!(*sint) || !(*cost))
	{
		free(*sint);
		free(*cost);
		//fgError("Failed to allocate memory in fghCircleTable");
	}

	/* Compute cos and sin around the circle */

	(*sint)[0] = 0.0;
	(*cost)[0] = 1.0;

	for (i = 1; i<size; i++)
	{
		(*sint)[i] = sin(angle*i);
		(*cost)[i] = cos(angle*i);
	}

	/* Last sample is duplicate of the first */

	(*sint)[size] = (*sint)[0];
	(*cost)[size] = (*cost)[0];
}

//------------------------------------------------------------------------------
void npGLSolidCone(GLdouble base, GLdouble height, GLint slices, GLint stacks)
{
	int i, j;

	/* Step in z and radius as stacks are drawn. */

	double z0, z1;
	double r0, r1;

	const double zStep = height / ((stacks > 0) ? stacks : 1);
	const double rStep = base / ((stacks > 0) ? stacks : 1);

	/* Scaling factors for vertex normals */

	const double cosn = (height / sqrt(height * height + base * base));
	const double sinn = (base / sqrt(height * height + base * base));

	/* Pre-computed circle */

	double *sint, *cost;

	fghCircleTable(&sint, &cost, -slices);

	/* Cover the circular base with a triangle fan... */

	z0 = 0.0;
	z1 = zStep;

	r0 = base;
	r1 = r0 - rStep;

	glBegin(GL_TRIANGLE_FAN);

	glNormal3d(0.0, 0.0, -1.0);
	glVertex3d(0.0, 0.0, z0);

	for (j = 0; j <= slices; j++)
		glVertex3d(cost[j] * r0, sint[j] * r0, z0);

	glEnd();

	/* Cover each stack with a quad strip, except the top stack */

	for (i = 0; i<stacks - 1; i++)
	{
		glBegin(GL_QUAD_STRIP);

		for (j = 0; j <= slices; j++)
		{
			glNormal3d(cost[j] * cosn, sint[j] * cosn, sinn);
			glVertex3d(cost[j] * r0, sint[j] * r0, z0);
			glVertex3d(cost[j] * r1, sint[j] * r1, z1);
		}

		z0 = z1; z1 += zStep;
		r0 = r1; r1 -= rStep;

		glEnd();
	}

	/* The top stack is covered with individual triangles */

	glBegin(GL_TRIANGLES);

	glNormal3d(cost[0] * cosn, sint[0] * cosn, sinn);

	for (j = 0; j<slices; j++)
	{
		glVertex3d(cost[j + 0] * r0, sint[j + 0] * r0, z0);
		glVertex3d(0, 0, height);
		glNormal3d(cost[j + 1] * cosn, sint[j + 1] * cosn, sinn);
		glVertex3d(cost[j + 1] * r0, sint[j + 1] * r0, z0);
	}

	glEnd();

	/* Release sin and cos tables */

	free(sint);
	free(cost);
}

//------------------------------------------------------------------------------
void npGLWireCone(GLdouble base, GLdouble height, GLint slices, GLint stacks)
{
    initNPQuadObj();
    gluQuadricDrawStyle(npQuadObj, GLU_LINE);
    gluQuadricNormals(npQuadObj, GLU_SMOOTH);
    gluCylinder(npQuadObj, base, 0.0, height, slices, stacks);
}

//------------------------------------------------------------------------------
void npGLSolidTorus(GLdouble innerRadius, GLdouble outerRadius,
    GLint slices, GLint stacks)
{
    drawTorus((GLfloat)innerRadius, (GLfloat)outerRadius, slices, stacks);
}

//------------------------------------------------------------------------------
void npGLWireTorus(GLdouble innerRadius, GLdouble outerRadius,
    GLint slices, GLint stacks)
{
    glPushAttrib(GL_POLYGON_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    drawTorus((GLfloat)innerRadius, (GLfloat)outerRadius, slices, stacks);
    glPopAttrib();
}

//------------------------------------------------------------------------------
void npGlutPrimitive(int primitive)
{
    switch (primitive)
    {
    case kNPgeoCubeWire:
        glScalef(0.6f, 0.6f, 0.6f);
        npWireCube(2.0f);
        glScalef(1.666667f, 1.666667f, 1.666667f);
        break;
    case kNPgeoCube:
        glScalef(0.6f, 0.6f, 0.6f);
        npSolidCube(2.0f);
        glScalef(1.666667f, 1.666667f, 1.666667f);
        break;
    case kNPgeoSphereWire: npGLWireSphere(1.0f, 24, 12); break;//15, 15 ); break;
    case kNPgeoSphere: npGLSolidSphere(1.0f, 24, 12); break;

    case kNPgeoConeWire: npGLWireCone(1.0f, 2.0f, 24, 1); break;
    case kNPgeoCone: npGLSolidCone(1.0f, 2.0f, 24, 1); break;

    case kNPgeoTorusWire: npGLWireTorus(kNPtorusRadius * 0.1f, kNPtorusRadius, 7, 16); break;
    case kNPgeoTorus: npGLSolidTorus(kNPtorusRadius * 0.1f, kNPtorusRadius, 7, 16); break;

    case kNPgeoDodecahedronWire:
        glScalef(0.6f, 0.6f, 0.6f);
        npWireDodecahedron();
        glScalef(1.666667f, 1.666667f, 1.666667f);
        break;
    case kNPgeoDodecahedron:
        glScalef(0.6f, 0.6f, 0.6f);
        npSolidDodecahedron();
        glScalef(1.666667f, 1.666667f, 1.666667f);
        break;
    case kNPgeoOctahedronWire: npWireOctahedron(); break;
    case kNPgeoOctahedron: npSolidOctahedron(); break;
    case kNPgeoTetrahedronWire: npWireTetrahedron(); break;
    case kNPgeoTetrahedron: npSolidTetrahedron(); break;
    case kNPgeoIcosahedronWire: npWireIcosahedron(); break;
    case kNPgeoIcosahedron: npSolidIcosahedron(); break;

        //	case kNPglutWireTeapot : glutWireTeapot( 2.0f ); break;
        //	case kNPglutSolidTeapot : glutSolidTeapot( 2.0f ); break;

    default: npWireTetrahedron(); break;
    }
}

void npWireCube(GLdouble size)
{
    drawBox((GLfloat)size, GL_LINE_LOOP);
}

void npSolidCube(GLdouble size)
{
    drawBox((GLfloat)size, GL_QUADS);
}

void drawBox(GLfloat size, GLenum type)
{
    static GLfloat n[6][3] =
    {
        { -1.0, 0.0, 0.0 },
        { 0.0, 1.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 0.0, -1.0, 0.0 },
        { 0.0, 0.0, 1.0 },
        { 0.0, 0.0, -1.0 }
    };
    static GLint faces[6][4] =
    {
        { 0, 1, 2, 3 },
        { 3, 2, 6, 7 },
        { 7, 6, 5, 4 },
        { 4, 5, 1, 0 },
        { 5, 6, 2, 1 },
        { 7, 4, 0, 3 }
    };
    GLfloat v[8][3];
    GLint i;

    v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size / 2;
    v[4][0] = v[5][0] = v[6][0] = v[7][0] = size / 2;
    v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size / 2;
    v[2][1] = v[3][1] = v[6][1] = v[7][1] = size / 2;
    v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size / 2;
    v[1][2] = v[2][2] = v[5][2] = v[6][2] = size / 2;

    for (i = 5; i >= 0; i--) {
        glBegin(type);
        glNormal3fv(&n[i][0]);
        glVertex3fv(&v[faces[i][0]][0]);
        glVertex3fv(&v[faces[i][1]][0]);
        glVertex3fv(&v[faces[i][2]][0]);
        glVertex3fv(&v[faces[i][3]][0]);
        glEnd();
    }
}

void drawSphere(GLdouble radius, GLint slices, GLint stacks, GLenum drawStyle)
{
    initNPQuadObj();
    gluQuadricDrawStyle(npQuadObj, drawStyle);
    gluQuadricNormals(npQuadObj, GLU_SMOOTH);
    gluSphere(npQuadObj, radius, slices, stacks);
}

void drawTorus(GLfloat r, GLfloat R, GLint nsides, GLint rings)
{
    int i, j;
    GLfloat theta, phi, theta1;
    GLfloat cosTheta, sinTheta;
    GLfloat cosTheta1, sinTheta1;
    GLfloat ringDelta, sideDelta;

    ringDelta = 2.0f * M_PI / rings;
    sideDelta = 2.0f * M_PI / nsides;

    theta = 0.0;
    cosTheta = 1.0;
    sinTheta = 0.0;
    for (i = rings - 1; i >= 0; i--) {
        theta1 = theta + ringDelta;
        cosTheta1 = (GLfloat)cos(theta1);
        sinTheta1 = (GLfloat)sin(theta1);
        glBegin(GL_QUAD_STRIP);
        phi = 0.0;
        for (j = nsides; j >= 0; j--) {
            GLfloat cosPhi, sinPhi, dist;

            phi += sideDelta;
            cosPhi = (GLfloat)cos(phi);
            sinPhi = (GLfloat)sin(phi);
            dist = R + r * cosPhi;

            glNormal3f(cosTheta1 * cosPhi, -sinTheta1 * cosPhi, sinPhi);
            glVertex3f(cosTheta1 * dist, -sinTheta1 * dist, r * sinPhi);
            glNormal3f(cosTheta * cosPhi, -sinTheta * cosPhi, sinPhi);
            glVertex3f(cosTheta * dist, -sinTheta * dist, r * sinPhi);
        }
        glEnd();
        theta = theta1;
        cosTheta = cosTheta1;
        sinTheta = sinTheta1;
    }
}

static GLfloat dodec[20][3];

void initDodecahedron()
{
    GLfloat alpha, beta;

    alpha = sqrt(2.0 / (3.0 + sqrt(5.0)));
    beta = 1.0 + sqrt(6.0 / (3.0 + sqrt(5.0)) -
        2.0 + 2.0 * sqrt(2.0 / (3.0 + sqrt(5.0))));
    /* *INDENT-OFF* */
    dodec[0][0] = -alpha; dodec[0][1] = 0; dodec[0][2] = beta;
    dodec[1][0] = alpha; dodec[1][1] = 0; dodec[1][2] = beta;
    dodec[2][0] = -1; dodec[2][1] = -1; dodec[2][2] = -1;
    dodec[3][0] = -1; dodec[3][1] = -1; dodec[3][2] = 1;
    dodec[4][0] = -1; dodec[4][1] = 1; dodec[4][2] = -1;
    dodec[5][0] = -1; dodec[5][1] = 1; dodec[5][2] = 1;
    dodec[6][0] = 1; dodec[6][1] = -1; dodec[6][2] = -1;
    dodec[7][0] = 1; dodec[7][1] = -1; dodec[7][2] = 1;
    dodec[8][0] = 1; dodec[8][1] = 1; dodec[8][2] = -1;
    dodec[9][0] = 1; dodec[9][1] = 1; dodec[9][2] = 1;
    dodec[10][0] = beta; dodec[10][1] = alpha; dodec[10][2] = 0;
    dodec[11][0] = beta; dodec[11][1] = -alpha; dodec[11][2] = 0;
    dodec[12][0] = -beta; dodec[12][1] = alpha; dodec[12][2] = 0;
    dodec[13][0] = -beta; dodec[13][1] = -alpha; dodec[13][2] = 0;
    dodec[14][0] = -alpha; dodec[14][1] = 0; dodec[14][2] = -beta;
    dodec[15][0] = alpha; dodec[15][1] = 0; dodec[15][2] = -beta;
    dodec[16][0] = 0; dodec[16][1] = beta; dodec[16][2] = alpha;
    dodec[17][0] = 0; dodec[17][1] = beta; dodec[17][2] = -alpha;
    dodec[18][0] = 0; dodec[18][1] = -beta; dodec[18][2] = alpha;
    dodec[19][0] = 0; dodec[19][1] = -beta; dodec[19][2] = -alpha;
    /* *INDENT-ON* */

}

#define DIFF3(_a,_b,_c) { \
    (_c)[0] = (_a)[0] - (_b)[0]; \
    (_c)[1] = (_a)[1] - (_b)[1]; \
    (_c)[2] = (_a)[2] - (_b)[2]; \
}

void crossprod(GLfloat v1[3], GLfloat v2[3], GLfloat prod[3])
{
    GLfloat p[3];         /* in case prod == v1 or v2 */

    p[0] = v1[1] * v2[2] - v2[1] * v1[2];
    p[1] = v1[2] * v2[0] - v2[2] * v1[0];
    p[2] = v1[0] * v2[1] - v2[0] * v1[1];
    prod[0] = p[0];
    prod[1] = p[1];
    prod[2] = p[2];
}

void normalize(GLfloat v[3])
{
    GLfloat d;

    d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (d == 0.0) {
        v[0] = d = 1.0;
    }
    d = 1 / d;
    v[0] *= d;
    v[1] *= d;
    v[2] *= d;
}

void pentagon(int a, int b, int c, int d, int e, GLenum shadeType)
{
    GLfloat n0[3], d1[3], d2[3];

    DIFF3(dodec[a], dodec[b], d1);
    DIFF3(dodec[b], dodec[c], d2);
    crossprod(d1, d2, n0);
    normalize(n0);

    glBegin(shadeType);
    glNormal3fv(n0);
    glVertex3fv(&dodec[a][0]);
    glVertex3fv(&dodec[b][0]);
    glVertex3fv(&dodec[c][0]);
    glVertex3fv(&dodec[d][0]);
    glVertex3fv(&dodec[e][0]);
    glEnd();
}

void dodecahedron(GLenum type)
{
    static int inited = 0;

    if (inited == 0) {
        inited = 1;
        initDodecahedron();
    }
    pentagon(0, 1, 9, 16, 5, type);
    pentagon(1, 0, 3, 18, 7, type);
    pentagon(1, 7, 11, 10, 9, type);
    pentagon(11, 7, 18, 19, 6, type);
    pentagon(8, 17, 16, 9, 10, type);
    pentagon(2, 14, 15, 6, 19, type);
    pentagon(2, 13, 12, 4, 14, type);
    pentagon(2, 19, 18, 3, 13, type);
    pentagon(3, 0, 5, 12, 13, type);
    pentagon(6, 15, 8, 10, 11, type);
    pentagon(4, 17, 8, 15, 14, type);
    pentagon(4, 12, 5, 16, 17, type);
}

void npWireDodecahedron()
{
    dodecahedron(GL_LINE_LOOP);
}

void npSolidDodecahedron()
{
    dodecahedron(GL_TRIANGLE_FAN);
}

void recorditem(GLfloat * n1, GLfloat * n2, GLfloat * n3, GLenum shadeType)
{
    GLfloat q0[3], q1[3];

    DIFF3(n1, n2, q0);
    DIFF3(n2, n3, q1);
    crossprod(q0, q1, q1);
    normalize(q1);

    glBegin(shadeType);
    glNormal3fv(q1);
    glVertex3fv(n1);
    glVertex3fv(n2);
    glVertex3fv(n3);
    glEnd();
}

void subdivide(GLfloat * v0, GLfloat * v1, GLfloat * v2, GLenum shadeType)
{
    int depth;
    GLfloat w0[3], w1[3], w2[3];
    GLfloat l;
    int i, j, k, n;

    depth = 1;
    for (i = 0; i < depth; i++) {
        for (j = 0; i + j < depth; j++) {
            k = depth - i - j;
            for (n = 0; n < 3; n++) {
                w0[n] = (i * v0[n] + j * v1[n] + k * v2[n]) / depth;
                w1[n] = ((i + 1) * v0[n] + j * v1[n] + (k - 1) * v2[n])
                    / depth;
                w2[n] = (i * v0[n] + (j + 1) * v1[n] + (k - 1) * v2[n])
                    / depth;
            }
            l = sqrt(w0[0] * w0[0] + w0[1] * w0[1] + w0[2] * w0[2]);
            w0[0] /= l;
            w0[1] /= l;
            w0[2] /= l;
            l = sqrt(w1[0] * w1[0] + w1[1] * w1[1] + w1[2] * w1[2]);
            w1[0] /= l;
            w1[1] /= l;
            w1[2] /= l;
            l = sqrt(w2[0] * w2[0] + w2[1] * w2[1] + w2[2] * w2[2]);
            w2[0] /= l;
            w2[1] /= l;
            w2[2] /= l;
            recorditem(w1, w0, w2, shadeType);
        }
    }
}

void drawtriangle(int i, GLfloat data[][3], int ndx[][3], GLenum shadeType)
{
    GLfloat *x0, *x1, *x2;

    x0 = data[ndx[i][0]];
    x1 = data[ndx[i][1]];
    x2 = data[ndx[i][2]];
    subdivide(x0, x1, x2, shadeType);
}

/* octahedron data: The octahedron produced is centered at the origin and has radius 1.0 */
static GLfloat odata[6][3] =
{
    { 1.0, 0.0, 0.0 },
    { -1.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0 },
    { 0.0, -1.0, 0.0 },
    { 0.0, 0.0, 1.0 },
    { 0.0, 0.0, -1.0 }
};

static int ondex[8][3] =
{
    { 0, 4, 2 },
    { 1, 2, 4 },
    { 0, 3, 4 },
    { 1, 4, 3 },
    { 0, 2, 5 },
    { 1, 5, 2 },
    { 0, 5, 3 },
    { 1, 3, 5 }
};

void octahedron(GLenum shadeType)
{
    int i;

    for (i = 7; i >= 0; i--) {
        drawtriangle(i, odata, ondex, shadeType);
    }
}

void npWireOctahedron()
{
    octahedron(GL_LINE_LOOP);
}

void npSolidOctahedron()
{
    octahedron(GL_TRIANGLES);
}

/* icosahedron data: These numbers are rigged to make an icosahedron of radius 1.0 */

#define X .525731112119133606
#define Z .850650808352039932

static GLfloat idata[12][3] =
{
    { -X, 0, Z },
    { X, 0, Z },
    { -X, 0, -Z },
    { X, 0, -Z },
    { 0, Z, X },
    { 0, Z, -X },
    { 0, -Z, X },
    { 0, -Z, -X },
    { Z, X, 0 },
    { -Z, X, 0 },
    { Z, -X, 0 },
    { -Z, -X, 0 }
};

static int index[20][3] =
{
    { 0, 4, 1 },
    { 0, 9, 4 },
    { 9, 5, 4 },
    { 4, 5, 8 },
    { 4, 8, 1 },
    { 8, 10, 1 },
    { 8, 3, 10 },
    { 5, 3, 8 },
    { 5, 2, 3 },
    { 2, 7, 3 },
    { 7, 10, 3 },
    { 7, 6, 10 },
    { 7, 11, 6 },
    { 11, 0, 6 },
    { 0, 1, 6 },
    { 6, 1, 10 },
    { 9, 0, 11 },
    { 9, 11, 2 },
    { 9, 2, 5 },
    { 7, 2, 11 },
};

void icosahedron(GLenum shadeType)
{
    int i;

    for (i = 19; i >= 0; i--) {
        drawtriangle(i, idata, index, shadeType);
    }
}

void npWireIcosahedron()
{
    icosahedron(GL_LINE_LOOP);
}

void npSolidIcosahedron()
{
    icosahedron(GL_TRIANGLES);
}

/* tetrahedron data: */

#define T       1.73205080756887729

static GLfloat tdata[4][3] =
{
    { T, T, T },
    { T, -T, -T },
    { -T, T, -T },
    { -T, -T, T }
};

static int tndex[4][3] =
{
    { 0, 1, 3 },
    { 2, 1, 0 },
    { 3, 2, 0 },
    { 1, 2, 3 }
};

void tetrahedron(GLenum shadeType)
{
    int i;

    for (i = 3; i >= 0; i--)
        drawtriangle(i, tdata, tndex, shadeType);
}

void npWireTetrahedron()
{
    tetrahedron(GL_LINE_LOOP);
}

void npSolidTetrahedron()
{
    tetrahedron(GL_TRIANGLES);
}