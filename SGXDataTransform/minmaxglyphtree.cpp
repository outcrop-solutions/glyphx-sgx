#include "minmaxglyphtree.h"
#include <boost/property_tree/xml_parser.hpp>
#include <exception>
#include <stack>

namespace SynGlyphX {

	MinMaxGlyphTree::MinMaxGlyphTree() : 
		stlplus::ntree< MinMaxGlyph >()
	{
	}

	MinMaxGlyphTree::MinMaxGlyphTree(const boost::property_tree::wptree& propertyTree) :
		stlplus::ntree< MinMaxGlyph >() {

		boost::optional<const PropertyTree&> glyphPropertyTree = propertyTree.get_child_optional(L"Glyph");
		if (glyphPropertyTree.is_initialized()) {
			ProcessPropertyTree(glyphPropertyTree.get());
		}
		else {
			throw std::exception("Property tree does not contain a glyph tree");
		}
	}

	MinMaxGlyphTree::~MinMaxGlyphTree()
	{
	}

	MinMaxGlyphTree::PropertyTree& MinMaxGlyphTree::ExportToPropertyTree(boost::property_tree::wptree& propertyTreeParent) const {

		std::stack<boost::property_tree::wptree*> parentStack;
		parentStack.push(&propertyTreeParent);

		for (MinMaxGlyphTree::const_prefix_iterator iT = prefix_begin(); iT != prefix_end(); ++iT) {

			MinMaxGlyphTree::const_iterator simpleIterator = iT.simplify();
			while (depth(simpleIterator) < (parentStack.size() - 1)) {
				parentStack.pop();
			}
			
			boost::property_tree::wptree& glyphPropertyTree = iT->ExportToPropertyTree(*parentStack.top());

			if (children(simpleIterator) > 0) {
				parentStack.push(&glyphPropertyTree.add(L"Children", L""));
			}
		}

		return propertyTreeParent.rbegin()->second;
	}

	void MinMaxGlyphTree::WriteToFile(const std::string& filename) const {

		boost::property_tree::wptree propertyTree;
		ExportToPropertyTree(propertyTree);
		boost::property_tree::write_xml(filename, propertyTree);
	}

	void MinMaxGlyphTree::ReadFromFile(const std::string& filename) {

		boost::property_tree::wptree propertyTree;
		boost::property_tree::read_xml(filename, propertyTree);
		boost::optional<PropertyTree&> glyphPropertyTree = propertyTree.get_child_optional(L"Glyph");
		if (glyphPropertyTree.is_initialized()) {
			ProcessPropertyTree(glyphPropertyTree.get());
		}
		else {
			throw std::exception((filename + " does not have glyph tree").c_str());
		}
	}

	void MinMaxGlyphTree::ProcessPropertyTree(const boost::property_tree::wptree& propertyTree) {

	}

} //namespace SynGlyphX