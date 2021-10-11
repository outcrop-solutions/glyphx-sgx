#include "BaseImageProperties.h"
#include <boost/property_tree/ptree.hpp>

namespace GlyphEngine {

	BaseImageProperties::BaseImageProperties()
	{
	}

	BaseImageProperties::BaseImageProperties(const boost::property_tree::wptree& /*propertyTree*/) {


	}

	BaseImageProperties::BaseImageProperties(const BaseImageProperties& /*properties*/) {


	}

	BaseImageProperties::~BaseImageProperties()
	{
	}

	bool BaseImageProperties::IsGeographic() const {

		return false;
	}

} //namespace SynGlyphX