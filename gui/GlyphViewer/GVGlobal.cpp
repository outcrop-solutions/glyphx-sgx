#include "GVGlobal.h"
#include <QtCore/qglobal.h>
#include "AppGlobal.h"

GVServices* instance = nullptr;
void GVGlobal::Init(GVServices* s) {
	Q_ASSERT(!instance);
	instance = s;
	SynGlyphX::AppGlobal::Init(s);
}

void GVGlobal::Destroy() {
	if (instance) {
		delete instance;
		instance = nullptr;
	}	
}

GVServices* GVGlobal::Services() { return instance; }
