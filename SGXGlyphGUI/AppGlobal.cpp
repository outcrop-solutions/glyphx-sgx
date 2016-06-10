#include "AppGlobal.h"
#include "AppServices.h"
namespace SynGlyphX {
AppServices* instance = nullptr;
void AppGlobal::Init(AppServices* s) {
	Q_ASSERT(!instance);
	instance = s;
}
}