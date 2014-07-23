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

			MinMaxGlyph glyph(glyphPropertyTree.get());
			insert(glyph);
			ProcessPropertyTreeChildren(root(), glyphPropertyTree.get());
		}
		else {
			throw std::exception("Property tree does not contain a glyph tree");
		}
	}

	MinMaxGlyphTree::MinMaxGlyphTree(const GlyphTree& glyphTree) :
		stlplus::ntree< MinMaxGlyph >() {

		MinMaxGlyph glyph(*glyphTree.root());
		insert(glyph);

		AddGlyphSubtree(root(), glyphTree, glyphTree.root());
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

			MinMaxGlyph glyph(glyphPropertyTree.get());
			insert(glyph);
			ProcessPropertyTreeChildren(root(), glyphPropertyTree.get());
		}
		else {
			throw std::exception((filename + " does not have glyph tree").c_str());
		}
	}

	void MinMaxGlyphTree::ProcessPropertyTreeChildren(const MinMaxGlyphTree::iterator& iT, const boost::property_tree::wptree& propertyTree) {

		boost::optional<const PropertyTree&> glyphTrees = propertyTree.get_child_optional(L"Children");

		if (glyphTrees.is_initialized()) {
		
			for (const PropertyTree::value_type& glyphTree : glyphTrees.get()) {

				if (glyphTree.first == L"Glyph") {

					MinMaxGlyph glyph(glyphTree.second);
					ProcessPropertyTreeChildren(insert(iT, glyph), glyphTree.second);
				}
			}
		}
	}

	void MinMaxGlyphTree::AddGlyphSubtree(MinMaxGlyphTree::iterator& parentNode, const GlyphTree& glyphTree, const GlyphTree::const_iterator& iT) {

		for (int i = 0; i < glyphTree.children(iT); ++i) {

			const GlyphTree::const_iterator& child = glyphTree.child(iT, i);
			MinMaxGlyph glyph(*child);
			AddGlyphSubtree(insert(parentNode, glyph), glyphTree, child);
		}
	}

} //namespace SynGlyphX