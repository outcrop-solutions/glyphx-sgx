#include "nptext.h"
#include "io/gl/npgldraw.h"
#include "io/gl/nptags.h"
#include "npdata.h"

#include <stdio.h>

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

//Draw the Text Labels (Simple Ring now)
//------------------------------------------------------------------------------
void npDrawNodeTags(void* dataRef)
{
    int i = 0;

    pData data = (pData)npGetDataRef();
    pNPnode node;

    glPushMatrix();

    //offset screen coordinates such that 0,0 is in lower left corner
    glTranslatef(data->io.gl.width * -0.5f, data->io.gl.height * -0.5f, 0.0f);

    for (i = 0; i < data->io.gl.hud.tags.count; i++)
    {
        node = data->io.gl.hud.tags.list[i];					//change to sort[], debug zz

        if (node == NULL)
        {
            printf("err 4901 - null node in text tags list");
            continue;
        }

        glPushMatrix();

        glTranslatef(node->screen.x, node->screen.y, 0.0f);

        //	node->tag = &data->io.gl.hud.fps;

        npDrawNodeTextTag(node, data);

        glPopMatrix();
    }
    data->io.gl.hud.tags.count = 0;

    glPopMatrix();
}

//draws the text and optional background box with outline
//------------------------------------------------------------------------------
void npDrawTextTag(pNPtag tag, void* dataRef)
{
    glColor4ub(tag->boxColor.r, tag->boxColor.g, tag->boxColor.b, tag->boxColor.a);

    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(tag->boxSize.x, 0.0f);
    glVertex2f(tag->boxSize.x, tag->boxSize.y);
    glVertex2f(0.0f, tag->boxSize.y);
    glEnd();

    glColor4ub(tag->lineColor.r, tag->lineColor.g, tag->lineColor.b, tag->lineColor.a);

    glBegin(GL_LINE_LOOP);
    glVertex2f(1.0f, 2.0f);
    glVertex2f(tag->boxSize.x - 2.0f, 2.0f);
    glVertex2f(tag->boxSize.x - 2.0f, tag->boxSize.y - 1.0f);
    glVertex2f(1.0f, tag->boxSize.y - 1.0f);
    glEnd();

    glColor4ub(tag->color.r, tag->color.g, tag->color.b, tag->color.a);

    //offset for text margin inside the background box
    glRasterPos2f(5.0f, 6.0f);
    npGlutDrawString(GLUT_BITMAP_9_BY_15, tag->title);
}

//draws the text and optional background box with outline
//------------------------------------------------------------------------------
void npDrawNodeTextTag(pNPnode node, void* dataRef)
{
    int idRed = 0, idGrn = 0, idBlu = 0;
    pData data = (pData)dataRef;
    pNPtag tag = node->tag;


    if (node->hide)
        return;

    //allow for picking a node by clicking on the text tag
    if (data->io.gl.pickPass)
    {											//Convert nodeID to RGB triplet
        npIDtoRGB(node->id, &idRed, &idGrn, &idBlu);
        glColor4ub(idRed, idGrn, idBlu, 255);	//nodeID encoded as pixel color

        glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(tag->boxSize.x, 0.0f);
        glVertex2f(tag->boxSize.x, tag->boxSize.y);
        glVertex2f(0.0f, tag->boxSize.y);
        glEnd();

        return;		//only need to draw the quad for pickPass, exit to save time
    }

    //draw background box unless the most basic mode ...ModeHUD
    if (1)//node->tagMode != kNPtagModeHUD)	//too hard to read without box, debug zz
    {
        glColor4ub(tag->boxColor.r, tag->boxColor.g, tag->boxColor.b, tag->boxColor.a);

        glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(tag->boxSize.x, 0.0f);
        glVertex2f(tag->boxSize.x, tag->boxSize.y);
        glVertex2f(0.0f, tag->boxSize.y);
        glEnd();
    }

    //either box outline or text drawn the same color as the node, not both
    if (node->tagMode == kNPtagModeBoxOutlineHUD)
    {
        //outline box using node color with fixed 50% opacity, unless HUD item
        if (node->type == kNodeHUD)
            glColor4ub(node->color.r, node->color.g, node->color.b, node->color.a);
        else
            glColor4ub(node->color.r, node->color.g, node->color.b, 128); //50%

        glBegin(GL_LINE_LOOP);
        glVertex2f(1.0f, 2.0f);
        glVertex2f(tag->boxSize.x - 2.0f, 2.0f);
        glVertex2f(tag->boxSize.x - 2.0f, tag->boxSize.y - 1.0f);
        glVertex2f(1.0f, tag->boxSize.y - 1.0f);
        glEnd();

        //using the consoles text color
        glColor4ub(tag->color.r, tag->color.g, tag->color.b, tag->color.a);
    }
    else	//else text color is node color with fixed 80% opacity
        glColor4ub(node->color.r, node->color.g, node->color.b, 205);


    //offset for text margin inside the background box
    glRasterPos2f(5.0f, 6.0f);

    npGlutDrawString(GLUT_BITMAP_9_BY_15, tag->title);
}

