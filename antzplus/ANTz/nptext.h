#ifndef NPTEXT_H
#define NPTEXT_H

#include "data/nptypes.h"

void npDrawNodeTextTag(pNPnode node, void* dataRef);
void npDrawTextTag(pNPtag tag, void* dataRef);
void npDrawNodeTags(void* dataRef);

void npDrawFPS(void* dataRef);

void npGlutDrawString(void *font, char *str);

void npDrawConsole(void* dataRef);

#endif