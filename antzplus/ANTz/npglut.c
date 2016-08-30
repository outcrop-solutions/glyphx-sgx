/* -----------------------------------------------------------------------------
*
*  npglut.c
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

#include "npglut.h"

#ifdef WIN32
#include <GL/freeglut.h>
#endif
#ifdef NP_OSX_
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#endif
#ifdef NP_LINUX_
#include <GL/freeglut.h>
#endif


#include "npio.h"
#include "npctrl.h"				//remove once npPostMsg is global, debug zz

#include "io/npgl.h"
#include "npkey.h"
#include "npmouse.h"

#include "os/npos.h"

#include "data/npmapfile.h"		//debug, zz

#include "nptext.h"
#include "npgui.h"

void npGlutInitGL();
void npGlutDrawGLScene(void);
void npGlutDrawGLSceneIdle(void);
// void npGlutResizeGLScene(int Width, int Height);

void npGlutKeyDown (unsigned char key, int x, int y);
void npGlutKeyUp (unsigned char key, int x, int y);
void npGlutKeyDownSpecial (int key, int x, int y);
void npGlutKeyUpSpecial (int key, int x, int y);

//the intent of this file is wrap glut functions and glut specific code
//its 'npgl.h' that handles the primary OpenGL routines
//app is designed to be wrapped by GLUT or other OS methods, wgl, Cocoa, etc...]

//update to allow entering and leaving game mode without re-loading textures
// wglShareLists() function in MSW and glXCreateContext() under X Windows. zz
//------------------------------------------------------------------------------
void npInitGlut (int argc, char **argv, void* dataRef)
{
	GLboolean stereoSupport = false;
	int depth = 0;
	int result = 0;
	int gMainWindow = 0;
	char msg[256];

	pData data = (pData) dataRef;
	pNPgl gl = &data->io.gl;

	glutInit (&argc, argv);

//zz debug, move OS specific code to npos.h ?	
	//stereo3D not yet supported on OSX, zz debug
#ifndef NP_OSX_
	sprintf (msg, "freeglut ver: %d", glutGet(GLUT_VERSION));
	npPostMsg (msg, kNPmsgCtrl, data);

	glGetBooleanv (GL_STEREO, &stereoSupport);

#else
	npPostMsg ("Apple GLUT", kNPmsgCtrl, data);

	stereoSupport = false;			
#endif
	
	gl->stereo = stereoSupport;

	if (gl->stereo)
		sprintf (msg, "Stereo 3D support: YES");
	else
		sprintf (msg, "Stereo 3D support: NO");
	npPostMsg (msg, kNPmsgCtrl, data);

	//OpenGL stereo 3D currently ONLY supported by Quadro and AMD Fire Pro series
	//at this time the bios is different consumer cards use same chip die...?

	if (gl->stereo)
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);
	else
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);			//add SRGBA support, zz

	glutInitWindowPosition (gl->position.x, gl->position.y);
	glutInitWindowSize (gl->windowSize.x, gl->windowSize.y);
	gl->windowID = glutCreateWindow (gl->name);

	glutSetWindowTitle("SynGlyphX Glyph Viewer");
	
	glutSetWindow (gl->windowID);			//S3D
	glutHideWindow();						//S3D

/*	//stereo 3D and GameMode
	//if (gl->gameMode)
	printf("Attempting Game Mode, please ignore GLUT warnings\n");
	glutGameModeString("1920x1080:32@121");
	if ( glutEnterGameMode() == 0 )
	{
		glutGameModeString("1920x1080:32@120");
		if ( glutEnterGameMode() == 0 )
		{
			glutGameModeString("1920x1080:32@119");
			if ( glutEnterGameMode() == 0 )
			{
				glutGameModeString("1920x1080:32@60"); //does not specify refresh
				result = ( glutEnterGameMode() );
	}}}	//add more res... 1680x1050 1366x768 1280x720 1280x1024, 1024x768, also 100Hz and 110Hz
*/
//	wglSwapIntervalEXT(1);		//to remove frame tearing with VSYNC debug, zz
//	nposSwapInterval(1);		//move OS methods to npos.h, debug zz
//	glfwSwapInterval(1);		//may use the glfw libarry, not implememted, zz

	//register keyboard events with GLUT
	glutKeyboardFunc (npGlutKeyDown);
	glutKeyboardUpFunc (npGlutKeyUp);
	glutSpecialFunc (npGlutKeyDownSpecial);
	glutSpecialUpFunc (npGlutKeyUpSpecial);
	
	//register mouse events with GLUT
	glutMouseFunc (npMouseEvent);
	glutMotionFunc (npMouseMotion);
	glutMouseWheelFunc (npMouseWheel);

	glutDisplayFunc (npGlutDrawGLScene);
	glutIdleFunc (npGlutDrawGLSceneIdle);

	glutReshapeFunc (npGLResizeScene);
	
	//---
	npInitGL (dataRef);

	glutShowWindow ();

	if (gl->fullscreen)
	{
		npPostMsg ("FullScreen Window", kNPmsgCtrl, dataRef);
		glutFullScreen ();
	}		


