#include "npui.h"
#include "io/npconsole.h"
#include "npdata.h"
#include "io/npch.h"

//Game Mode executes dynamically mapped commands for RAW key events
//the default preset for game mode control commands is hard-coded here
//three types of fundamental keys: toggle, momentary, repeating
//and the odd-ball caps-lock which only sends (trigger) key down events 
//------------------------------------------------------------------------------
void npInitKey(void* dataRef)
{
    int i = 0;
    int j = 0;

    pData data = (pData)dataRef;
    pNPkey key = &data->io.key;

    //set all to zero
    key->modAlphaUpper = 0;

    key->modCapsLock = 0;

    key->modShift = 0;
    key->modCtrl = 0;
    key->modAlt = 0;
    key->modCommand = 0;

    key->modShiftLeft = 0;
    key->modShiftRight = 0;
    key->modCtrlLeft = 0;
    key->modCtrlRight = 0;
    key->modAltLeft = 0;
    key->modAltRight = 0;
    key->modCommandLeft = 0;
    key->modCommandRight = 0;


    for (i = 0; i < kNPkeyEventTypeSize; i++)
        for (j = 0; j < kNPkeyMapSize; j++)
            key->map[i][j] = 0;


    //Fire Commands only respond to key down event
    key->map[kKeyDown][kKeyCodeM] = kNPcmdMenu;

    key->map[kKeyDown][kKeyCodeBackSlash] = kNPcmdSubsample;			//zzhp

    key->map[kKeyDown][kKeyCodeY] = kNPcmdSegments;
    key->map[kKeyDown][kKeyCodeV] = kNPcmdTexture;

    key->map[kKeyDown][kKeyCodeESC] = kNPcmdFullscreen;
    //	key->map[kKeyDown][kKeyCodeB]				= kNPcmdClearFrameBuffer;
    key->map[kKeyDown][kKeyCodeB] = kNPcmdBackground;

    key->map[kKeyDown][kKeyCodeT] = kNPcmdTool;
    key->map[kKeyDown][kKeyCodeX] = kNPcmdSelectAxes;

    key->map[kKeyDown][kKeyCodeC] = kNPcmdCamera;
    key->map[kKeyDown][kKeyCodeG] = kNPcmdGrid;

    key->map[kKeyDown][kKeyCode4] = kNPcmdSelectAll;
    key->map[kKeyDown][kKeyCodeTilda] = kNPcmdSelectNone;
    //	key->map[kKeyDown][kKeyCodeSlash]			= kNPcmdSelectToggle; //zzsql
    key->map[kKeyDown][kKeyCode1] = kNPfileMapOne;
    key->map[kKeyDown][kKeyCode2] = kNPfileMapTwo;
    key->map[kKeyDown][kKeyCode3] = kNPfileMapThree;

    key->map[kKeyDown][kKeyCode5] = kNPcmdPresetOne;
    key->map[kKeyDown][kKeyCode6] = kNPcmdPresetTwo;
    key->map[kKeyDown][kKeyCode7] = kNPcmdPresetThree;
    //	key->map[kKeyDown][kKeyCode8]				= kNPcmdPresetFour;

    //	key->map[kKeyDown][kKeyCodeC]				= kNPcmdCenter;
    //	key->map[kKeyDown][kKeyCodeBackSlash]		= kNPcmdScroll;
    key->map[kKeyDown][kKeyCodeF] = kNPcmdFreeze;
    key->map[kKeyDown][kKeyCodeH] = kNPcmdHide;

    key->map[kKeyDown][kKeyCodeO] = kNPcmdGeometry;
    key->map[kKeyDown][kKeyCodeJ] = kNPcmdTopo;

    key->map[kKeyDown][kKeyCodeI] = kNPcmdTagMode;

    key->map[kKeyDown][kKeyCodeK] = kNPfileSave;
    key->map[kKeyDown][kKeyCodeL] = kNPfileOpen;
    key->map[kKeyDown][kKeyCodeP] = kNPfileImport;	// kNPcmdPoints;

    key->map[kKeyDown][kKeyCodeSlash] = kNPcmdViewer;

    key->map[kKeyDown][kKeyCodeU] = kNPopenURL;


    //	key->map[kKeyDown][kKeyCodeL]				= kNPcmdMesh;


    key->map[kKeyDown][kKeyCodeLeftBracket] = kNPcmdSetpointLo;
    key->map[kKeyDown][kKeyCodeRightBracket] = kNPcmdSetpointHi;

    key->map[kKeyDown][kKeyCodeSpace] = kNPcmdPause;

    //Repeat keys, don't usually respond to keyUp
    key->map[kKeyDown][kKeyCodeN] = kNPcmdNew;
    key->map[kKeyRepeat][kKeyCodeN] = kNPcmdNew;

    key->map[kKeyDown][kKeyCodeDelete] = kNPcmdDelete;
    key->map[kKeyRepeat][kKeyCodeDelete] = kNPcmdDelete;

    key->map[kKeyDown][kKeyCodeBackSpace] = kNPcmdDelete;
    key->map[kKeyRepeat][kKeyCodeBackSpace] = kNPcmdDelete;

    key->map[kKeyDown][kKeyCodeTab] = kNPcmdPin;
    key->map[kKeyRepeat][kKeyCodeTab] = kNPcmdPin;

    key->map[kKeyDown][kKeyCodeRightQuote] = kNPcmdNextBranch;
    key->map[kKeyRepeat][kKeyCodeRightQuote] = kNPcmdNextBranch;
    key->map[kKeyDown][kKeyCodeSemiColon] = kNPcmdPrevBranch;
    key->map[kKeyRepeat][kKeyCodeSemiColon] = kNPcmdPrevBranch;

    key->map[kKeyDown][kKeyCodeReturn] = kNPcmdConsole;
    key->map[kKeyRepeat][kKeyCodeReturn] = kNPcmdConsole;
    key->map[kKeyDown][kKeyCodeNumPadEnter] = kNPcmdConsole;
    key->map[kKeyRepeat][kKeyCodeNumPadEnter] = kNPcmdConsole;

    key->map[kKeyDown][kKeyCodeComma] = kNPcmdChannelDown;
    key->map[kKeyRepeat][kKeyCodeComma] = kNPcmdChannelDown;

    key->map[kKeyDown][kKeyCodePeriod] = kNPcmdChannelUp;
    key->map[kKeyRepeat][kKeyCodePeriod] = kNPcmdChannelUp;

    key->map[kKeyDown][kKeyCodeMinus] = kNPcmdColorDown;
    key->map[kKeyRepeat][kKeyCodeMinus] = kNPcmdColorDown;

    key->map[kKeyDown][kKeyCodeEqual] = kNPcmdColorUp;
    key->map[kKeyRepeat][kKeyCodeEqual] = kNPcmdColorUp;

    key->map[kKeyDown][kKeyCodeZero] = kNPcmdAlphaUp;
    key->map[kKeyRepeat][kKeyCodeZero] = kNPcmdAlphaUp;

    key->map[kKeyDown][kKeyCode9] = kNPcmdAlphaDown;
    key->map[kKeyRepeat][kKeyCode9] = kNPcmdAlphaDown;

    key->map[kKeyDown][kKeyCode8] = kNPcmdAlphaMode;
    key->map[kKeyRepeat][kKeyCode8] = kNPcmdAlphaMode;

    key->map[kKeyDown][kKeyCodeR] = kNPcmdRatio; //kNPcmdNormalMode;
    key->map[kKeyRepeat][kKeyCodeR] = kNPcmdRatio; //kNPcmdNormalMode;

    //	key->map[kKeyDown][kKeyCode]				= kNPcmdShaderMode;
    //	key->map[kKeyRepeat][kKeyCode]				= kNPcmdShaderMode;

    //	key->map[kKeyDown][kKeyCode]				= kNPcmdBiasUp;
    //	key->map[kKeyRepeat][kKeyCode]				= kNPcmdBiasUp;
    //	key->map[kKeyDown][kKeyCode]				= kNPcmdBiasDown;
    //	key->map[kKeyRepeat][kKeyCode]				= kNPcmdBiasDown;

    //	key->map[kKeyDown][kKeyCode]				= kNPcmdGainUp;
    //	key->map[kKeyRepeat][kKeyCode]				= kNPcmdGainUp;
    //	key->map[kKeyDown][kKeyCode]				= kNPcmdGainDown;
    //	key->map[kKeyRepeat][kKeyCode]				= kNPcmdGainDown;


    //Momentary Keys are active while pressed and disable upon keyup
    //Arrow keys for X,Y,Z translate, rotate and scale of selected objects

    // translate
    key->map[kKeyDown][kKeyCodeA] = kNPcmdXdecrease;
    key->map[kKeyUp][kKeyCodeA] = kNPcmdXdecreaseOff;

    key->map[kKeyDown][kKeyCodeD] = kNPcmdXincrease;
    key->map[kKeyUp][kKeyCodeD] = kNPcmdXincreaseOff;

    key->map[kKeyDown][kKeyCodeW] = kNPcmdYincrease;
    key->map[kKeyUp][kKeyCodeW] = kNPcmdYincreaseOff;

    key->map[kKeyDown][kKeyCodeS] = kNPcmdYdecrease;
    key->map[kKeyUp][kKeyCodeS] = kNPcmdYdecreaseOff;

    key->map[kKeyDown][kKeyCodeE] = kNPcmdZincrease;
    key->map[kKeyUp][kKeyCodeE] = kNPcmdZincreaseOff;

    key->map[kKeyDown][kKeyCodeQ] = kNPcmdZdecrease;
    key->map[kKeyUp][kKeyCodeQ] = kNPcmdZdecreaseOff;

    // rotate
    key->map[kKeyDown][kKeyCodeLeft] = kNPcmdRotateLeft;
    key->map[kKeyUp][kKeyCodeLeft] = kNPcmdRotateLeftOff;

    key->map[kKeyDown][kKeyCodeRight] = kNPcmdRotateRight;
    key->map[kKeyUp][kKeyCodeRight] = kNPcmdRotateRightOff;

    key->map[kKeyDown][kKeyCodeUp] = kNPcmdRotateUp;
    key->map[kKeyUp][kKeyCodeUp] = kNPcmdRotateUpOff;

    key->map[kKeyDown][kKeyCodeDown] = kNPcmdRotateDown;
    key->map[kKeyUp][kKeyCodeDown] = kNPcmdRotateDownOff;

    //	key->map[kKeyDown][kKeyCodeLeft]			= kNPcmdRotateCCW;
    //	key->map[kKeyUp][kKeyCodeLeft]				= kNPcmdRotateCCWOff;

    //	key->map[kKeyDown][kKeyCodeRight]			= kNPcmdRotateCW;
    //	key->map[kKeyUp][kKeyCodeRight]				= kNPcmdRotateCWOff;

    // scale
    key->map[kKeyDown][kKeyCodeZ] = kNPcmdZoomOn;
    key->map[kKeyUp][kKeyCodeZ] = kNPcmdZoomOff;
}


