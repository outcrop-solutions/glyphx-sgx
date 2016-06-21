#include "DMGlobal.h"
#include <QtCore/qglobal.h>
#include "AppGlobal.h"

DMServices* instance = nullptr;
void DMGlobal::Init(DMServices* s) {
	Q_ASSERT(!instance);
	instance = s;
	SynGlyphX::AppGlobal::Init(s);
}

void DMGlobal::Destroy() {
	if (instance) {
		delete instance;
		instance = nullptr;
	}
}

DMServices* DMGlobal::Services() { return instance; }
