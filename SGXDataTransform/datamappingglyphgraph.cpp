#include "datamappingglyphgraph.h"
#include <boost/property_tree/xml_parser.hpp>
#include <exception>
#include <stack>
#include <stdexcept>
#include "csvfilereader.h"
#include "antzcsvwriter.h"

namespace SynGlyphX {

	DataMappingGlyphGraph::DataMappingGlyphGraph() :
		boost::undirected_graph<DataMappingGlyph>()
	{
	}

	DataMappingGlyphGraph::DataMappingGlyphGraph(const boost::property_tree::wptree& propertyTree) :
		boost::undirected_graph<DataMappingGlyph>() {

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

	DataMappingGlyphGraph::DataMappingGlyphGraph(const GlyphGraph& graph) :
		boost::undirected_graph<DataMappingGlyph>() {

		MinMaxGlyph glyph(*glyphTree.root());
		insert(glyph);

		AddGlyphSubtree(root(), glyphTree, glyphTree.root());
	}

	DataMappingGlyphGraph::~DataMappingGlyphGraph()
	{
	}
	/*
	bool DataMappingGlyphGraph::operator==(const DataMappingGlyphGraph& graph) const {
		
		if (m_inputFields != graph.m_inputFields) {

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
	}*/

	DataMappingGlyphGraph::PropertyTree& DataMappingGlyphGraph::ExportToPropertyTree(boost::property_tree::wptree& propertyTreeParent) const {

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

	void DataMappingGlyphGraph::ExportToPropertyTree(const MinMaxGlyphTree::const_iterator& parent, boost::property_tree::wptree& propertyTreeParent) const {

		unsigned int numChildren = children(parent);
		if (numChildren > 0) {

			boost::property_tree::wptree& childrenPropertyTree = propertyTreeParent.add(L"Children", L"");
			for (int i = 0; i < numChildren; ++i) {

				MinMaxGlyphTree::const_iterator iterator = child(parent, i);
				ExportToPropertyTree(iterator, iterator->ExportToPropertyTree(childrenPropertyTree));
			}
		}
	}
	/*
	void MinMaxGlyphTree::WriteToFile(const std::string& filename) const {

		boost::property_tree::wptree propertyTree;
		ExportToPropertyTree(propertyTree);
		boost::property_tree::write_xml(filename, propertyTree);
	}

	void MinMaxGlyphTree::ReadFromFile(const std::string& filename) {

		std::string extension = filename.substr(filename.length() - 4, 4);
		if (extension == ".sgt") {

			ReadFromSGTFile(filename);
		}
		else if (extension == ".csv") {

			ReadFromANTzCSVFile(filename);
		}
		else {

			throw std::exception("File type not supported");
		}
	}

	void MinMaxGlyphTree::ReadFromSGTFile(const std::string& filename) {

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

	void MinMaxGlyphTree::ReadFromANTzCSVFile(const std::string& filename) {

		CSVFileReader csvReader(filename);

		if (csvReader.GetHeaders() != SynGlyphXANTz::ANTzCSVWriter::GetInstance().GetNodeHeaders()) {

			throw std::invalid_argument("Could not read CSV file into glyph tree: Invalid Headers");
		}

		CSVFileHandler::CSVValues currentLineValues;
		while (!csvReader.IsAtEndOfFile()) {

			currentLineValues = csvReader.GetValuesFromLine();
			if (currentLineValues[1] == L"5") {

				break;
			}
		}

		if ((currentLineValues.empty()) || (currentLineValues[1] != L"5")) {

			throw std::invalid_argument("CSV file has no glyph");
		}

		std::unordered_map<std::wstring, MinMaxGlyphTree::iterator> indexToNodeMap;

		indexToNodeMap[currentLineValues[0]] = insert(MinMaxGlyph(SynGlyphXANTz::GlyphProperties(currentLineValues)));
		currentLineValues = csvReader.GetValuesFromLine();

		do {
			if (currentLineValues[5] == L"0") {

				break;
			}

			if (currentLineValues[1] != L"5") {

				currentLineValues = csvReader.GetValuesFromLine();
				continue;
			}

			indexToNodeMap[currentLineValues[0]] = insert(indexToNodeMap[currentLineValues[4]], MinMaxGlyph(SynGlyphXANTz::GlyphProperties(currentLineValues)));
			currentLineValues = csvReader.GetValuesFromLine();

		} while (!csvReader.IsAtEndOfFile());
	}*/

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

	void MinMaxGlyphTree::AddGlyphSubtree(MinMaxGlyphTree::iterator& parentNode, const SynGlyphXANTz::GlyphTree& glyphTree, const SynGlyphXANTz::GlyphTree::const_iterator& iT) {

		for (int i = 0; i < glyphTree.children(iT); ++i) {

			const SynGlyphXANTz::GlyphTree::const_iterator& child = glyphTree.child(iT, i);
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

	SynGlyphXANTz::GlyphTree::SharedPtr MinMaxGlyphTree::GetMinGlyphTree() const {

		SynGlyphXANTz::GlyphTree::SharedPtr minGlyphTree(new SynGlyphXANTz::GlyphTree());
		minGlyphTree->insert(root()->GetMinGlyph());

		CreateMinGlyphSubtree(root(), minGlyphTree->root(), minGlyphTree);

		return minGlyphTree;
	}

	void MinMaxGlyphTree::CreateMinGlyphSubtree(const MinMaxGlyphTree::const_iterator& parentNode, SynGlyphXANTz::GlyphTree::iterator& newParent, SynGlyphXANTz::GlyphTree::SharedPtr newGlyphTree) const {

		for (int i = 0; i < children(parentNode); ++i) {

			const MinMaxGlyphTree::const_iterator& childNode = child(parentNode, i);
			CreateMinGlyphSubtree(childNode, newGlyphTree->insert(newParent, childNode->GetMinGlyph()), newGlyphTree);
		}
	}

	SynGlyphXANTz::GlyphTree::SharedPtr MinMaxGlyphTree::GetMaxGlyphTree() const {

		SynGlyphXANTz::GlyphTree::SharedPtr maxGlyphTree(new SynGlyphXANTz::GlyphTree());
		maxGlyphTree->insert(root()->GetMaxGlyph());

		CreateMaxGlyphSubtree(root(), maxGlyphTree->root(), maxGlyphTree);

		return maxGlyphTree;
	}

	void MinMaxGlyphTree::CreateMaxGlyphSubtree(const MinMaxGlyphTree::const_iterator& parentNode, SynGlyphXANTz::GlyphTree::iterator& newParent, SynGlyphXANTz::GlyphTree::SharedPtr newGlyphTree) const {

		for (int i = 0; i < children(parentNode); ++i) {

			const MinMaxGlyphTree::const_iterator& childNode = child(parentNode, i);
			CreateMaxGlyphSubtree(childNode, newGlyphTree->insert(newParent, childNode->GetMaxGlyph()), newGlyphTree);
		}
	}

	MinMaxGlyphTree::SharedPtr MinMaxGlyphTree::CreateDefault() {

		MinMaxGlyphTree::SharedPtr newTree(new MinMaxGlyphTree());
		MinMaxGlyphTree::iterator root = newTree->insert(MinMaxGlyph(*SynGlyphXANTz::GlyphProperties::GetRoot().get()));
		newTree->insert(root, MinMaxGlyph(*SynGlyphXANTz::GlyphProperties::GetTemplate().get()));
		return newTree;
	}

} //namespace SynGlyphX