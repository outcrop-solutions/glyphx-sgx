#include "datamappingglyphgraph.h"
#include <boost/property_tree/xml_parser.hpp>
#include <exception>
#include <stack>
#include <stdexcept>
#include "csvfilereader.h"
#include "datamappingglyphgraphexportvisitor.h"

namespace SynGlyphX {

	DataMappingGlyphGraph::DataMappingGlyphGraph() :
		GlyphGraphTemplate<DataMappingGlyph>()
	{
	}

	DataMappingGlyphGraph::DataMappingGlyphGraph(const boost::property_tree::wptree& propertyTree) :
		GlyphGraphTemplate<DataMappingGlyph>() {

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
		SetRootGlyph(rootGlyph, propertyTree.get_optional<Label>(L"<xmlattr>.label").get_value_or(GetNextLabel()));
		IncrementInputBindingCountsFromGlyph(GetRoot()->second);
		ProcessPropertyTreeChildren(GetRoot(), propertyTree);

		boost::optional<const PropertyTree&> linksPropertyTree = propertyTree.get_child_optional(L"Links");
		if (linksPropertyTree.is_initialized()) {

			for (const PropertyTree::value_type& linkProperties : linksPropertyTree.get()) {

				if (linkProperties.first == L"Glyph") {

					DataMappingGlyph linkGlyph(linkProperties.second);
					std::pair<Label, Label> linkLabels(linkProperties.second.get<Label>(L"<xmlattr>.source"), linkProperties.second.get<Label>(L"<xmlattr>.destination"));
					m_linkGlyphs[linkLabels] = linkGlyph;
				}
			}
		}
	}

	DataMappingGlyphGraph::DataMappingGlyphGraph(const DataMappingGlyphGraph& graph) :
		GlyphGraphTemplate<DataMappingGlyph>(graph),
		m_inputFields(graph.m_inputFields),
		m_inputFieldReferenceCounts(graph.m_inputFieldReferenceCounts) {


	}

	DataMappingGlyphGraph::DataMappingGlyphGraph(const GlyphGraph& graph) :
		GlyphGraphTemplate<DataMappingGlyph>() {

		/*if (!graph.HasSingleRoot()) {

			throw std::invalid_argument("GlyphGraph must only have one root vertex.");
		}

		const GlyphGraph::Vertex& glyphGraphRoot = graph.GetRootVertices()[0];
		m_rootVertex = add_vertex(DataMappingGlyph(graph[glyphGraphRoot]));*/

		SetRootGlyph(DataMappingGlyph(graph.GetRoot()->second, true));
		AddGraphGlyphSubgraph(root(), graph.GetRoot(), graph);

		for (auto& link : graph.GetLinks()) {

			AddLink(link.first.first, link.first.second, link.second);
		}
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

		return AreSubtreesEqual(GetRoot(), graph.GetRoot(), graph);
	}

