#include "minmaxglyphtree.h"

namespace SynGlyphX {

	MinMaxGlyphTree::MinMaxGlyphTree() : 
		stlplus::ntree< MinMaxGlyph >()
	{
	}

	MinMaxGlyphTree::MinMaxGlyphTree(const boost::property_tree::wptree& propertyTree) :
		stlplus::ntree< MinMaxGlyph >() {

	}

	MinMaxGlyphTree::~MinMaxGlyphTree()
	{
	}

	void MinMaxGlyphTree::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) {


	}

} //namespace SynGlyphX