//------------------------------------------------------------------------------
void npCloseKey(void* dataRef)
{
    return;
}

//------------------------------------------------------------------------------
float npLowPassFilter(float start, float target, float fac)
{
    return (float)(fac*target + (1.0f - fac)*start);
}

ANTZCORE_API void npSetCamTarget(void* dataRef)
{
    pData data = (pData)dataRef;
    pNPnode node = data->map.selectedPinNode; //.node[data->map.selectedPinIndex];
    npSetCamTargetNode(node, data);
}

ANTZCORE_API void npSetCamTargetNode(pNPnode node, void* dataRef)
{
    float dX = 0.0f, dY = 0.0f, dZ = 0.0f;
    float cdX = 0.0f, cdY = 0.0f, cdZ = 0.0f;

    pData data = (pData)dataRef;

    pNPnode cam = data->map.currentCam;
    
    pNPnode rootGrid = data->map.node[kNPnodeRootGrid];
    pNPmouse mouse = (pNPmouse)&data->io.mouse;

    if (mouse->pickMode == kNPmodeGrid)									//zz debug
        node = data->map.selectedGrid;

    if (node == NULL)
        return;
    /*
    mouse->targetDest.x = node->translate.x;
    mouse->targetDest.y = node->translate.y;
    mouse->targetDest.z = node->translate.z + kNPoffsetPin * node->scale.z;

    dX = cam->translate.x - mouse->targetDest.x;
    dY = cam->translate.y - mouse->targetDest.y;
    dZ = cam->translate.z - mouse->targetDest.z;

    mouse->targetRadius = sqrtf(dX*dX + dY*dY + dZ*dZ);

    //true on first pass
    if (1)//cam->proximity.x == 0.0f)
    cam->proximity.x = mouse->targetRadius;

    //set target based on radius
    cdX = -sinf(cam->rotate.y / kRADtoDEG) * cam->proximity.x;
    cdY = -cosf(cam->rotate.y / kRADtoDEG) * cam->proximity.x;
    cdZ = sinf(cam->rotate.x / kRADtoDEG) * cam->proximity.x;

    mouse->target.x = cdX + mouse->targetDest.x;
    mouse->target.y = cdY + mouse->targetDest.y;
    mouse->target.z = cdZ + mouse->targetDest.z;
    */

    mouse->targetDest.x = node->world.x;
    mouse->targetDest.y = node->world.y;
    mouse->targetDest.z = node->world.z;

    //set target of root pin or rod topo types to top of object
    //sub-node world coordinates are already corrected for the top of object
    if (node->branchLevel == 0)
    {
        if (node->topo == kNPtopoPin
            && (node->geometry == kNPgeoPin || node->geometry == kNPgeoPinWire))
            mouse->targetDest.z += kNPoffsetPin * node->scale.z;
        else if (node->topo == kNPtopoRod)
            mouse->targetDest.z += kNPoffsetRod * node->scale.z;
    }

    dX = cam->translate.x - mouse->targetDest.x;
    dY = cam->translate.y - mouse->targetDest.y;
    dZ = cam->translate.z - mouse->targetDest.z;

    mouse->targetRadius = sqrtf(dX*dX + dY*dY + dZ*dZ);

    //true on first pass
    if (cam->proximity.x == 0.0f)
        cam->proximity.x = mouse->targetRadius;

    //set target based on radius
    cdX = -sinf(cam->rotate.y / kRADtoDEG) * cam->proximity.x;
    cdY = -cosf(cam->rotate.y / kRADtoDEG) * cam->proximity.x;
    cdZ = sinf((-(cam->rotate.x) + 90.0f) / kRADtoDEG) * cam->proximity.x;

    mouse->target.x = cdX + mouse->targetDest.x;
    mouse->target.y = cdY + mouse->targetDest.y;
    mouse->target.z = cdZ + mouse->targetDest.z;

    mouse->targeting = true;

    /*	printf ("cdX: %f  %f  %f \n", cdX,cdY,cdZ);

    printf ("%f  %f  %f  r: %f  rD: %f\n", mouse->targetDest.x,
    mouse->targetDest.y,
    mouse->targetDest.z,
    mouse->targetRadius,
    cam->proximity.x);
    */
}

