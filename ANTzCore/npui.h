#ifndef NPUI_H
#define NPUI_H

#include "ANTzCore.h"
#include "data/nptypes.h"

#ifdef __cplusplus
extern "C" {
#endif

void npInitKey(void* dataRef);
void npCloseKey(void* dataRef);

float npLowPassFilter(float start, float target, float fac);

ANTZCORE_API void npSetCamTarget(void* dataRef);
ANTZCORE_API void npSetCamTargetNode(pNPnode node, void* dataRef);

void npUpdateIO(void* dataRef);

void npUpdateMouse(void* dataRef);

#ifdef __cplusplus
}
#endif

#endif //NPUI_H