#ifndef NPGUI_H
#define NPGUI_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void npCtrlCommandGUI(int command, void* dataRef);

FILE* nposFileDialog(const char* fileName, int dialogType, void* dataRef);

int nposShowCursor(int hide);
int nposSetCursorPos(int x, int y);

FILE* npFileDialog(const char* fileName, int dialogType, void* dataRef);

void npFileBrowser(void* dataRef);

void npDrawCompass(void* dataRef);

void npPickGLUT(int x, int y, void* dataRef);

#ifdef __cplusplus
}
#endif

#endif