#include "DataMappingGlyph.h"
#include <boost/property_tree/xml_parser.hpp>

namespace GlyphEngine {

	DataMappingGlyph::DataMappingGlyph(const boost::property_tree::wptree& propertyTree)
	{
		label = propertyTree.get_optional<int>(L"<xmlattr>.label").get_value_or(0);
		bool merge = propertyTree.get_optional<bool>(L"<xmlattr>.merge").get_value_or(false);

		const boost::property_tree::wptree& positionPropertyTree = propertyTree.get_child(L"Position");
		const boost::property_tree::wptree& scalePropertyTree = propertyTree.get_child(L"Scale");
		const boost::property_tree::wptree& colorPropertyTree = propertyTree.get_child(L"Color");
		const boost::property_tree::wptree& tagPropertyTree = propertyTree.get_child(L"Tag");
		const boost::property_tree::wptree& geometryPropertyTree = propertyTree.get_child(L"Geometry");

	}

} 