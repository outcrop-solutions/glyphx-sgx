#include "datamappingglyphgraph.h"
#include <boost/property_tree/xml_parser.hpp>
#include <exception>
#include <stack>
#include <stdexcept>
#include "csvfilereader.h"
#include "antzcsvwriter.h"

namespace SynGlyphX {

	DataMappingGlyphGraph::DataMappingGlyphGraph() :
		boost::directed_graph<DataMappingGlyph>()
	{
	}

	DataMappingGlyphGraph::DataMappingGlyphGraph(const boost::property_tree::wptree& propertyTree) :
		boost::directed_graph<DataMappingGlyph>() {

		m_rootVertex = add_vertex(DataMappingGlyph(propertyTree));
		ProcessPropertyTreeChildren(m_rootVertex, propertyTree);

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

	DataMappingGlyphGraph::DataMappingGlyphGraph(const DataMappingGlyphGraph& graph) :
		boost::directed_graph<DataMappingGlyph>(graph),
		m_inputFields(graph.m_inputFields),
		m_inputFieldReferenceCounts(graph.m_inputFieldReferenceCounts),
		m_rootVertex(graph.m_rootVertex) {


	}

	DataMappingGlyphGraph::DataMappingGlyphGraph(const GlyphGraph& graph) :
		boost::directed_graph<DataMappingGlyph>() {

		if (!graph.HasSingleRoot()) {

			throw std::invalid_argument("GlyphGraph must only have one root vertex.");
		}

		const GlyphGraph::Vertex& glyphGraphRoot = graph.GetRootVertices()[0];
		m_rootVertex = add_vertex(DataMappingGlyph(graph[glyphGraphRoot]));

		AddGraphGlyphSubgraph(m_rootVertex, glyphGraphRoot, graph);
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

		boost::property_tree::wptree& rootPropertyTree = operator[](m_rootVertex).ExportToPropertyTree(propertyTreeParent);
		ExportToPropertyTree(m_rootVertex, rootPropertyTree);

		if (!m_inputFields.empty()) {

			boost::property_tree::wptree& inputFieldsPropertyTree = rootPropertyTree.add(L"InputFields", L"");

			for (auto inputfield : m_inputFields) {

				inputfield.second.ExportToPropertyTree(inputFieldsPropertyTree);
			}
		}

		return rootPropertyTree;
	}

	void DataMappingGlyphGraph::ExportToPropertyTree(const Vertex& parent, boost::property_tree::wptree& propertyTreeParent) const {


		std::pair<out_edge_iterator, out_edge_iterator> children = boost::out_edges(parent, *this);

		if (children.first != children.second) {

			boost::property_tree::wptree& childrenPropertyTree = propertyTreeParent.add(L"Children", L"");
			for (out_edge_iterator iT = children.first; iT != children.second; ++iT) {

				const Vertex& child = boost::target(*iT, *this);
				ExportToPropertyTree(child, operator[](child).ExportToPropertyTree(childrenPropertyTree));
			}
		}
	}

	const DataMappingGlyphGraph::Vertex& DataMappingGlyphGraph::GetRootVertex() const {

		return m_rootVertex;
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

	void DataMappingGlyphGraph::ProcessPropertyTreeChildren(Vertex& parent, const boost::property_tree::wptree& propertyTree) {

		boost::optional<const PropertyTree&> glyphTrees = propertyTree.get_child_optional(L"Children");

		if (glyphTrees.is_initialized()) {
		
			for (const PropertyTree::value_type& glyphTree : glyphTrees.get()) {

				if (glyphTree.first == L"Glyph") {

					Vertex child = add_vertex(DataMappingGlyph(glyphTree.second));
					add_edge(parent, child);
					ProcessPropertyTreeChildren(child, glyphTree.second);
				}
			}
		}
	}

	void DataMappingGlyphGraph::AddGraphGlyphSubgraph(Vertex& parent, const GlyphGraph::Vertex& glyphGraphParent, const GlyphGraph& graph) {

		std::pair<GlyphGraph::out_edge_iterator, GlyphGraph::out_edge_iterator> children = boost::out_edges(glyphGraphParent, graph);
		for (GlyphGraph::out_edge_iterator iT = children.first; iT != children.second; ++iT) {

			const GlyphGraph::Vertex& glyphGraphChild = boost::target(*iT, graph);
			Vertex child = add_vertex(DataMappingGlyph(graph[glyphGraphChild]));
			add_edge(parent, child);
			AddGraphGlyphSubgraph(child, glyphGraphChild, graph);
		}
	}

	void DataMappingGlyphGraph::SetInputField(InputBinding& binding, const InputField& inputfield) {

		//Check if new input field is from same table as other input fields.  We shouldn't need to be this restrictive in the future, but that
		//requires more database work than we have time for right now.

		if (!m_inputFields.empty()) {
			if ((inputfield.GetDatasourceID() != m_inputFields.begin()->second.GetDatasourceID()) ||
				(inputfield.GetTable() != m_inputFields.begin()->second.GetTable())) {

				throw std::invalid_argument("Argument inputfield does not match the datasource and table of the other input fields of this tree");
			}
		}

		if (binding.IsBoundToInputField()) {

			ClearInputBinding(binding);
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

		binding = InputBinding(inputFieldID);
	}

	void DataMappingGlyphGraph::ClearInputBinding(InputBinding& binding) {

		InputField::HashID inputFieldID = binding.GetInputFieldID();

		if (m_inputFieldReferenceCounts[inputFieldID] == 1) {

			m_inputFieldReferenceCounts.erase(m_inputFieldReferenceCounts.find(inputFieldID));
			m_inputFields.erase(m_inputFields.find(inputFieldID));
		}
		else {

			--m_inputFieldReferenceCounts[inputFieldID];
		}

		binding.Clear();
	}

	const DataMappingGlyphGraph::InputFieldMap& DataMappingGlyphGraph::GetInputFields() const {

		return m_inputFields;
	}

	bool DataMappingGlyphGraph::DoesRootGlyphPositionXYHaveBindings() const {

		return (operator[](0).IsPositionXYBoundToInputFields());
	}

	GlyphGraph::SharedPtr DataMappingGlyphGraph::GetMinGlyphTree() const {

		GlyphGraph::SharedPtr minGlyphTree = std::make_shared<GlyphGraph>();
		CreateMinOrMaxGlyphSubtree(root(), minGlyphTree->root(), minGlyphTree);

		return minGlyphTree;
	}

	GlyphGraph::SharedPtr DataMappingGlyphGraph::GetMaxGlyphTree() const {

		GlyphGraph::SharedPtr maxGlyphTree = std::make_shared<GlyphGraph>();
		CreateMinOrMaxGlyphSubtree(root(), maxGlyphTree->root(), maxGlyphTree);

		return maxGlyphTree;
	}

	void DataMappingGlyphGraph::CreateMinOrMaxGlyphSubtree(const Vertex& parent, const GlyphGraph::Vertex& newVertex, GlyphGraph::SharedPtr newGlyphGraph, bool isMax) const {

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