//		gl->stereo = false;		
/*
	if (result == 0)
	{
		npPostMsg ("FullScreen Window", kNPmsgCtrl, dataRef);
		glutShowWindow ();
		glutFullScreen ();
//		gl->stereo = false;								//stereo 3D, debug zz
	}
	else
	{	//GameMode may be different then what we requested, so get the modes
		glutSetWindowTitle("ANTz - GameMode");

		gl->width = glutGameModeGet( GLUT_GAME_MODE_WIDTH );
		gl->height = glutGameModeGet( GLUT_GAME_MODE_HEIGHT );
		gl->pixelDepth = glutGameModeGet( GLUT_GAME_MODE_PIXEL_DEPTH );
		gl->refreshRate = (float)glutGameModeGet( GLUT_GAME_MODE_REFRESH_RATE );
		printf("FullScreen Game Mode: %dx%d:%d@%d\n",gl->width,gl->height,
						gl->pixelDepth, (int)gl->refreshRate);
		//stereo 3D, turn off stereo if reresh rate is too low, update this, zz
		if (gl->refreshRate >= 99)
			printf("Stereo 3D enabled\n");
		else
		{
			if (gl->stereo)
				printf("Stereo 3D disabled, low refresh rate: %dHz\n", 
					gl->refreshRate);
			gl->stereo = false;
		}
	}
*/
	npPostMsg ("www.synglyphx.com", kNPmsgCtrl, dataRef);
	npPostMsg ("System Ready...", kNPmsgCtrl, dataRef);
	//glGetIntegerv (GL_TEXTURE_STACK_DEPTH, &depth); // GL_MODELVIEW_STACK_DEPTH
}


//------------------------------------------------------------------------------
void npCloseGlut (void* dataRef)
{
	npCloseGL (dataRef);

	return;
}

//------------------------------------------------------------------------------
void npAppLoopGlut(void* dataRef)
{
	npInitGlut (0, 0, dataRef);
	glutMainLoop();
}


//------------------------------------------------------------------------------
void npGlutDrawGLScene(void) 
{
	double deltaTime = 0.0;
	double currentTime = 0.0;
	double targePeriod = 0.0;

	pData data = (pData) npGetDataRef();
		
	//update data, positions, physics, user input
	npUpdateCtrl (data);

    if (data->io.mouse.tool == kNPtoolSelect)		//zz select
        glutSetCursor(GLUT_CURSOR_CROSSHAIR);
    else
        glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);

	//init time on first run
	npGLDrawScene (data);

    npGlutDrawHUD(data);

    int err = glGetError();
    if (err)
        printf("err: 2388 - OpenGL error: %d\n", err);

//	glFlush();		//zzhp							//vsync, not yet functional, debug zz
//	glFinish();		//zzhp

	//calculate the delta time using previously stored time
	currentTime = nposGetTime();
	deltaTime = currentTime - data->io.time;	//add clock roll-over handling, debug zz

	//if extra time left over then sleep for remainder of cycle
	if (deltaTime < data->io.cyclePeriod)
		nposSleep (data->io.cyclePeriod - deltaTime);

	glutPostRedisplay();		//supposed to help with window freezing,	debug zz
								//issue with mouse hitting edge of screen
	glutSwapBuffers();

//	glFinish();					//vsync

	//update the locally stored time, used when calculating the delta time
	nposUpdateTime (data);
}

