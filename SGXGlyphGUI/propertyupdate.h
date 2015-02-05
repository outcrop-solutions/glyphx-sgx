#ifndef SYNGLYPHX_GLYPHPROPERTYUPDATE_H
#define SYNGLYPHX_GLYPHPROPERTYUPDATE_H

#include "sgxglyphgui_global.h"

namespace SynGlyphX {

	enum PropertyUpdate {
		UpdateNone = 0x00,
		UpdateColor = 0x01,
		UpdateGeometry = 0x02,
		UpdateTopology = 0x04,
		UpdateSurface = 0x08,
		UpdatePosition = 0x10,
		UpdateRotation = 0x20,
		UpdateScale = 0x40,
		UpdateRotationRate = 0x80,
		UpdateTorusRatio = 0x100,
		//All for when all properties are being updated
		UpdateAll = 0xFFFF
	};

	Q_DECLARE_FLAGS(PropertyUpdates, PropertyUpdate);
	Q_DECLARE_OPERATORS_FOR_FLAGS(PropertyUpdates)

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPHPROPERTYUPDATE_H