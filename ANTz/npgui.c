#include "npgui.h"
#include "npglut.h"
#include "npctrl.h"
#include "data/npmapfile.h"
#include "io/npgl.h"
#include "io/gl/nptags.h"

#include "nptext.h"

#ifdef WIN32
#include <Windows.h>
#include <commdlg.h>
#endif

#include <io.h>

void npCtrlCommandGUI(int command, void* dataRef)
{
    pData data = (pData)dataRef;

    switch (command)
    {
        case kNPcmdFullscreen: 
            npglFullscreen(data); 
            break;
        case kNPfileNew: npFileDialog(NULL, kNPfileDialogNew, dataRef); break;

            //navigate folders, files and DBs
            /*case kNPcmdViewer :
            if (data->io.key.modShift)
            npSaveAntzStateToDatabase(data->io.connect, dataRef);			//zzsql
            else
            npViewer (dataRef);
            break;*/
            //		case kNPcmdFileOpen : npFileOpen (dataRef); break;
        case kNPfileOpen: npFileBrowser(dataRef); break;
        case kNPfileClose: npFileDialog(NULL, kNPfileDialogClose, dataRef);
            break;
        case kNPfileSaveAs: npFileDialog(NULL, kNPfileDialogSaveAs, dataRef);
            break;
        case kNPfileExport: npFileDialog(NULL, kNPfileDialogExport, dataRef); break;
        default: 
            npCtrlCommand(command, dataRef); 
            break;
    }
}

FILE* SaveFileDialog()
{
#ifdef WIN32
    FILE* filePtr;
    OPENFILENAME tmpOfn;
    HANDLE hwnd = NULL;
    //	char fileBuff[260];	// buffer for file name
    LPTSTR testString[260];
    LPCTSTR* filterStr = (LPCTSTR*)"All\0*.*\0\0";
    //	const TCHAR* filterStr = "All\0*.*\0\0";

    int i = 0;

    ZeroMemory(&tmpOfn, sizeof(tmpOfn));
    tmpOfn.lStructSize = sizeof(tmpOfn);
    tmpOfn.hwndOwner = hwnd;
    tmpOfn.lpstrFile = (LPTSTR)testString;
    tmpOfn.lpstrFile[0] = '\0';
    tmpOfn.nMaxFile = sizeof(testString);
    //	tmpOfn.lpstrFilter = (LPWSTR)"All\0*.*\0Text\0*.TXT\0";
    tmpOfn.lpstrFilter = NULL;
    tmpOfn.nFilterIndex = 1;
    tmpOfn.lpstrFileTitle = NULL;
    tmpOfn.nMaxFileTitle = 0;
    tmpOfn.lpstrInitialDir = NULL;
    tmpOfn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    printf("\nSave File Dialog Function Called\n");

    if (GetSaveFileName(&tmpOfn) == TRUE)
    {

        if ((filePtr = _wfopen((wchar_t*)tmpOfn.lpstrFile, L"w")) != NULL)
        {
            printf("NOT NULL");
            return filePtr;
        }
        else
        {
            printf("NULL");
            return NULL;
        }

        /*
        if(filePtr == NULL)
        {
        printf("filePtr is NULL");
        }
        */


        printf("\nSave Returned True\n");
        return filePtr;
    }
    else
    {
        printf("\nSave Returned False\n");
        return NULL;
    }
#endif
}
/*
enum
{
parameterNULL,
includeAllFiles,
kNPfileDialogCount

};
*/

