#include "datamappingglyphgraph.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <exception>
#include <stack>
#include <stdexcept>
#include "csvfilereader.h"
#include "datamappingglyphgraphexportvisitor.h"
#include "hashid.h"


namespace SynGlyphX {

	DataMappingGlyphGraph::DataMappingGlyphGraph() :
		GlyphGraphTemplate<DataMappingGlyph>(),
		m_mergeRoots(false)
	{
	}

	DataMappingGlyphGraph::DataMappingGlyphGraph(const boost::property_tree::wptree& propertyTree) :
		GlyphGraphTemplate<DataMappingGlyph>(),
		m_mergeRoots(false) {

		DataMappingGlyph rootGlyph(propertyTree);
		SetRootGlyph(rootGlyph, propertyTree.get_optional<Label>(L"<xmlattr>.label").get_value_or(GetNextLabel()));

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

		m_mergeRoots = propertyTree.get_optional<bool>(L"<xmlattr>.merge").get_value_or(false);

	}

	DataMappingGlyphGraph::DataMappingGlyphGraph(const DataMappingGlyphGraph& graph) :
		GlyphGraphTemplate<DataMappingGlyph>(graph),
		m_mergeRoots(graph.m_mergeRoots) {


	}

	DataMappingGlyphGraph::DataMappingGlyphGraph(const GlyphGraph& graph) :
		GlyphGraphTemplate<DataMappingGlyph>(),
		m_mergeRoots(false) {

		SetRootGlyph(DataMappingGlyph(graph.GetRoot()->second, true));
		AddGraphGlyphSubgraph(GetRoot(), graph.GetRoot(), graph);

		for (auto& link : graph.GetLinks()) {

			AddLink(link.first.first, link.first.second, link.second);
		}
	}

	DataMappingGlyphGraph::DataMappingGlyphGraph(const LabeledTree& graph) :
		GlyphGraphTemplate<DataMappingGlyph>(graph),
		m_mergeRoots(false) {


	}

	DataMappingGlyphGraph::~DataMappingGlyphGraph()
	{
	}
	