// update IO devices such as keyboard, mice, video, TNG3B and other inputs
//------------------------------------------------------------------------------
void npUpdateIO(void* dataRef)
{
    pData data = (pData)dataRef;

    data->io.cycleCount++;

    //we double buffer the mouse delta movement to maintain engine cycle sync
    npUpdateMouse(dataRef);

    npUpdateConsole(dataRef);

    npUpdateCh(dataRef);	//zz-JJ
}

// we double buffer the mouse delta to maintain engine sync
// mouse position is averaged over several cycles to smooth
//-----------------------------------------------------------------------------

#define kNPavgCount 10		//the number of cycles to average mouse movements

void npUpdateMouse(void* dataRef)
{
    int i = 0;
    float totalX = 0.0f;
    float totalY = 0.0f;
    static float x[kNPavgCount] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    static float y[kNPavgCount] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    static int count = 0;

    pData data = (pData)dataRef;


    x[count] = data->io.mouse.deltaSum.x;
    y[count] = data->io.mouse.deltaSum.y;

    for (i = 0; i < kNPavgCount; i++)
    {
        totalX += x[i];
        totalY += y[i];
    }

    data->io.mouse.delta.x = totalX / (float)kNPavgCount;
    data->io.mouse.delta.y = totalY / (float)kNPavgCount;

    if (data->io.mouse.mode != kNPmouseModeCamFlyA)
        data->io.mouse.deltaSum.x = 0.0f;

    data->io.mouse.deltaSum.y = 0.0f;


    count++;
    if (count >= kNPavgCount)
        count = 0;
}