//zz debug make this return the fileName (includes path) instead of opening the FILE*
//------------------------------------------------------------------------------
FILE* OpenFileDialog(const char* fileName, int kNPfileDialogOpen, void* dataRef)
{
#ifdef WIN32
    FILE* filePtr = NULL;
    int h = 0;
    int pathSize = 0;
    char msg[kNPmaxPath + 64];

    OPENFILENAME ofn;			// common dialog box structure
    char szFile[MAX_PATH];		// buffer for file name
    //	char dirPath[MAX_PATH];		// buffer for file name
    HWND hwnd = NULL;			// owner window
    HANDLE hf;					// file handle


    szFile[0] = '\0';

    //	nposSetCWD ("C:\\data\\");					//zz debug
    //	GetCurrentDirectory (pathSize, dirPath);

    //	nposGetCWD (dirPath, &pathSize);
    //	strcat (dirPath, "antzcsv\\");

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;		//hf
    ofn.lpstrFile = (LPSTR)szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not	//zz ???
    // use the contents of szFile to initialize itself.
    //	ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = NULL;// "All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = (LPCSTR)fileName;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box.
    if (GetOpenFileName(&ofn) == TRUE)
        hf = CreateFile(ofn.lpstrFile,
        GENERIC_READ,
        0,
        (LPSECURITY_ATTRIBUTES)NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        (HANDLE)NULL);
    else
    {
        npPostMsg("File Open Dialog Cancelled", kNPmsgCtrl, dataRef);
        return NULL;
    }

    //prevents crash if app exited while dialog is open
    if (hf == INVALID_HANDLE_VALUE)
    {
        npPostMsg("err 2794 - File Invalid", kNPmsgErr, dataRef);
        return NULL;
    }

    h = _open_osfhandle((long)hf, 0);

    filePtr = _fdopen(h, "r");

    if (filePtr != NULL)
    {
        sprintf(msg, "File Open: %s", &szFile);
        npPostMsg(msg, kNPmsgCtrl, dataRef);
    }
    else
        npPostMsg("err 2795 - File Pointer is NULL", kNPmsgErr, dataRef);

    return filePtr;
#endif
}


//-----------------------------------------------------------------------------
// Take file path returned from microsoft createFile, put into fopen and return File Pointer
FILE* nposFileDialog(const char* fileName, int dialogType, void* dataRef)
{
    FILE *filePtr = NULL;

    switch (dialogType)
    {
    case kNPfileDialogNew:
        nposFileDialog(fileName, kNPfileDialogOpen, dataRef);
        break;

    case kNPfileDialogOpen:
        filePtr = OpenFileDialog(fileName, kNPfileDialogOpen, dataRef);
        break;

    case kNPfileDialogClose:
        nposFileDialog(fileName, kNPfileDialogSaveAs, dataRef);
        break;

    case kNPfileDialogSave:
        nposFileDialog(fileName, kNPfileDialogSaveAs, dataRef);
        break;

    case kNPfileDialogSaveAs:
        filePtr = SaveFileDialog(fileName);
        break;

    case kNPfileDialogImport:
        nposFileDialog(fileName, kNPfileDialogOpen, dataRef);
        break;
    case kNPfileDialogExport:
        nposFileDialog(fileName, kNPfileDialogSaveAs, dataRef);
        break;

    default: break;
    }

    return filePtr;
}

//-----------------------------------------------------------------------------
int nposShowCursor(int hide)
{
    if (hide)
        return ShowCursor(TRUE);
    else
        return ShowCursor(FALSE);
}
//-----------------------------------------------------------------------------
int nposSetCursorPos(int x, int y)
{
    return SetCursorPos(x, y);
}

//zz replace the OS file dialog with custom in GL
//-----------------------------------------------------------------------------
FILE* npFileDialog(const char* fileName, int dialogType, void* dataRef)
{
    FILE* file = NULL;

    switch (dialogType)
    {
    case kNPfileDialogNew:
        file = nposFileDialog(fileName, dialogType, dataRef); break;

    case kNPfileDialogOpen:
        file = nposFileDialog(fileName, dialogType, dataRef); break;
    case kNPfileDialogClose:
        file = nposFileDialog(fileName, dialogType, dataRef); break;

    case kNPfileDialogSave:
        file = nposFileDialog(fileName, dialogType, dataRef); break;
    case kNPfileDialogSaveAs:
        file = nposFileDialog(fileName, dialogType, dataRef); break;

    case kNPfileDialogImport:
        file = nposFileDialog(fileName, dialogType, dataRef); break;
    case kNPfileDialogExport:
        file = nposFileDialog(fileName, dialogType, dataRef); break;

        // pass through unknown dialog types to the os file handler
    default: file = nposFileDialog(fileName, dialogType, dataRef); break;
    }

    return file;
}

