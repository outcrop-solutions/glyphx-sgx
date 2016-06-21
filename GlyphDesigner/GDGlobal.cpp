#include "GDGlobal.h"
#include <QtCore/qglobal.h>
#include "AppGlobal.h"

GDServices* instance = nullptr;
void GDGlobal::Init(GDServices* s) {
	Q_ASSERT(!instance);
	instance = s;
	SynGlyphX::AppGlobal::Init(s);
}

void GDGlobal::Destroy() {
	if (instance) {
		delete instance;
		instance = nullptr;
	}	
}

GDServices* GDGlobal::Services() { return instance; }
