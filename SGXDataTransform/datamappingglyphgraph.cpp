#include "datamappingglyphgraph.h"
#include <boost/property_tree/xml_parser.hpp>
#include <exception>
#include <stack>
#include <stdexcept>
#include "csvfilereader.h"
#include "datamappingglyphgraphexportvisitor.h"

namespace SynGlyphX {

	DataMappingGlyphGraph::DataMappingGlyphGraph() :
		stlplus::ntree<DataMappingGlyph>()
	{
	}

	DataMappingGlyphGraph::DataMappingGlyphGraph(const boost::property_tree::wptree& propertyTree) :
		stlplus::ntree<DataMappingGlyph>() {

		boost::optional<const PropertyTree&> inputFieldsPropertyTree = propertyTree.get_child_optional(L"InputFields");
		if (inputFieldsPropertyTree.is_initialized()) {

			for (const PropertyTree::value_type& inputfieldProperties : inputFieldsPropertyTree.get()) {

				if (inputfieldProperties.first == L"InputField") {

					InputField inputfield(inputfieldProperties.second);
					m_inputFields[inputfield.GetHashID()] = inputfield;
					m_inputFieldReferenceCounts[inputfield.GetHashID()] = 0;
				}
			}
		}

		//m_rootVertex = add_vertex(DataMappingGlyph(propertyTree));
		//IncrementInputBindingCountsFromGlyph(operator[](m_rootVertex));
		DataMappingGlyph rootGlyph(propertyTree);
		insert(rootGlyph);
		IncrementInputBindingCountsFromGlyph(*root());
		ProcessPropertyTreeChildren(root(), propertyTree);
	}

	DataMappingGlyphGraph::DataMappingGlyphGraph(const DataMappingGlyphGraph& graph) :
		stlplus::ntree<DataMappingGlyph>(graph),
		m_inputFields(graph.m_inputFields),
		m_inputFieldReferenceCounts(graph.m_inputFieldReferenceCounts) {


	}

	DataMappingGlyphGraph::DataMappingGlyphGraph(const GlyphGraph& graph) :
		stlplus::ntree<DataMappingGlyph>() {

		/*if (!graph.HasSingleRoot()) {

			throw std::invalid_argument("GlyphGraph must only have one root vertex.");
		}

		const GlyphGraph::Vertex& glyphGraphRoot = graph.GetRootVertices()[0];
		m_rootVertex = add_vertex(DataMappingGlyph(graph[glyphGraphRoot]));*/

		insert(DataMappingGlyph(*graph.root(), true));
		AddGraphGlyphSubgraph(root(), graph.root(), graph);
	}

	DataMappingGlyphGraph::~DataMappingGlyphGraph()
	{
	}
	
	bool DataMappingGlyphGraph::operator==(const DataMappingGlyphGraph& graph) const {
		
		if (m_inputFields != graph.m_inputFields) {

			return false;
		}

		if (empty() != graph.empty()) {

			return false;
		}

		if (empty() && graph.empty()) {

			return true;
		}

		return AreSubtreesEqual(root(), graph.root(), graph);
	}