//choose a folder, file or DB, can select multiple items
//launches the OS file dialog from a thread
//------------------------------------------------------------------------------
void npFileBrowser(void* dataRef)
{
    FILE* file = NULL;

    pData data = (pData)dataRef;

    if (data->io.gl.fullscreen)	//workaround, gl file dialog conflict
    {
        npCtrlCommandGUI(kNPcmdFullscreen, data);
        file = npFileDialog(data->io.file.csvPath, kNPfileDialogOpen, data);
        npCtrlCommandGUI(kNPcmdFullscreen, data);
    }
    else
        file = npFileDialog(NULL, kNPfileDialogOpen, data);

    npFileOpenAuto(NULL, file, data);
    //	npCSVtoMap (file, kNPtableNode, dataRef);			//move this, debug zz
    npPostTool(NULL, data);											//zz-s debug, remove... or change defaults

    printf("Sync Tags...\n");
    npSyncTags(dataRef);								//upgrade this, debug zz
    npPostMsg("Sync Done!", kNPmsgCtrl, dataRef);

    return;
}

//------------------------------------------------------------------------------
void npDrawCompass(void* dataRef)
{
    pData data = (pData)dataRef;
    pNPnode hudParent = data->map.node[kNPnodeRootHUD];
    pNPnode hudItem = NULL;
    pNPnode node = data->map.currentNode;
    pNPnode parent = node->parent;

    int i = 0;
    int parentTopo = 0;
    NPboolXYZ axes = data->io.axes;	//for convienance
    static const NPubyteRGBA black = { 0, 0, 0, 85 };
    static const NPubyteRGBA grey = { 127, 127, 127, 127 };
    static const NPubyteRGBA green = { 0, 100, 0, 127 };
    static const NPubyteRGBA red = { 255, 0, 0, 200 };
    static const NPubyteRGBA yellow = { 255, 255, 0, 220 };


    if (parent != NULL)
        parentTopo = parent->topo;

    //pNPtag tag = &data->io.gl.hud.fps;								//zz debug

    //draw with Heading for tile or as compass directions
    hudItem = hudParent->child[kNPhudCompass];
    if (hudItem->childIndex)
        sprintf(hudItem->tag->title, "Heading: %6.2f", node->rotate.y);
    else
    {
        if (node->rotate.y > 337.5f || node->rotate.y < 22.5f)
            sprintf(hudItem->tag->title, "   %6.2f N    ", node->rotate.y);
        else if (node->rotate.y > 292.5f)
            sprintf(hudItem->tag->title, "   %6.2f NW   ", node->rotate.y);
        else if (node->rotate.y > 247.5f)
            sprintf(hudItem->tag->title, "   %6.2f W    ", node->rotate.y);
        else if (node->rotate.y > 202.5f)
            sprintf(hudItem->tag->title, "   %6.2f SW   ", node->rotate.y);
        else if (node->rotate.y > 157.5f)
            sprintf(hudItem->tag->title, "   %6.2f S    ", node->rotate.y);
        else if (node->rotate.y > 112.5f)
            sprintf(hudItem->tag->title, "   %6.2f SE   ", node->rotate.y);
        else if (node->rotate.y > 67.5f)
            sprintf(hudItem->tag->title, "   %6.2f E    ", node->rotate.y);
        else if (node->rotate.y > 22.5f)
            sprintf(hudItem->tag->title, "   %6.2f NE   ", node->rotate.y);
    }
    npUpdateTextTag(hudItem->tag);

    //draw XYZ axes with green for active and grey for disabled
    if (node->scaleRate.x || node->scaleRate.y || node->scaleRate.z
        || data->io.mouse.tool == kNPtoolSize
        && data->map.currentNode != data->map.currentCam
        || (node->branchLevel && data->io.mouse.buttonR //does not handle if root is a torus, no ratio displayed, zz debug
        && data->io.mouse.pickMode == kNPmodePin
        && (data->io.mouse.tool == kNPtoolCombo
        || data->io.mouse.tool == kNPtoolCreate)))
    {
        hudItem = hudParent->child[kNPhudAngle];
        sprintf(hudItem->tag->title, "Ratio:  %7.2f", node->ratio);
        npUpdateTextTag(hudItem->tag);
        if (data->io.mouse.buttonR)
            hudItem->tag->boxColor = green;			//green if active
        else
            hudItem->tag->boxColor = grey;			//grey if unavailable

        hudItem = hudParent->child[kNPhudCoordX];
        sprintf(hudItem->tag->title, "Scale X:%7.2f", node->scale.x);
        npUpdateTextTag(hudItem->tag);
        if (axes.x)
        {
            if (data->io.mouse.buttonR)
                hudItem->tag->boxColor = grey;
            else
                hudItem->tag->boxColor = green;
        }
        else
            hudItem->tag->boxColor = black;

        hudItem = hudParent->child[kNPhudCoordY];
        sprintf(hudItem->tag->title, "Scale Y:%7.2f", node->scale.y);
        npUpdateTextTag(hudItem->tag);
        if (axes.y)
        {
            if (data->io.mouse.buttonR)
                hudItem->tag->boxColor = grey;
            else
                hudItem->tag->boxColor = green;
        }
        else
            hudItem->tag->boxColor = black;

        hudItem = hudParent->child[kNPhudCoordZ];
        sprintf(hudItem->tag->title, "Scale Z:%7.2f", node->scale.z);
        npUpdateTextTag(hudItem->tag);
        if (axes.z)
        {
            if (data->io.mouse.buttonR)
                hudItem->tag->boxColor = grey;
            else
                hudItem->tag->boxColor = green;
        }
        else
            hudItem->tag->boxColor = black;
    }
    else if (data->io.mouse.tool == kNPtoolRotate
        && data->map.currentNode != data->map.currentCam)
    {
        hudItem = hudParent->child[kNPhudAngle];
        if (hudItem->childIndex)					//clean this method up, zz debug
            sprintf(hudItem->tag->title, "Roll:   %7.2f", node->rotate.z);
        else
            sprintf(hudItem->tag->title, "Tilt:   %7.2f", node->rotate.x);
        hudItem->tag->boxColor = black;

        hudItem = hudParent->child[kNPhudCoordX];
        sprintf(hudItem->tag->title, "RotateX:%7.2f", node->rotate.x);
        npUpdateTextTag(hudItem->tag);
        if (axes.x)
        {
            if (data->io.mouse.buttonR)
                hudItem->tag->boxColor = grey;
            else
                hudItem->tag->boxColor = green;
        }
        else
            hudItem->tag->boxColor = black;

        hudItem = hudParent->child[kNPhudCoordY];
        sprintf(hudItem->tag->title, "RotateY:%7.2f", node->rotate.y);
        npUpdateTextTag(hudItem->tag);
        if (axes.y)
        {
            if (data->io.mouse.buttonR)
                hudItem->tag->boxColor = grey;
            else
                hudItem->tag->boxColor = green;
        }
        else
            hudItem->tag->boxColor = black;

        hudItem = hudParent->child[kNPhudCoordZ];
        sprintf(hudItem->tag->title, "RotateZ:%7.2f", node->rotate.z);
        npUpdateTextTag(hudItem->tag);
        if (axes.z)
        {
            if (data->io.mouse.buttonR || !(axes.x && axes.y))
                hudItem->tag->boxColor = green;
            else
                hudItem->tag->boxColor = grey;
        }
        else
        {
            if (data->io.mouse.buttonR)
                hudItem->tag->boxColor = green;
            else
                hudItem->tag->boxColor = black;
        }
    }
    else if ((data->io.mouse.tool == kNPtoolCombo
        || data->io.mouse.tool == kNPtoolCreate
        || data->io.mouse.tool == kNPtoolMove)
        && data->map.currentNode != data->map.currentCam)
    {
        hudItem = hudParent->child[kNPhudAngle];
        if (hudItem->childIndex)					//clean this method up, zz debug
            sprintf(hudItem->tag->title, "Roll:   %7.2f", node->rotate.z);
        else
            sprintf(hudItem->tag->title, "Tilt:   %7.2f", node->rotate.x);
        npUpdateTextTag(hudItem->tag);
        hudItem->tag->boxColor = black;

        hudItem = hudParent->child[kNPhudCoordX];
        sprintf(hudItem->tag->title, "Coord X:%7.2f", node->translate.x);
        npUpdateTextTag(hudItem->tag);
        if (axes.x)
            hudItem->tag->boxColor = green;
        else
        {
            if (axes.x)
                hudItem->tag->boxColor = grey;
            else
                hudItem->tag->boxColor = black;
        }

        hudItem = hudParent->child[kNPhudCoordY];
        sprintf(hudItem->tag->title, "Coord Y:%7.2f", node->translate.y);
        npUpdateTextTag(hudItem->tag);
        if (axes.y)
        {
            if (!(axes.x && axes.y))
                hudItem->tag->boxColor = green;
            else if (node->branchLevel == 0 && !axes.y)
                hudItem->tag->boxColor = black;
            else
            {
                if (parentTopo != kNPtopoPin && parentTopo != kNPtopoRod)
                    hudItem->tag->boxColor = green;
                else
                    hudItem->tag->boxColor = grey;
            }
        }
        else
        {
            if (data->io.axes.y)
                hudItem->tag->boxColor = grey;
            else
                hudItem->tag->boxColor = black;
        }

        hudItem = hudParent->child[kNPhudCoordZ];
        sprintf(hudItem->tag->title, "Coord Z:%7.2f", node->translate.z);
        npUpdateTextTag(hudItem->tag);
        if (axes.z)
        {
            if (!(axes.x && axes.y))
            {
                if (parentTopo != kNPtopoPin && parentTopo != kNPtopoRod
                    && !(node->branchLevel == 0 && axes.y))
                    hudItem->tag->boxColor = green;
                else
                    hudItem->tag->boxColor = grey;
            }
            else
                hudItem->tag->boxColor = grey;
        }
        else
        {
            if (data->io.mouse.buttonR
                && parentTopo != kNPtopoPin
                && parentTopo != kNPtopoRod
                && node->branchLevel != 0)
                hudItem->tag->boxColor = green;
            else
                hudItem->tag->boxColor = black;
        }
    }
    else
    {
        hudItem = hudParent->child[kNPhudAngle];
        if (hudItem->childIndex)					//clean this method up, zz debug
            sprintf(hudItem->tag->title, "Roll:   %7.2f", node->rotate.z);
        else
            sprintf(hudItem->tag->title, "Tilt:   %7.2f", node->rotate.x);
        npUpdateTextTag(hudItem->tag);
        hudItem->tag->boxColor = black;

        //	sprintf (coordX.title,	"LONG X:%7.2f", node->translate.x);	//zz debug, base on compass vs heading?
        //	sprintf (coordY.title,	"LAT  Y:%7.2f", node->translate.y);
        //	sprintf (coordZ.title,	"ALT  Z:%7.2f", node->translate.z);

        hudItem = hudParent->child[kNPhudCoordX];
        sprintf(hudItem->tag->title, "Coord X:%7.2f", node->translate.x);
        npUpdateTextTag(hudItem->tag);
        if (data->io.axes.x)
            hudItem->tag->boxColor = green;
        else
            hudItem->tag->boxColor = black;

        hudItem = hudParent->child[kNPhudCoordY];
        sprintf(hudItem->tag->title, "Coord Y:%7.2f", node->translate.y);
        npUpdateTextTag(hudItem->tag);
        if (data->io.axes.y)
            hudItem->tag->boxColor = green;
        else
            hudItem->tag->boxColor = black;

        hudItem = hudParent->child[kNPhudCoordZ];
        sprintf(hudItem->tag->title, "Coord Z:%7.2f", node->translate.z);
        npUpdateTextTag(hudItem->tag);
        if (data->io.axes.z)
            hudItem->tag->boxColor = green;
        else
            hudItem->tag->boxColor = black;
    }


    //update the mode outline color
    hudItem = hudParent->child[kNPhudMode];
    if (data->io.mouse.pickMode == kNPmodeCamera)
    {
        strcpy(hudItem->tag->title, "mode: Camera   "); //unify this with npPostMode, zz debug
        hudItem->color = red;
    }
    else if (data->io.mouse.pickMode == kNPmodeGrid)
    {
        strcpy(hudItem->tag->title, "mode: Grid     ");
        hudItem->color = yellow;
    }
    else // if (data->io.mouse.pickMode == kNPmodePin)
    {
        strcpy(hudItem->tag->title, "mode: Pin      ");
        hudItem->color = grey;
    }
    npUpdateTextTag(hudItem->tag);

    if (data->io.mouse.pickMode == kNPmodeCamera)
    {
        hudItem = hudParent->child[kNPhudTool];
        strcpy(hudItem->tag->title, "tool: Target   ");
        npUpdateTextTag(hudItem->tag);
    }

    //zzf //zz select
    hudItem = hudParent->child[kNPhudSelect];
    if (data->io.mouse.tool == kNPtoolSelect && data->io.mouse.pickMode == kNPmodePin)
        hudItem->color = yellow;
    else
        hudItem->color = grey;
    strcpy(hudItem->tag->title, "Select Region  ");
    npUpdateTextTag(hudItem->tag);

    if (1)//data->io.mouse.pickMode == kNPmodeCamera)
    {
        hudItem = hudParent->child[kNPhudSave];
        if (data->io.file.saveSelect)
        {
            hudItem->tag->boxColor.r = 255;
            strcpy(hudItem->tag->title, "Saving.....    ");
        }
        else
        {
            hudItem->tag->boxColor.r = 127;
            strcpy(hudItem->tag->title, "Save Selected  ");
        }
        npUpdateTextTag(hudItem->tag);
    }
    //zzf end

    //zz select
    hudItem = hudParent->child[kNPhudCreate];
    if (data->io.mouse.tool == kNPtoolCreate && data->io.mouse.pickMode == kNPmodePin)
        hudItem->color = yellow;
    else
        hudItem->color = grey;
    strcpy(hudItem->tag->title, "Create         ");
    npUpdateTextTag(hudItem->tag);

    hudItem = hudParent->child[kNPhudLink];
    if (data->io.mouse.tool == kNPtoolLink && data->io.mouse.pickMode == kNPmodePin)
        hudItem->color = yellow;
    else
        hudItem->color = grey;
    strcpy(hudItem->tag->title, "Link           ");
    npUpdateTextTag(hudItem->tag);

    hudItem = hudParent->child[kNPhudCombo];
    if (data->io.mouse.tool == kNPtoolCombo && data->io.mouse.pickMode == kNPmodePin)
        hudItem->color = yellow;
    else
        hudItem->color = grey;
    strcpy(hudItem->tag->title, "Combo          ");
    npUpdateTextTag(hudItem->tag);

    hudItem = hudParent->child[kNPhudTopo];
    if (data->io.mouse.tool == kNPtoolTopo && data->io.mouse.pickMode == kNPmodePin)
        hudItem->color = yellow;
    else
        hudItem->color = grey;
    strcpy(hudItem->tag->title, "Topo           ");
    npUpdateTextTag(hudItem->tag);

    hudItem = hudParent->child[kNPhudGeo];
    if (data->io.mouse.tool == kNPtoolGeometry && data->io.mouse.pickMode == kNPmodePin)
        hudItem->color = yellow;
    else
        hudItem->color = grey;
    strcpy(hudItem->tag->title, "Geometry       ");
    npUpdateTextTag(hudItem->tag);

    hudItem = hudParent->child[kNPhudColor];
    if (data->io.mouse.tool == kNPtoolColor && data->io.mouse.pickMode == kNPmodePin)
        hudItem->color = yellow;
    else
        hudItem->color = grey;
    strcpy(hudItem->tag->title, "Color          ");
    npUpdateTextTag(hudItem->tag);

    hudItem = hudParent->child[kNPhudTexture];
    if (data->io.mouse.tool == kNPtoolTexture && data->io.mouse.pickMode == kNPmodePin)
        hudItem->color = yellow;
    else
        hudItem->color = grey;
    strcpy(hudItem->tag->title, "Texture        ");
    npUpdateTextTag(hudItem->tag);

    hudItem = hudParent->child[kNPhudHide];
    if (data->io.mouse.tool == kNPtoolHide && data->io.mouse.pickMode == kNPmodePin)
        hudItem->color = yellow;
    else
        hudItem->color = grey;
    strcpy(hudItem->tag->title, "Hide           ");
    npUpdateTextTag(hudItem->tag);

    hudItem = hudParent->child[kNPhudInfo];
    if (data->io.mouse.tool == kNPtoolInfo && data->io.mouse.pickMode == kNPmodePin)
        hudItem->color = yellow;
    else
        hudItem->color = grey;
    strcpy(hudItem->tag->title, "Info           ");
    npUpdateTextTag(hudItem->tag);


    //draw indicators
    glPushMatrix();

    //update this to use hudRoot->tools position,						 zz degug
    //set initial position to upper left corner
    glTranslatef(data->io.gl.width * -0.5f + 5.0f,
        data->io.gl.height * 0.5f - 5.0f, 0.0f);

    // for (i=0; i < hudParent->childCount; i++)						//zz debug
    for (i = kNPhudCompass; i < kNPhudCount; i++)		//zzf
    {
        hudItem = hudParent->child[i];
        glTranslatef(0.0f, -21.0f, 0.0f);
        npDrawNodeTextTag(hudItem, data);
    }

    glPopMatrix();
}