void npGlutDrawHUD(void* dataRef) 
{
#define BUGFIX_MATRIX 0.0000000001	//prevents 0 vector issue

    pData data = (pData)dataRef;

    //MB-LABEL
    NPfloatXYZ	trans;
    NPfloatXYZA rot;

    trans.x = 0.0f;
    trans.y = 0.0f;
    trans.z = 100.0f;
    rot.x = 0.0f;
    rot.y = 0.0f;
    rot.z = -1.0f;
    rot.angle = 0.0f;
    //MB-END

    //MB-LABEL
    //HUD LookAt Camera
    glPushMatrix();
    glLoadIdentity();
    gluLookAt(trans.x, trans.y, trans.z,
        trans.x + rot.x,
        trans.x + rot.y + BUGFIX_MATRIX,
        trans.z + rot.z,
        0.0, 0.0, 1.0);

    npDrawHUD(data);

    glPopMatrix();
    //MB-LABEL-END
}

// draw 2D overlays including grids, text, messages, highlight objects, etc...
//------------------------------------------------------------------------------
void npDrawHUD(void* dataRef)
{
    float sc;
    pData data = (pData)dataRef;
    pNPhud hud = (pNPhud)&data->io.gl.hud;

    glPushMatrix();

    glDisable(GL_DEPTH_TEST);	//turn off depth testing so HUD is not obscured
    glDisable(GL_LIGHTING);		//disable lighting to draw 100% ambient

    //enable subtractive transparency unless already done
    if (data->io.gl.alphaMode != kNPalphaModeSubtractive)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    //scale to Screen Coords
    sc = 63.067f / data->io.gl.height;	//63.067f for 35mm fov, upgrade for any fov, debug zz
    glScalef(sc, sc, sc);

    //draws node text tags... //MB-LABEL
    if (hud->drawTags)
        npDrawNodeTags(data);

    if (data->io.mouse.tool == kNPtoolSelect)
        npDrawSelect(data);

    //zz debug, currently drawConsole is redundant to kNPconsoleLevelNull
    if (hud->drawConsole)
    {
        //draw FPS first puts it below other HUD elements	
        if (hud->drawFPS && hud->console.level > kNPconsoleLevelOne)
            npDrawFPS(data);

        //draw console before indicators, allows indicators to be clicked
        npDrawConsole(data);

        //draw compass based on level and mode
        if (hud->drawCompass
            && hud->console.level > kNPconsoleLevelOne
            && hud->console.mode != kNPconsoleMenu)
            npDrawCompass(data);
    }

    glPopMatrix();
}

//------------------------------------------------------------------------------

void npGlutDrawGLSceneIdle(void)
{
//	npGlutDrawGLScene();

	return;
}

