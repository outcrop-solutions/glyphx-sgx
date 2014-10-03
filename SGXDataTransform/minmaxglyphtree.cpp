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

	bool MinMaxGlyphTree::operator==(const MinMaxGlyphTree& tree) const {
		
		if (m_inputFields != tree.m_inputFields) {

			return false;
		}

		if (empty() != tree.empty()) {

			return false;
		}

		if (empty() && tree.empty()) {

			return true;
		}

		return AreSubtreesEqual(root(), tree.root(), tree);
	}

	bool MinMaxGlyphTree::AreSubtreesEqual(const MinMaxGlyphTree::const_iterator& thisTreeNode, const MinMaxGlyphTree::const_iterator& otherTreeNode, const MinMaxGlyphTree& otherTree) const {

		if (thisTreeNode->operator!=(*otherTreeNode)) {

			return false;
		}

		unsigned int numberOfChildren = children(thisTreeNode);
		if (otherTree.children(otherTreeNode) != numberOfChildren) {

			return false;
		}

		for (unsigned int i = 0; i < numberOfChildren; ++i) {

			if (!AreSubtreesEqual(child(thisTreeNode, i), otherTree.child(otherTreeNode, i), otherTree)) {

				return false;
			}
		}

		return true;
	}

	bool MinMaxGlyphTree::operator!=(const MinMaxGlyphTree& tree) const {

		return !operator==(tree);
	}

	MinMaxGlyphTree::PropertyTree& MinMaxGlyphTree::ExportToPropertyTree(boost::property_tree::wptree& propertyTreeParent) const {

		MinMaxGlyphTree::const_iterator iterator = root();
		boost::property_tree::wptree& rootPropertyTree = iterator->ExportToPropertyTree(propertyTreeParent);
		ExportToPropertyTree(iterator, rootPropertyTree);

		if (!m_inputFields.empty()) {

			boost::property_tree::wptree& inputFieldsPropertyTree = rootPropertyTree.add(L"InputFields", L"");

			for (auto inputfield : m_inputFields) {

				inputfield.second.ExportToPropertyTree(inputFieldsPropertyTree);
			}
		}

		return rootPropertyTree;
	}

	void MinMaxGlyphTree::ExportToPropertyTree(const MinMaxGlyphTree::const_iterator& parent, boost::property_tree::wptree& propertyTreeParent) const {

		unsigned int numChildren = children(parent);
		if (numChildren > 0) {

			boost::property_tree::wptree& childrenPropertyTree = propertyTreeParent.add(L"Children", L"");
			for (int i = 0; i < numChildren; ++i) {

				MinMaxGlyphTree::const_iterator iterator = child(parent, i);
				ExportToPropertyTree(iterator, iterator->ExportToPropertyTree(childrenPropertyTree));
			}
		}
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

	void MinMaxGlyphTree::SetInputField(MinMaxGlyphTree::const_iterator& node, unsigned int index, const InputField& inputfield) {

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

		node.deconstify()->SetInputBinding(index, InputBinding(inputFieldID));
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

	bool MinMaxGlyphTree::DoesRootGlyphPositionXYHaveBindings() const {

		return (root()->IsPositionXYBoundToInputFields());
	}

	GlyphTree::SharedPtr MinMaxGlyphTree::GetMinGlyphTree() const {

		GlyphTree::SharedPtr minGlyphTree(new GlyphTree());
		minGlyphTree->insert(root()->GetMinGlyph());

		CreateMinGlyphSubtree(root(), minGlyphTree->root(), minGlyphTree);

		return minGlyphTree;
	}

	void MinMaxGlyphTree::CreateMinGlyphSubtree(const MinMaxGlyphTree::const_iterator& parentNode, GlyphTree::iterator& newParent, GlyphTree::SharedPtr newGlyphTree) const {

		for (int i = 0; i < children(parentNode); ++i) {

			const MinMaxGlyphTree::const_iterator& childNode = child(parentNode, i);
			CreateMinGlyphSubtree(childNode, newGlyphTree->insert(newParent, childNode->GetMinGlyph()), newGlyphTree);
		}
	}

} //namespace SynGlyphX