void npPickGLUT(int x, int y, void* dataRef)
{
    int id = 0;
    char msg[256];

    GLubyte r, g, b;	//Unsigned Byte 8-bit Range: 0-255 // changed from u_int8_t, zz debug
    GLubyte pixels[4];		//4 Bytes

    pNPnode node = NULL;
    pData data = (pData)dataRef;

    if (data->io.mouse.buttonR
        && data->io.mouse.tool == kNPtoolLink
        && data->io.mouse.linkA != NULL)
    {
        data->io.mouse.linkA = NULL;
        npPostMsg("Cancelled, select a new Link A", kNPmsgCtrl, data);
        return;
    }

    //pickPass flag tells all object draw routines to use node ID mapped color
    data->io.gl.pickPass = true;

    //draw entire scene as flat 100% ambient with color mapped by node ID
    //think of it as a draw picture by numbers coloring book
    npGLDrawScene(data);			//optimize to draw only picked region, debug zz
    npGlutDrawHUD(data);        //make sure to draw HUD

    //upgrade to read a 9x9 region as to allow picking near an object, zz debug
    //then spiral out from the center looking for any object that is close
    //must handle edges of screen
    //read the pixel that was picked
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    r = pixels[0];
    g = pixels[1];
    b = pixels[2];			// 2^24 rgb limit, does not use alpha

    id = npRGBtoID(r, g, b);

    if (id >= kNPnodeMax)	//mouse selection crashing workaround, debug zz
        id = 0;				//add additional handling for bad picking IDs

    data->io.gl.pickID = id;

    if (id != 0)
    {
        node = data->map.nodeID[id];	//get node by id

        if (node != NULL)
        {
            if (node->type != kNodeHUD)
            {
                if (node != data->map.currentNode)
                {
                    //set active and selected node pointers unless in cam mode
                    npSelectNode(node, data);

                    //if recordID exists then display it, otherwise use node->id
                    if (node->recordID)
                        sprintf(msg, "picked id: %d   recordID: %d",
                        node->id, node->recordID);
                    else
                        sprintf(msg, "picked id: %d", node->id);
                    npPostMsg(msg, kNPmsgCtrl, data);
                }
                else
                    npSelectNode(node, data);	//zz debug, remove this and test to verify ok
            }
            //applies action to nodes including HUD items, pins, cam, etc
            npPickTool(node, data);
        }
        else
            data->io.gl.pickID = 0;	//if invalid node then set to default camera
    }
    else if (data->io.mouse.tool == kNPtoolCreate
        && !data->io.mouse.buttonR
        && data->io.mouse.pickMode != kNPmodeCamera)						 //zz-s debug, more testing!
    {
        data->io.mouse.createEvent = true; //used to create click, but not drag so cam works
    }

    data->io.gl.pickPass = false;	//clear the flag to draw normal colors
}