//------------------------------------------------------------------------------
void npGlutDrawString(void *font, char *str)
{
    int i = 0, size = 0;

    size = strlen(str);

    for (i = 0; i < size; i++)
        glutBitmapCharacter(font, str[i]);
}

//------------------------------------------------------------------------------
void npDrawFPS(void* dataRef)
{
    float fps = 0.0f;

    pData data = (pData)dataRef;
    pNPtag tag = &data->io.gl.hud.fps;

    //zz debug, does not allow mouse picking the FPS, update to allow hiding
    if (data->io.gl.pickPass)
        return;

    if (data->io.cycleDelta)
        fps = (float)(1.0 / data->io.cycleDelta);

    sprintf(tag->title, "FPS:%6.2f", fps);

    //updates the box size, we really only need to do this once, debug zz
    npUpdateTextTag(tag);

    glPushMatrix();
    glTranslatef(data->io.gl.width * 0.5f - 105.0f,
        data->io.gl.height * 0.5f - 25.0f, 0.0f);
    //now draw the tag
    npDrawTextTag(tag, data);
    glPopMatrix();
}

//zz debug, add handling for character color map using index colors
//zz debug, add character selection using pickPass and/or mouseXY to cell calc
//zz debug, add inverse blinking cursor, underbar, vertical bar and whole char
//draw console GUI messages based on mode
//------------------------------------------------------------------------------
void npDrawConsole(void* dataRef)
{
    int i = 0;
    int index = 0;
    int level = 0;

    float charWidth = 9.0f;
    float charHeight = 15.0f;

    float boxHeight = 0.0f;			//top edge of text background box
    float boxWidth = 0.0f;

    int lineCount = 0;

    pData data = (pData)dataRef;
    pNPconsole console = (pNPconsole)&data->io.gl.hud.console;

    //exit to not draw if mode is null
    if (console->level == 0 && console->mode == 0)		//update to use the hide 
        return;

    //zz debug, does not allow mouse picking the console, update for text entry
    if (data->io.gl.pickPass)
        return;

    //set the number of text rows
    switch (console->level)
    {
    case kNPconsoleLevelOne: lineCount = 1; break;
    case kNPconsoleLevelThree: lineCount = 3; break;
    case kNPconsoleLevelMax: lineCount = kNPconsoleLineMax; break;
    }

    //if viewer mode then set console lines to max
    if (console->mode == kNPconsoleMenu)					//zz debug, move this to npconsole.c
        lineCount = kNPconsoleLineMax;

    //calculate background box with margin
    boxWidth = console->box.x;//charsPerLine * charWidth + 14.0f;		//zz debug
    boxHeight = lineCount * charHeight + 8.0f;

    //draw background box
    glColor4ub(console->boxColor.r, console->boxColor.g, console->boxColor.b, console->boxColor.a);

    glPushMatrix();
    glTranslatef(console->screen.x, console->screen.y, 0.0f);

    glBegin(GL_QUADS);
    glVertex2f(0.0f, 0.0f);
    glVertex2f(boxWidth, 0.0f);
    glVertex2f(boxWidth, boxHeight);
    glVertex2f(0.0f, boxHeight);
    glEnd();

    //draw text color
    glColor4ub(console->color.r, console->color.g, console->color.b, console->color.a);
    glTranslatef(4.0f, 7.0f, 0.0f);	//text margin offset

    //set the initial line index for the draw text loop
    index = console->lineIndex;

    //draw number of text lines specified by lineCount
    for (i = 0; i < lineCount; i++)
    {
        //set position and draw text by line index
        glRasterPos2f(0.0f, charHeight * (float)i);
        npGlutDrawString(GLUT_BITMAP_9_BY_15, &console->line[index--][0]);

        if (index < 0)
            index = kNPconsoleLineMax - 1;		//handles buffer roll-over
    }

    //draw blinking cursor on top of text using inverse color
    if (console->cursorShow && data->io.blinkState)
    {
        glEnable(GL_COLOR_LOGIC_OP);
        glLogicOp(GL_XOR);
        glRasterPos2f(charWidth * (float)console->cursorColumn,
            charHeight * (float)console->cursorLine);
        npGlutDrawString(GLUT_BITMAP_9_BY_15, "_");
        glDisable(GL_COLOR_LOGIC_OP);
    }

    glPopMatrix();
}