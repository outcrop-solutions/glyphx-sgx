#include "baseimageproperties.h"

namespace SynGlyphX {

	BaseImageProperties::BaseImageProperties()
	{
	}

	BaseImageProperties::BaseImageProperties(const boost::property_tree::wptree& propertyTree) {


	}

	BaseImageProperties::BaseImageProperties(const BaseImageProperties& properties) {


	}

	BaseImageProperties::~BaseImageProperties()
	{
	}

	bool BaseImageProperties::IsGeographic() const {

		return true;
	}

	void BaseImageProperties::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		//Nothing to do here right now since a default base image has no properties
	}

} //namespace SynGlyphX