//fullscreen GameMode creates a new GL context
//currently requires re-registering event callbacks and re-loading texture maps
//------------------------------------------------------------------------------
void npglFullscreen (void* dataRef)
{
	int deltaX = 0, deltaY = 0;
	int result = 0;		//fullscreen window used only if gamemode fails

	pData data = (pData) dataRef;
	pNPgl gl = &data->io.gl;

	if (gl->fullscreen)
	{
		if (glutGameModeGet(GLUT_GAME_MODE_ACTIVE) != 0)		//stereo 3D
		{
			glutLeaveGameMode();
			glutSetWindow (gl->windowID);
            glutShowWindow ();

			glutKeyboardFunc (npGlutKeyDown);
			glutKeyboardUpFunc (npGlutKeyUp);
			glutSpecialFunc (npGlutKeyDownSpecial);
			glutSpecialUpFunc (npGlutKeyUpSpecial);


			//register mouse events with GLUT
			glutMouseFunc (npMouseEvent);
			glutMotionFunc (npMouseMotion);

			glutDisplayFunc (npGlutDrawGLScene);
			glutIdleFunc (npGlutDrawGLSceneIdle);

			glutReshapeFunc (npGLResizeScene);
			//---
			npInitGL (data);
		}
	
		//exit fullscreen and restore previous window position
		gl->fullscreen = false;

		glutReshapeWindow (gl->windowSize.x, gl->windowSize.y);
		glutPositionWindow (gl->position.x, gl->position.y);

		//correct for window border offset, glut workaround
		deltaX = gl->position.x - glutGet((GLenum)GLUT_WINDOW_X);
		deltaY = gl->position.y - glutGet((GLenum)GLUT_WINDOW_Y);
		if (deltaX != 0 || deltaY != 0)
			glutPositionWindow (gl->position.x + deltaX,
								gl->position.y + deltaY);
		
		//zz debug err checking to make sure window is proper size!
/*
		if (x != gl->windowSize.x);
		{
			gl->windowSize.x = x;
			gl->windowSize.y = y;
			glutReshapeWindow (x, y);
			glutPositionWindow (100, 100);

			npPostMsg("err 2978 - window width mismatch", kNPmsgGL, data);
		}
*/

	
		npPostMsg("Exit FullScreen", kNPmsgGL, data);
	}
	else
	{
/*		glutSetWindow (	gl->windowID);
		glutHideWindow ();

		//Game Mode with stereo 3D
		if (gl->stereo)
			glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STEREO); // stereo display mode for glut
		else
			glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

		//stereo 3D and GameMode
		printf("Attempting Game Mode, please ignore GLUT warnings\n");
		glutGameModeString("1920x1080:32@121");
		if ( glutEnterGameMode() == 0 )
		{
			glutGameModeString("1920x1080:32@120");
			if ( glutEnterGameMode() == 0 )
			{
				glutGameModeString("1920x1080:32@119");
				if ( glutEnterGameMode() == 0 )
				{
					glutGameModeString("1920x1080:32@60"); //does not specify refresh
					result = ( glutEnterGameMode() );
		}}}

		//register keyboard events with the new GameMode GL context
		glutKeyboardFunc (npGlutKeyDown);
		glutKeyboardUpFunc (npGlutKeyUp);
		glutSpecialFunc (npGlutKeyDownSpecial);
		glutSpecialUpFunc (npGlutKeyUpSpecial);
		
		//register mouse events with GLUT
		glutMouseFunc (npMouseEvent);
		glutMotionFunc (npMouseMotion);

		glutDisplayFunc (npGlutDrawGLScene);
		glutIdleFunc (npGlutDrawGLSceneIdle);
		glutReshapeFunc (npGLResizeScene);
		
		//---
		npInitGL (data);
*/
		gl->position.x = glutGet((GLenum)GLUT_WINDOW_X);
		gl->position.y = glutGet((GLenum)GLUT_WINDOW_Y);

		gl->windowSize.x = gl->width;
		gl->windowSize.y = gl->height;

		if (result == 0)	//fullscreen window used only if gamemode fails
		{
			printf("FullScreen Window\n");
			glutShowWindow ();
			glutFullScreen ();
			//gl->stereo = false;							//stereo 3D, debug zz
		}
		else
		{	//GameMode may be different then what we requested, so get the modes
			//glutSetWindowTitle("ANTz - GameMode");

			gl->width = glutGameModeGet( GLUT_GAME_MODE_WIDTH );
			gl->height = glutGameModeGet( GLUT_GAME_MODE_HEIGHT );
			gl->pixelDepth = glutGameModeGet( GLUT_GAME_MODE_PIXEL_DEPTH );
			gl->refreshRate = (float)glutGameModeGet( GLUT_GAME_MODE_REFRESH_RATE );
			printf("FullScreen Game Mode: %dx%d:%d@%d\n",gl->width,gl->height,
							gl->pixelDepth, (int)gl->refreshRate);
			//stereo 3D, turn off stereo if reresh rate is too low, update this, zz
			if (gl->refreshRate >= 99)
			{
				printf("Stereo 3D enabled\n");
				gl->stereo = true;
			}
			else
			{
				printf("Stereo 3D disabled, low refresh rate\n");
				gl->stereo = false;
			}
		}

		gl->fullscreen = true;
	}
}


//------------------------------------------------------------------------------
void npGlutKeyDown (unsigned char key, int x, int y) 
{
	// printf("1 key: %d \n", key);
	npKeyGlut (key, x, y, kGlutKeyDown, 0);		// glutGetModifiers()); //zz debug, not a problem here, just no longer needed
}

//------------------------------------------------------------------------------
void npGlutKeyUp (unsigned char key, int x, int y) 
{
	// printf("2 key: %d \n", key);
	npKeyGlut (key, x, y, kGlutKeyUp, 0);		// glutGetModifiers()); //zz debug
}

//------------------------------------------------------------------------------
void npGlutKeyDownSpecial (int key, int x, int y) 
{
	// printf("3 key: %d \n", key);
	npKeyGlut (key, x, y, kGlutKeyDownSpecial, 0);	// glutGetModifiers());	
	
	//zz debug, if glutGetModifiers() called from a ...Special callback 
	//then err - "freeglut glutGetModifiers() called outside an input callback"
}

//------------------------------------------------------------------------------
void npGlutKeyUpSpecial (int key, int x, int y) 
{
	// printf("4 key: %d \n", key);
	npKeyGlut (key, x, y, kGlutKeyUpSpecial, 0);	// glutGetModifiers());	
}
