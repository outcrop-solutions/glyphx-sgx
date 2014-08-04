#include "minmaxglyphtree.h"
#include <boost/property_tree/xml_parser.hpp>
#include <exception>
#include <stack>
#include <stdexcept>

namespace SynGlyphX {

	MinMaxGlyphTree::MinMaxGlyphTree() : 
		stlplus::ntree< MinMaxGlyph >()
	{
	}

	MinMaxGlyphTree::MinMaxGlyphTree(const boost::property_tree::wptree& propertyTree) :
		stlplus::ntree< MinMaxGlyph >() {

		MinMaxGlyph glyph(propertyTree);
		insert(glyph);
		ProcessPropertyTreeChildren(root(), propertyTree);

		boost::optional<const PropertyTree&> inputFieldsPropertyTree = propertyTree.get_child_optional(L"InputFields");
		if (inputFieldsPropertyTree.is_initialized()) {

			for (const PropertyTree::value_type& inputfieldProperties : inputFieldsPropertyTree.get()) {

				if (inputfieldProperties.first == L"InputField") {

					InputField inputfield(inputfieldProperties.second);
					m_inputFields[inputfield.GetHashID()] = inputfield;
				}
			}
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

		boost::property_tree::wptree& rootPropertyTree = propertyTreeParent.rbegin()->second;

		if (!m_inputFields.empty()) {

			boost::property_tree::wptree& inputFieldsPropertyTree = rootPropertyTree.add(L"InputFields", L"");

			for (auto inputfield : m_inputFields) {

				inputfield.second.ExportToPropertyTree(inputFieldsPropertyTree);
			}
		}

		return rootPropertyTree;
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

			boost::optional<PropertyTree&> inputFieldsPropertyTree = glyphPropertyTree.get().get_child_optional(L"InputFields");
			if (inputFieldsPropertyTree.is_initialized()) {

				for (const PropertyTree::value_type& inputfieldProperties : inputFieldsPropertyTree.get()) {

					if (inputfieldProperties.first == L"InputField") {

						InputField inputfield(inputfieldProperties.second);
						m_inputFields[inputfield.GetHashID()] = inputfield;
					}
				}
			}
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

	void MinMaxGlyphTree::SetInputField(MinMaxGlyphTree::const_iterator& node, unsigned int index, const InputField& inputfield, double min, double max) {

		//Check if new input field is from same table as other input fields.  We shouldn't need to be this restrictive in the future, but that
		//requires more database work than we have time for right now.

		if (!m_inputFields.empty()) {
			if ((inputfield.GetDatasourceID() != m_inputFields.begin()->second.GetDatasourceID()) ||
				(inputfield.GetTable() != m_inputFields.begin()->second.GetTable())) {

				throw std::invalid_argument("Argument inputfield does not match the datasource and table of the other input fields of this tree");
			}
		}

		InputField::HashID inputFieldID = inputfield.GetHashID();
		std::unordered_map<InputField::HashID, unsigned int>::iterator referenceCount = m_inputFieldReferenceCounts.find(inputFieldID);
		
		if (referenceCount == m_inputFieldReferenceCounts.end()) {

			m_inputFieldReferenceCounts[inputFieldID] = 1;
			m_inputFields[inputFieldID] = inputfield;
		}
		else {

			++m_inputFieldReferenceCounts[inputFieldID];
		}

		node.deconstify()->SetInputBinding(index, InputBinding(inputFieldID, min, max));
	}

	void MinMaxGlyphTree::ClearInputBinding(MinMaxGlyphTree::const_iterator& node, unsigned int index) {

		InputField::HashID inputFieldID = node->GetInputBinding(index).GetInputFieldID();

		if (m_inputFieldReferenceCounts[inputFieldID] == 1) {

			m_inputFieldReferenceCounts.erase(m_inputFieldReferenceCounts.find(inputFieldID));
			m_inputFields.erase(m_inputFields.find(inputFieldID));
		}
		else {

			--m_inputFieldReferenceCounts[inputFieldID];
		}

		node.deconstify()->ClearInputBinding(index);
	}

	const MinMaxGlyphTree::InputFieldMap& MinMaxGlyphTree::GetInputFields() const {

		return m_inputFields;
	}

} //namespace SynGlyphX