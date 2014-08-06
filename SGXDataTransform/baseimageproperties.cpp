#include "baseimageproperties.h"

namespace SynGlyphX {

	BaseImageProperties::BaseImageProperties()
	{
	}

	BaseImageProperties::BaseImageProperties(const BaseImageProperties& properties) {


	}

	BaseImageProperties::~BaseImageProperties()
	{
	}

	bool BaseImageProperties::IsGeographic() const {

		return true;
	}

} //namespace SynGlyphX