	bool DataMappingGlyphGraph::AreSubtreesEqual(const DataMappingGlyphGraph::const_iterator& thisTreeNode, const DataMappingGlyphGraph::const_iterator& otherTreeNode, const DataMappingGlyphGraph& otherTree) const {

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

	bool DataMappingGlyphGraph::operator!=(const DataMappingGlyphGraph& tree) const {

		return !operator==(tree);
	}

	DataMappingGlyphGraph::PropertyTree& DataMappingGlyphGraph::ExportToPropertyTree(boost::property_tree::wptree& propertyTreeParent) const {

		boost::property_tree::wptree& rootPropertyTree = root()->ExportToPropertyTree(propertyTreeParent);
		ExportToPropertyTree(root(), rootPropertyTree);

		if (!m_inputFields.empty()) {

			boost::property_tree::wptree& inputFieldsPropertyTree = rootPropertyTree.add(L"InputFields", L"");

			for (auto inputfield : m_inputFields) {

				inputfield.second.ExportToPropertyTree(inputFieldsPropertyTree);
			}
		}

		return rootPropertyTree;
	}

	void DataMappingGlyphGraph::ExportToPropertyTree(const DataMappingGlyphGraph::const_iterator& parent, boost::property_tree::wptree& propertyTreeParent) const {


		/*std::pair<out_edge_iterator, out_edge_iterator> children = boost::out_edges(parent, *this);

		if (children.first != children.second) {

			boost::property_tree::wptree& childrenPropertyTree = propertyTreeParent.add(L"Children", L"");
			for (out_edge_iterator iT = children.first; iT != children.second; ++iT) {

				const Vertex& child = boost::target(*iT, *this);
				ExportToPropertyTree(child, operator[](child).ExportToPropertyTree(childrenPropertyTree));
			}
		}*/

		unsigned int numChildren = children(parent);
		if (numChildren > 0) {

			boost::property_tree::wptree& childrenPropertyTree = propertyTreeParent.add(L"Children", L"");
			for (int i = 0; i < numChildren; ++i) {

				DataMappingGlyphGraph::const_iterator iterator = child(parent, i);
				ExportToPropertyTree(iterator, iterator->ExportToPropertyTree(childrenPropertyTree));
			}
		}
	}

	/*
	const DataMappingGlyphGraph::Vertex& DataMappingGlyphGraph::GetRootVertex() const {

		return m_rootVertex;
	}*/

	void DataMappingGlyphGraph::ProcessPropertyTreeChildren(const DataMappingGlyphGraph::iterator& parent, const boost::property_tree::wptree& propertyTree) {

		boost::optional<const PropertyTree&> glyphTrees = propertyTree.get_child_optional(L"Children");

		if (glyphTrees.is_initialized()) {
		
			for (const PropertyTree::value_type& glyphTree : glyphTrees.get()) {

				if (glyphTree.first == L"Glyph") {

					//Vertex child = add_vertex(DataMappingGlyph(glyphTree.second));
					//IncrementInputBindingCountsFromGlyph(operator[](child));
					//add_edge(parent, child);
					//ProcessPropertyTreeChildren(child, glyphTree.second);

					DataMappingGlyph glyph(glyphTree.second);
					IncrementInputBindingCountsFromGlyph(glyph);
					ProcessPropertyTreeChildren(insert(parent, glyph), glyphTree.second);
				}
			}
		}
	}

	void DataMappingGlyphGraph::AddGraphGlyphSubgraph(DataMappingGlyphGraph::iterator& parent, const GlyphGraph::const_iterator& glyphGraphParent, const GlyphGraph& graph) {

		/*std::pair<GlyphGraph::out_edge_iterator, GlyphGraph::out_edge_iterator> children = boost::out_edges(glyphGraphParent, graph);
		for (GlyphGraph::out_edge_iterator iT = children.first; iT != children.second; ++iT) {

			const GlyphGraph::Vertex& glyphGraphChild = boost::target(*iT, graph);
			Vertex child = add_vertex(DataMappingGlyph(graph[glyphGraphChild]));
			add_edge(parent, child);
			AddGraphGlyphSubgraph(child, glyphGraphChild, graph);
		}*/

		for (int i = 0; i < graph.children(glyphGraphParent); ++i) {

			const GlyphGraph::const_iterator& child = graph.child(glyphGraphParent, i);
			DataMappingGlyph glyph(*child);
			AddGraphGlyphSubgraph(insert(parent, glyph), child, graph);
		}
	}

	void DataMappingGlyphGraph::SetInputField(DataMappingGlyphGraph::const_iterator& node, DataMappingGlyph::MappableField field, const InputField& inputfield) {

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

		node.deconstify()->SetInputBinding(field, InputBinding(inputFieldID));
	}

	void DataMappingGlyphGraph::ClearInputBinding(DataMappingGlyphGraph::const_iterator& node, DataMappingGlyph::MappableField field) {

		const InputBinding& binding = node->GetInputBinding(field);
		InputField::HashID inputFieldID = binding.GetInputFieldID();

		if (m_inputFieldReferenceCounts[inputFieldID] == 1) {

			m_inputFieldReferenceCounts.erase(m_inputFieldReferenceCounts.find(inputFieldID));
			m_inputFields.erase(m_inputFields.find(inputFieldID));
		}
		else {

			--m_inputFieldReferenceCounts[inputFieldID];
		}

		node.deconstify()->ClearInputBinding(field);
	}

	const DataMappingGlyphGraph::InputFieldMap& DataMappingGlyphGraph::GetInputFields() const {

		return m_inputFields;
	}

	bool DataMappingGlyphGraph::DoesRootGlyphPositionXYHaveBindings() const {

		return (root()->IsPositionXYBoundToInputFields());
	}

	GlyphGraph::SharedPtr DataMappingGlyphGraph::GetMinGlyphTree() const {

		//DataMappingGlyphGraphMinExportVisitor vis;
		//boost::depth_first_search(*this, boost::visitor(vis));
		//return vis.GetNewGraph();

		GlyphGraph::SharedPtr minGlyphTree = std::make_shared<GlyphGraph>();
		minGlyphTree->insert(root()->GetMinGlyph());

		CreateMinOrMaxGlyphSubtree(root(), minGlyphTree->root(), minGlyphTree, false);

		return minGlyphTree;
	}

	GlyphGraph::SharedPtr DataMappingGlyphGraph::GetMaxGlyphTree() const {

		//DataMappingGlyphGraphMaxExportVisitor vis;
		//boost::depth_first_search(*this, boost::visitor(vis));
		//return vis.GetNewGraph();

		GlyphGraph::SharedPtr maxGlyphTree = std::make_shared<GlyphGraph>();
		maxGlyphTree->insert(root()->GetMaxGlyph());

		CreateMinOrMaxGlyphSubtree(root(), maxGlyphTree->root(), maxGlyphTree, true);

		return maxGlyphTree;
	}

	void DataMappingGlyphGraph::CreateMinOrMaxGlyphSubtree(const DataMappingGlyphGraph::const_iterator& parent, GlyphGraph::iterator& newVertex, GlyphGraph::SharedPtr newGlyphGraph, bool isMax) const {

		for (int i = 0; i < children(parent); ++i) {

			const DataMappingGlyphGraph::const_iterator& childNode = child(parent, i);

			if (isMax) {

				CreateMinOrMaxGlyphSubtree(childNode, newGlyphGraph->insert(newVertex, childNode->GetMaxGlyph()), newGlyphGraph, true);
			}
			else {

				CreateMinOrMaxGlyphSubtree(childNode, newGlyphGraph->insert(newVertex, childNode->GetMinGlyph()), newGlyphGraph, false);
			}
		}
	}

	DataMappingGlyphGraph::SharedPtr DataMappingGlyphGraph::CreateDefault() {

		DataMappingGlyphGraph::SharedPtr newGraph = std::make_shared<DataMappingGlyphGraph>();

		DataMappingGlyphGraph::iterator root = newGraph->insert(DataMappingGlyph(Glyph::s_defaultRootGlyph));
		root->GetPosition()[0].GetValue() = std::pair<double, double>(-180.0, 360.0);
		root->GetPosition()[1].GetValue() = std::pair<double, double>(-90.0, 180.0);
		newGraph->insert(root, DataMappingGlyph(Glyph::s_defaultGlyph));

		return newGraph;
	}

	void DataMappingGlyphGraph::IncrementInputBindingCountsFromGlyph(const DataMappingGlyph& glyph) {

		if (!m_inputFields.empty()) {

			IncrementInputBindingCount(glyph.GetPosition()[0].GetBinding());
			IncrementInputBindingCount(glyph.GetPosition()[1].GetBinding());
			IncrementInputBindingCount(glyph.GetPosition()[2].GetBinding());

			IncrementInputBindingCount(glyph.GetRotation()[0].GetBinding());
			IncrementInputBindingCount(glyph.GetRotation()[1].GetBinding());
			IncrementInputBindingCount(glyph.GetRotation()[2].GetBinding());

			IncrementInputBindingCount(glyph.GetScale()[0].GetBinding());
			IncrementInputBindingCount(glyph.GetScale()[1].GetBinding());
			IncrementInputBindingCount(glyph.GetScale()[2].GetBinding());

			IncrementInputBindingCount(glyph.GetColor().GetBinding());
			IncrementInputBindingCount(glyph.GetTransparency().GetBinding());

			IncrementInputBindingCount(glyph.GetTag().GetBinding());
			IncrementInputBindingCount(glyph.GetDescription().GetBinding());

			IncrementInputBindingCount(glyph.GetRotationRate()[0].GetBinding());
			IncrementInputBindingCount(glyph.GetRotationRate()[1].GetBinding());
			IncrementInputBindingCount(glyph.GetRotationRate()[2].GetBinding());
		}
	}

	void DataMappingGlyphGraph::IncrementInputBindingCount(const InputBinding& binding) {

		if (binding.IsBoundToInputField()) {

			++m_inputFieldReferenceCounts[binding.GetInputFieldID()];
		}
	}

	void DataMappingGlyphGraph::ResetRootMinMaxPositionXY() {

		root()->GetPosition()[0].GetValue() = std::pair<double, double>(-180.0, 360.0);
		root()->GetPosition()[1].GetValue() = std::pair<double, double>(-90.0, 180.0);
	}

} //namespace SynGlyphX