	bool DataMappingGlyphGraph::operator==(const DataMappingGlyphGraph& graph) const {
		
		if (m_mergeRoots != graph.m_mergeRoots) {

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

		boost::property_tree::wptree& rootPropertyTree = GetRoot()->second.ExportToPropertyTree(propertyTreeParent);
		rootPropertyTree.put<Label>(L"<xmlattr>.label", GetRoot()->first);
		ExportChildrenToPropertyTree(GetRoot(), rootPropertyTree);

		if (!m_linkGlyphs.empty()) {

			boost::property_tree::wptree& linksPropertyTree = rootPropertyTree.add(L"Links", L"");

			for (auto link : m_linkGlyphs) {

				boost::property_tree::wptree& linkPropertyTree = link.second.ExportToPropertyTree(linksPropertyTree);
				linkPropertyTree.put<Label>(L"<xmlattr>.source", link.first.first);
				linkPropertyTree.put<Label>(L"<xmlattr>.destination", link.first.second);
			}
		}

		rootPropertyTree.put<bool>(L"<xmlattr>.merge", m_mergeRoots);

		return rootPropertyTree;
	}

	void DataMappingGlyphGraph::ExportChildrenToPropertyTree(const DataMappingGlyphGraph::ConstGlyphIterator& parent, boost::property_tree::wptree& propertyTreeParent) const {

		unsigned int numChildren = children(parent);
		if (numChildren > 0) {

			boost::property_tree::wptree& childrenPropertyTree = propertyTreeParent.add(L"Children", L"");
			for (unsigned int i = 0; i < numChildren; ++i) {

				DataMappingGlyphGraph::ConstGlyphIterator iterator = child(parent, i);
				boost::property_tree::wptree& newPropertyTree = iterator->second.ExportToPropertyTree(childrenPropertyTree);
				newPropertyTree.put<Label>(L"<xmlattr>.label", iterator->first);
				ExportChildrenToPropertyTree(iterator, newPropertyTree);
			}
		}
	}

	DataMappingGlyphGraph DataMappingGlyphGraph::GetSubgraph(const GlyphIterator& vertex, bool includeChildren) {

		DataMappingGlyphGraph graph;

		if (includeChildren) {

			graph = GlyphGraphTemplate<DataMappingGlyph>::GetSubgraph(vertex);
		}
		else {

			graph.SetRootGlyph(vertex->second);
		}

		return graph;
	}

	DataMappingGlyphGraph DataMappingGlyphGraph::GetAndRemoveSubgraph(const GlyphIterator& vertex) {

		DataMappingGlyphGraph graph = GlyphGraphTemplate<DataMappingGlyph>::GetAndRemoveSubgraph(vertex);
		return graph;
	}

	void DataMappingGlyphGraph::ClearAllInputBindings() {

		ClearAllInputBindings(*this, GetRoot());
	}

	void DataMappingGlyphGraph::ResetAllFunctions() {
		
		ResetAllFunctions(*this, GetRoot());
	}

	void DataMappingGlyphGraph::ResetAllFunctions(DataMappingGlyphGraph& graph, const GlyphIterator& vertex) {
		auto node = vertex.constify();
		node.deconstify()->second.ResetAllFunctions();
		for (unsigned int i = 0; i < graph.children(vertex); ++i) {
			ResetAllFunctions(graph, graph.child(vertex, i));
		}
	}

	void DataMappingGlyphGraph::ClearAllInputBindings(DataMappingGlyphGraph& graph, const GlyphIterator& vertex) {
        auto cvertex = vertex.constify();
		ClearAllInputBindings(cvertex);
		for (unsigned int i = 0; i < graph.children(vertex); ++i) {

			ClearAllInputBindings(graph, graph.child(vertex, i));
		}
	}

	void DataMappingGlyphGraph::ClearFieldGroup(const std::wstring& fieldGroupName) {

		ClearFieldGroup(fieldGroupName, GetRoot());
	}

	void DataMappingGlyphGraph::ClearFieldGroup(const std::wstring& fieldGroupName, const GlyphIterator& vertex) {

		vertex->second.ClearFieldGroup(fieldGroupName);
		for (unsigned int i = 0; i < this->children(vertex); ++i) {

			ClearFieldGroup(fieldGroupName, GetChild(vertex, i));
		}
	}

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
					//IncrementInputBindingCountsFromGlyph(glyph);
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

	void DataMappingGlyphGraph::AddGraphGlyphSubgraph(DataMappingGlyphGraph::GlyphIterator parent, GlyphGraph::ConstGlyphIterator glyphGraphParent, const GlyphGraph& graph) {

		/*std::pair<GlyphGraph::out_edge_iterator, GlyphGraph::out_edge_iterator> children = boost::out_edges(glyphGraphParent, graph);
		for (GlyphGraph::out_edge_iterator iT = children.first; iT != children.second; ++iT) {

			const GlyphGraph::Vertex& glyphGraphChild = boost::target(*iT, graph);
			Vertex child = add_vertex(DataMappingGlyph(graph[glyphGraphChild]));
			add_edge(parent, child);
			AddGraphGlyphSubgraph(child, glyphGraphChild, graph);
		}*/

		for (unsigned int i = 0; i < graph.ChildCount(glyphGraphParent); ++i) {

			const GlyphGraph::ConstGlyphIterator& child = graph.GetChild(glyphGraphParent, i);
			DataMappingGlyph glyph(child->second);
			AddGraphGlyphSubgraph(AddChildGlyph(parent, glyph, child->first), child, graph);
		}
	}

	void DataMappingGlyphGraph::SetInputField(DataMappingGlyphGraph::ConstGlyphIterator node, DataMappingGlyph::MappableField field, const std::wstring& inputfield) {

		//Checking if new input field is from same table as other input fields is done before calling this
	
		ClearInputBinding(node, field);

		node.deconstify()->second.SetInputBinding(field, InputBinding(inputfield));
	}

	void DataMappingGlyphGraph::ClearInputBinding(DataMappingGlyphGraph::ConstGlyphIterator& node, DataMappingGlyph::MappableField field) {

		const InputBinding& binding = node->second.GetInputBinding(field);

		if (binding.IsBoundToInputField()) {

			node.deconstify()->second.ClearInputBinding(field);
		}
	}

	void DataMappingGlyphGraph::ClearAllInputBindings(DataMappingGlyphGraph::ConstGlyphIterator& node) {

		for (int i = 0; i < DataMappingGlyph::MappableField::MappableFieldSize; ++i) {

			ClearInputBinding(node, static_cast<SynGlyphX::DataMappingGlyph::MappableField>(i));
		}
	}

	void DataMappingGlyphGraph::ClearInputFieldBindings(const std::wstring& inputfield) {
		
		ClearInputFieldBindings(*this, GetRoot(), inputfield);		
	}

	void DataMappingGlyphGraph::ClearInputFieldBindings(DataMappingGlyphGraph& graph, const GlyphIterator& vertex, const std::wstring& inputfield) {

		//ClearAllInputBindings(vertex.constify());
		auto node = vertex.constify();
		for (int i = 0; i < DataMappingGlyph::MappableField::MappableFieldSize; ++i) {

			DataMappingGlyph::MappableField field = static_cast<SynGlyphX::DataMappingGlyph::MappableField>(i);
			const InputBinding& binding = node->second.GetInputBinding(field);
			if (binding.IsBoundToInputField() && (binding.GetInputFieldID() == inputfield)) {
				
				ClearInputBinding(node, field);
			}

		}

		for (unsigned int i = 0; i < graph.children(vertex); ++i) {

			ClearInputFieldBindings(graph, graph.child(vertex, i), inputfield);
		}
	}

	bool DataMappingGlyphGraph::DoesRootGlyphPositionXYHaveBindings() const {

		return (GetRoot()->second.IsPositionXYBoundToInputFields());
	}

	GlyphGraph::SharedPtr DataMappingGlyphGraph::GetMinGlyphTree() const {

		return CreateMinOrMaxGlyphSubtree(CreateGlyphTreeType::MIN);
	}

	GlyphGraph::SharedPtr DataMappingGlyphGraph::GetMaxGlyphTree() const {

		return CreateMinOrMaxGlyphSubtree(CreateGlyphTreeType::MAX);
	}

	GlyphGraph::SharedPtr DataMappingGlyphGraph::GetLegendGlyphTree() const {

		return CreateMinOrMaxGlyphSubtree(CreateGlyphTreeType::LEGEND);
	}

	GlyphGraph::SharedPtr DataMappingGlyphGraph::CreateMinOrMaxGlyphSubtree(CreateGlyphTreeType type) const {

		//DataMappingGlyphGraphMaxExportVisitor vis;
		//boost::depth_first_search(*this, boost::visitor(vis));
		//return vis.GetNewGraph();

		GlyphGraph::SharedPtr glyphTree = std::make_shared<GlyphGraph>();

		if (type == CreateGlyphTreeType::MAX) {
			glyphTree->SetRootGlyph(GetRoot()->second.GetMaxGlyph());
		}
		else if (type == CreateGlyphTreeType::MIN) {
			glyphTree->SetRootGlyph(GetRoot()->second.GetMinGlyph());
		}
		else {
			glyphTree->SetRootGlyph(GetRoot()->second.GetLegendGlyph());
		}

		CreateMinOrMaxGlyphSubtree(GetRoot(), glyphTree->GetRoot(), glyphTree, type);

		if (type == CreateGlyphTreeType::MAX) {
			for (const auto& link : m_linkGlyphs) {
				glyphTree->AddLink(link.first.first, link.first.second, link.second.GetMaxGlyph());
			}
		}
		else if (type == CreateGlyphTreeType::MIN) {
			for (const auto& link : m_linkGlyphs) {
				glyphTree->AddLink(link.first.first, link.first.second, link.second.GetMinGlyph());
			}
		}
		else {
			for (const auto& link : m_linkGlyphs) {
				glyphTree->AddLink(link.first.first, link.first.second, link.second.GetLegendGlyph());
			}
		}

		return glyphTree;
	}

	void DataMappingGlyphGraph::CreateMinOrMaxGlyphSubtree(const DataMappingGlyphGraph::ConstGlyphIterator parent, GlyphGraph::GlyphIterator newVertex, GlyphGraph::SharedPtr newGlyphGraph, CreateGlyphTreeType type) const {

		for (unsigned int i = 0; i < children(parent); ++i) {

			const DataMappingGlyphGraph::ConstGlyphIterator& childNode = child(parent, i);

			GlyphGraph::GlyphIterator newChild;
			if (type == CreateGlyphTreeType::MAX) {
				newChild = newGlyphGraph->AddChildGlyph(newVertex, childNode->second.GetMaxGlyph());
			}
			else if (type == CreateGlyphTreeType::MIN) {
				newChild = newGlyphGraph->AddChildGlyph(newVertex, childNode->second.GetMinGlyph());
			}
			else {
				newChild = newGlyphGraph->AddChildGlyph(newVertex, childNode->second.GetLegendGlyph());
			}

			//Keep labels the same
			newChild->first = childNode->first;

			CreateMinOrMaxGlyphSubtree(childNode, newChild, newGlyphGraph, type);
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



	void DataMappingGlyphGraph::ResetRootMinMaxPositionXY() {

		GetRoot()->second.GetPosition()[0].GetValue() = DoubleMinDiff(-180.0, 360.0);
		GetRoot()->second.GetPosition()[1].GetValue() = DoubleMinDiff(-90.0, 180.0);
	}


	bool DataMappingGlyphGraph::IsTransformable() const {

		return ((GetRoot()->second.IsAnInputFieldBoundToAPosition()));
	}

	void DataMappingGlyphGraph::SetMergeRoots(bool mergeRoots) {

		m_mergeRoots = mergeRoots;
	}

	bool DataMappingGlyphGraph::GetMergeRoots() const {

		return m_mergeRoots;
	}

} //namespace SynGlyphX
