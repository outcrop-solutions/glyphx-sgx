#ifndef NPUI_H
#define NPUI_H

#include "ANTzCore.h"

void npInitKey(void* dataRef);
void npCloseKey(void* dataRef);

float npLowPassFilter(float start, float target, float fac);

ANTZCORE_API void npSetCamTarget(void* dataRef);

void npUpdateIO(void* dataRef);

void npUpdateMouse(void* dataRef);

#endif //NPUI_H