	bool DataMappingGlyphGraph::AreSubtreesEqual(const DataMappingGlyphGraph::ConstGlyphIterator& thisTreeNode, const DataMappingGlyphGraph::ConstGlyphIterator& otherTreeNode, const DataMappingGlyphGraph& otherTree) const {

		if (thisTreeNode->first != otherTreeNode->first) {

			return false;
		}

		if (thisTreeNode->second.operator!=(otherTreeNode->second)) {

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

		boost::property_tree::wptree& rootPropertyTree = ExportSubgraphToPropertyTree(GetRoot(), propertyTreeParent);

		if (!m_linkGlyphs.empty()) {

			boost::property_tree::wptree& linksPropertyTree = rootPropertyTree.add(L"Links", L"");

			for (auto link : m_linkGlyphs) {

				boost::property_tree::wptree& linkPropertyTree = link.second.ExportToPropertyTree(linksPropertyTree);
				linkPropertyTree.put<Label>(L"<xmlattr>.source", link.first.first);
				linkPropertyTree.put<Label>(L"<xmlattr>.destination", link.first.second);
			}
		}

		if (!m_inputFields.empty()) {

			boost::property_tree::wptree& inputFieldsPropertyTree = rootPropertyTree.add(L"InputFields", L"");

			for (auto inputfield : m_inputFields) {

				inputfield.second.ExportToPropertyTree(inputFieldsPropertyTree);
			}
		}

		return rootPropertyTree;
	}

	DataMappingGlyphGraph::PropertyTree& DataMappingGlyphGraph::ExportSubgraphToPropertyTree(const DataMappingGlyphGraph::ConstGlyphIterator& vertex, boost::property_tree::wptree& propertyTreeParent) const {

		boost::property_tree::wptree& newPropertyTree = GetRoot()->second.ExportToPropertyTree(propertyTreeParent);
		newPropertyTree.put<Label>(L"<xmlattr>.label", GetRoot()->first);
		ExportChildrenToPropertyTree(GetRoot(), newPropertyTree);

		return newPropertyTree;
	}

	void DataMappingGlyphGraph::ExportChildrenToPropertyTree(const DataMappingGlyphGraph::ConstGlyphIterator& parent, boost::property_tree::wptree& propertyTreeParent) const {

		unsigned int numChildren = children(parent);
		if (numChildren > 0) {

			boost::property_tree::wptree& childrenPropertyTree = propertyTreeParent.add(L"Children", L"");
			for (int i = 0; i < numChildren; ++i) {

				DataMappingGlyphGraph::ConstGlyphIterator iterator = child(parent, i);
				boost::property_tree::wptree& newPropertyTree = iterator->second.ExportToPropertyTree(childrenPropertyTree);
				newPropertyTree.put<Label>(L"<xmlattr>.label", iterator->first);
				ExportChildrenToPropertyTree(iterator, newPropertyTree);
			}
		}
	}

	/*
	const DataMappingGlyphGraph::Vertex& DataMappingGlyphGraph::GetRootVertex() const {

		return m_rootVertex;
	}*/

	void DataMappingGlyphGraph::ProcessPropertyTreeChildren(const DataMappingGlyphGraph::GlyphIterator& parent, const boost::property_tree::wptree& propertyTree) {

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
					boost::optional<Label> label = glyphTree.second.get_optional<Label>(L"<xmlattr>.label");
					GlyphIterator newChild;
					if (label.is_initialized()) {

						newChild = AddChildGlyph(parent, glyph, label.get());
					}
					else {

						newChild = AddChildGlyph(parent, glyph, GetNextLabel());
					}
					ProcessPropertyTreeChildren(newChild, glyphTree.second);
				}
			}
		}
	}

	void DataMappingGlyphGraph::AddGraphGlyphSubgraph(DataMappingGlyphGraph::GlyphIterator& parent, const GlyphGraph::ConstGlyphIterator& glyphGraphParent, const GlyphGraph& graph) {

		/*std::pair<GlyphGraph::out_edge_iterator, GlyphGraph::out_edge_iterator> children = boost::out_edges(glyphGraphParent, graph);
		for (GlyphGraph::out_edge_iterator iT = children.first; iT != children.second; ++iT) {

			const GlyphGraph::Vertex& glyphGraphChild = boost::target(*iT, graph);
			Vertex child = add_vertex(DataMappingGlyph(graph[glyphGraphChild]));
			add_edge(parent, child);
			AddGraphGlyphSubgraph(child, glyphGraphChild, graph);
		}*/

		for (int i = 0; i < graph.ChildCount(glyphGraphParent); ++i) {

			const GlyphGraph::ConstGlyphIterator& child = graph.GetChild(glyphGraphParent, i);
			DataMappingGlyph glyph(child->second);
			AddGraphGlyphSubgraph(AddChildGlyph(parent, glyph, child->first), child, graph);
		}
	}

	void DataMappingGlyphGraph::SetInputField(DataMappingGlyphGraph::ConstGlyphIterator& node, DataMappingGlyph::MappableField field, const InputField& inputfield) {

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

		node.deconstify()->second.SetInputBinding(field, InputBinding(inputFieldID));
	}

	void DataMappingGlyphGraph::ClearInputBinding(DataMappingGlyphGraph::ConstGlyphIterator& node, DataMappingGlyph::MappableField field) {

		const InputBinding& binding = node->second.GetInputBinding(field);
		InputField::HashID inputFieldID = binding.GetInputFieldID();

		if (m_inputFieldReferenceCounts[inputFieldID] == 1) {

			m_inputFieldReferenceCounts.erase(m_inputFieldReferenceCounts.find(inputFieldID));
			m_inputFields.erase(m_inputFields.find(inputFieldID));
		}
		else {

			--m_inputFieldReferenceCounts[inputFieldID];
		}

		node.deconstify()->second.ClearInputBinding(field);
	}

	const DataMappingGlyphGraph::InputFieldMap& DataMappingGlyphGraph::GetInputFields() const {

		return m_inputFields;
	}

	bool DataMappingGlyphGraph::DoesRootGlyphPositionXYHaveBindings() const {

		return (GetRoot()->second.IsPositionXYBoundToInputFields());
	}

	GlyphGraph::SharedPtr DataMappingGlyphGraph::GetMinGlyphTree() const {

		//DataMappingGlyphGraphMinExportVisitor vis;
		//boost::depth_first_search(*this, boost::visitor(vis));
		//return vis.GetNewGraph();

		GlyphGraph::SharedPtr minGlyphTree = std::make_shared<GlyphGraph>();
		minGlyphTree->SetRootGlyph(GetRoot()->second.GetMinGlyph());

		CreateMinOrMaxGlyphSubtree(GetRoot(), minGlyphTree->GetRoot(), minGlyphTree, false);

		return minGlyphTree;
	}

	GlyphGraph::SharedPtr DataMappingGlyphGraph::GetMaxGlyphTree() const {

		//DataMappingGlyphGraphMaxExportVisitor vis;
		//boost::depth_first_search(*this, boost::visitor(vis));
		//return vis.GetNewGraph();

		GlyphGraph::SharedPtr maxGlyphTree = std::make_shared<GlyphGraph>();
		maxGlyphTree->SetRootGlyph(GetRoot()->second.GetMaxGlyph());

		CreateMinOrMaxGlyphSubtree(GetRoot(), maxGlyphTree->GetRoot(), maxGlyphTree, true);

		return maxGlyphTree;
	}

	void DataMappingGlyphGraph::CreateMinOrMaxGlyphSubtree(const DataMappingGlyphGraph::ConstGlyphIterator& parent, GlyphGraph::GlyphIterator& newVertex, GlyphGraph::SharedPtr newGlyphGraph, bool isMax) const {

		for (int i = 0; i < children(parent); ++i) {

			const DataMappingGlyphGraph::ConstGlyphIterator& childNode = child(parent, i);

			if (isMax) {

				CreateMinOrMaxGlyphSubtree(childNode, newGlyphGraph->AddChildGlyph(newVertex, childNode->second.GetMaxGlyph()), newGlyphGraph, true);
			}
			else {

				CreateMinOrMaxGlyphSubtree(childNode, newGlyphGraph->AddChildGlyph(newVertex, childNode->second.GetMinGlyph()), newGlyphGraph, false);
			}
		}
	}

	DataMappingGlyphGraph::SharedPtr DataMappingGlyphGraph::CreateDefault() {

		DataMappingGlyphGraph::SharedPtr newGraph = std::make_shared<DataMappingGlyphGraph>();

		DataMappingGlyphGraph::GlyphIterator root = newGraph->SetRootGlyph(DataMappingGlyph(Glyph::s_defaultRootGlyph));
		root->second.GetPosition()[0].GetValue() = DoubleMinDiff(-180.0, 360.0);
		root->second.GetPosition()[1].GetValue() = DoubleMinDiff(-90.0, 180.0);
		newGraph->AddChildGlyph(root, DataMappingGlyph(Glyph::s_defaultGlyph));

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

		GetRoot()->second.GetPosition()[0].GetValue() = DoubleMinDiff(-180.0, 360.0);
		GetRoot()->second.GetPosition()[1].GetValue() = DoubleMinDiff(-90.0, 180.0);
	}

} //namespace SynGlyphX