#include "datamappingglyphgraph.h"
#include <boost/property_tree/xml_parser.hpp>
#include <exception>
#include <stack>
#include <stdexcept>
#include "csvfilereader.h"
#include "datamappingglyphgraphexportvisitor.h"

namespace SynGlyphX {

	DataMappingGlyphGraph::DataMappingGlyphGraph() :
		GlyphGraphTemplate<DataMappingGlyph>(),
		m_mergeRoots(false)
	{
	}

	DataMappingGlyphGraph::DataMappingGlyphGraph(const boost::property_tree::wptree& propertyTree) :
		GlyphGraphTemplate<DataMappingGlyph>(),
		m_mergeRoots(false) {

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

		m_mergeRoots = propertyTree.get_optional<bool>(L"<xmlattr>.merge").get_value_or(false);

		//Since there was a previous bug where input fields would get written to the file that were not in use, clear them out
		std::vector<SynGlyphX::InputTable::HashID> fieldsToRemove;
		for (auto inputField : m_inputFieldReferenceCounts) {

			if (inputField.second == 0) {

				fieldsToRemove.push_back(inputField.first);
			}
		}
		for (auto fieldToRemove : fieldsToRemove) {

			m_inputFields.erase(fieldToRemove);
			m_inputFieldReferenceCounts.erase(fieldToRemove);
		}
	}

	DataMappingGlyphGraph::DataMappingGlyphGraph(const DataMappingGlyphGraph& graph) :
		GlyphGraphTemplate<DataMappingGlyph>(graph),
		m_inputFields(graph.m_inputFields),
		m_inputFieldReferenceCounts(graph.m_inputFieldReferenceCounts),
		m_mergeRoots(graph.m_mergeRoots) {


	}

	DataMappingGlyphGraph::DataMappingGlyphGraph(const GlyphGraph& graph) :
		GlyphGraphTemplate<DataMappingGlyph>(),
		m_mergeRoots(false) {

		/*if (!graph.HasSingleRoot()) {

			throw std::invalid_argument("GlyphGraph must only have one root vertex.");
		}

		const GlyphGraph::Vertex& glyphGraphRoot = graph.GetRootVertices()[0];
		m_rootVertex = add_vertex(DataMappingGlyph(graph[glyphGraphRoot]));*/

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

		if (!m_inputFields.empty()) {

			boost::property_tree::wptree& inputFieldsPropertyTree = rootPropertyTree.add(L"InputFields", L"");

			for (auto inputfield : m_inputFields) {

				inputfield.second.ExportToPropertyTree(inputFieldsPropertyTree);
			}
		}

		rootPropertyTree.put<bool>(L"<xmlattr>.merge", m_mergeRoots);

		return rootPropertyTree;
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

	DataMappingGlyphGraph DataMappingGlyphGraph::GetSubgraph(const GlyphIterator& vertex, bool includeChildren) {

		DataMappingGlyphGraph graph;

		if (includeChildren) {

			graph = GlyphGraphTemplate<DataMappingGlyph>::GetSubgraph(vertex);
		}
		else {

			graph.SetRootGlyph(vertex->second);
		}

		AddAllInputBindingsToSubgraph(graph, graph.GetRoot(), false);
		return graph;
	}

	DataMappingGlyphGraph DataMappingGlyphGraph::GetAndRemoveSubgraph(const GlyphIterator& vertex) {

		DataMappingGlyphGraph graph = GlyphGraphTemplate<DataMappingGlyph>::GetAndRemoveSubgraph(vertex);
		AddAllInputBindingsToSubgraph(graph, graph.GetRoot(), true);
		return graph;
	}

	void DataMappingGlyphGraph::AddAllInputBindingsToSubgraph(DataMappingGlyphGraph& graph, const GlyphIterator& vertex, bool removeFromThisGraph) {

		for (unsigned int field = 0; field < DataMappingGlyph::MappableField::MappableFieldSize; ++field) {

			InputBinding& selectedBinding = const_cast<InputBinding&>(vertex->second.GetInputBinding(static_cast<DataMappingGlyph::MappableField>(field)));
			if (selectedBinding.IsBoundToInputField()) {

				InputField::HashID inputFieldID = selectedBinding.GetInputFieldID();
				std::unordered_map<InputField::HashID, unsigned int>::iterator referenceCount = graph.m_inputFieldReferenceCounts.find(inputFieldID);

				if (referenceCount == graph.m_inputFieldReferenceCounts.end()) {

					graph.m_inputFieldReferenceCounts[inputFieldID] = 1;
					graph.m_inputFields[inputFieldID] = m_inputFields[inputFieldID];
				}
				else {

					++graph.m_inputFieldReferenceCounts[inputFieldID];
				}

				if (removeFromThisGraph) {

					if (m_inputFieldReferenceCounts[inputFieldID] == 1) {

						m_inputFieldReferenceCounts.erase(m_inputFieldReferenceCounts.find(inputFieldID));
						m_inputFields.erase(m_inputFields.find(inputFieldID));
					}
					else {

						--m_inputFieldReferenceCounts[inputFieldID];
					}
				}
			}
		}

		for (unsigned int i = 0; i < graph.children(vertex); ++i) {

			AddAllInputBindingsToSubgraph(graph, graph.child(vertex, i), removeFromThisGraph);
		}
	}

	void DataMappingGlyphGraph::ClearAllInputBindings() {

		ClearAllInputBindings(*this, GetRoot());
	}

	void DataMappingGlyphGraph::ClearAllInputBindings(DataMappingGlyphGraph& graph, const GlyphIterator& vertex) {

		ClearAllInputBindings(vertex.constify());
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

	void DataMappingGlyphGraph::AddGraphGlyphSubgraph(DataMappingGlyphGraph::GlyphIterator parent, GlyphGraph::ConstGlyphIterator glyphGraphParent, const GlyphGraph& graph) {

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

	void DataMappingGlyphGraph::SetInputField(DataMappingGlyphGraph::ConstGlyphIterator node, DataMappingGlyph::MappableField field, const InputField& inputfield) {

		//Check if new input field is from same table as other input fields.  We shouldn't need to be this restrictive in the future, but that
		//requires more database work than we have time for right now.

		if (!m_inputFields.empty()) {
			if ((inputfield.GetDatasourceID() != m_inputFields.begin()->second.GetDatasourceID()) ||
				(inputfield.GetTable() != m_inputFields.begin()->second.GetTable())) {

				throw std::invalid_argument("Argument inputfield does not match the datasource and table of the other input fields of this tree");
			}
		}

		//Clear input binding if field as a previous value
		ClearInputBinding(node, field);

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

		if (binding.IsBoundToInputField()) {

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
	}

	void DataMappingGlyphGraph::ClearAllInputBindings(DataMappingGlyphGraph::ConstGlyphIterator& node) {

		for (int i = 0; i < DataMappingGlyph::MappableField::MappableFieldSize; ++i) {

			ClearInputBinding(node, static_cast<SynGlyphX::DataMappingGlyph::MappableField>(i));
		}
	}

	void DataMappingGlyphGraph::ClearInputFieldBindings(const InputField& inputfield) {
		
		ClearInputFieldBindings(*this, GetRoot(), inputfield);		
	}

	void DataMappingGlyphGraph::ClearInputFieldBindings(DataMappingGlyphGraph& graph, const GlyphIterator& vertex, const InputField& inputfield) {

		//ClearAllInputBindings(vertex.constify());
		DataMappingGlyphGraph::ConstGlyphIterator& node = vertex.constify();
		for (int i = 0; i < DataMappingGlyph::MappableField::MappableFieldSize; ++i) {

			DataMappingGlyph::MappableField field = static_cast<SynGlyphX::DataMappingGlyph::MappableField>(i);
			const InputBinding& binding = node->second.GetInputBinding(field);
			if (binding.IsBoundToInputField() && (binding.GetInputFieldID() == inputfield.GetHashID())) {
				
				ClearInputBinding(node, field);
			}

		}

		for (unsigned int i = 0; i < graph.children(vertex); ++i) {

			ClearInputFieldBindings(graph, graph.child(vertex, i), inputfield);
		}
	}

	const DataMappingGlyphGraph::InputFieldMap& DataMappingGlyphGraph::GetInputFields() const {

		return m_inputFields;
	}

	bool DataMappingGlyphGraph::DoesRootGlyphPositionXYHaveBindings() const {

		return (GetRoot()->second.IsPositionXYBoundToInputFields());
	}

	GlyphGraph::SharedPtr DataMappingGlyphGraph::GetMinGlyphTree() const {

		return CreateMinOrMaxGlyphSubtree(false);
	}

	GlyphGraph::SharedPtr DataMappingGlyphGraph::GetMaxGlyphTree() const {

		return CreateMinOrMaxGlyphSubtree(true);
	}

	GlyphGraph::SharedPtr DataMappingGlyphGraph::CreateMinOrMaxGlyphSubtree(bool isMax) const {

		//DataMappingGlyphGraphMaxExportVisitor vis;
		//boost::depth_first_search(*this, boost::visitor(vis));
		//return vis.GetNewGraph();

		GlyphGraph::SharedPtr glyphTree = std::make_shared<GlyphGraph>();

		if (isMax) {

			glyphTree->SetRootGlyph(GetRoot()->second.GetMaxGlyph());
		}
		else {

			glyphTree->SetRootGlyph(GetRoot()->second.GetMinGlyph());
		}

		CreateMinOrMaxGlyphSubtree(GetRoot(), glyphTree->GetRoot(), glyphTree, isMax);

		if (isMax) {

			for (const auto& link : m_linkGlyphs) {

				glyphTree->AddLink(link.first.first, link.first.second, link.second.GetMaxGlyph());
			}
		}
		else {

			for (const auto& link : m_linkGlyphs) {

				glyphTree->AddLink(link.first.first, link.first.second, link.second.GetMinGlyph());
			}
		}

		return glyphTree;
	}

	void DataMappingGlyphGraph::CreateMinOrMaxGlyphSubtree(const DataMappingGlyphGraph::ConstGlyphIterator parent, GlyphGraph::GlyphIterator newVertex, GlyphGraph::SharedPtr newGlyphGraph, bool isMax) const {

		for (int i = 0; i < children(parent); ++i) {

			const DataMappingGlyphGraph::ConstGlyphIterator& childNode = child(parent, i);

			GlyphGraph::GlyphIterator newChild;
			if (isMax) {

				newChild = newGlyphGraph->AddChildGlyph(newVertex, childNode->second.GetMaxGlyph());
			}
			else {

				newChild = newGlyphGraph->AddChildGlyph(newVertex, childNode->second.GetMinGlyph());
			}

			//Keep labels the same
			newChild->first = childNode->first;

			CreateMinOrMaxGlyphSubtree(childNode, newChild, newGlyphGraph, isMax);
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

			for (unsigned int field = 0; field < DataMappingGlyph::MappableField::MappableFieldSize; ++field) {

				IncrementInputBindingCount(glyph.GetInputBinding(static_cast<DataMappingGlyph::MappableField>(field)));
			}
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

	DataMappingGlyphGraph::GlyphIterator DataMappingGlyphGraph::AddChildGlyphGraph(const GlyphIterator& vertex, const DataMappingGlyphGraph& graph) {

		if (!m_inputFields.empty() && !graph.m_inputFields.empty()) {
			if ((graph.m_inputFields.begin()->second.GetDatasourceID() != m_inputFields.begin()->second.GetDatasourceID()) ||
				(graph.m_inputFields.begin()->second.GetTable() != m_inputFields.begin()->second.GetTable())) {

				DataMappingGlyphGraph graphWithoutInputBindings = graph;
				graphWithoutInputBindings.ClearAllInputBindings();
				return GlyphGraphTemplate<DataMappingGlyph>::AddChildGlyphGraph(vertex, graphWithoutInputBindings);
			}
		}

		for (auto inputField : graph.m_inputFields) {

			if (m_inputFields.count(inputField.first) == 0) {

				m_inputFields[inputField.first] = inputField.second;
			}
		}

		for (auto inputFieldCount : graph.m_inputFieldReferenceCounts) {

			if (m_inputFieldReferenceCounts.count(inputFieldCount.first) == 0) {

				m_inputFieldReferenceCounts[inputFieldCount.first] = 1;
			}
			else {

				m_inputFieldReferenceCounts[inputFieldCount.first] += inputFieldCount.second;
			}

		}

		return GlyphGraphTemplate<DataMappingGlyph>::AddChildGlyphGraph(vertex, graph);
	}

	void DataMappingGlyphGraph::UpdateGlyph(const GlyphIterator& vertex, const DataMappingGlyph& glyph) {

		DataMappingGlyph glyphWithoutUnlinkedInputBindings = glyph;
		for (unsigned int field = 0; field < DataMappingGlyph::MappableField::MappableFieldSize; ++field) {

			const InputBinding& selectedBinding = glyphWithoutUnlinkedInputBindings.GetInputBinding(static_cast<DataMappingGlyph::MappableField>(field));
			if (selectedBinding.IsBoundToInputField()) {

				InputField::HashID inputFieldID = selectedBinding.GetInputFieldID();
				if (m_inputFields.count(inputFieldID) == 0) {

					glyphWithoutUnlinkedInputBindings.ClearInputBinding(static_cast<DataMappingGlyph::MappableField>(field));
				}
			}
		}

		GlyphGraphTemplate<DataMappingGlyph>::UpdateGlyph(vertex, glyphWithoutUnlinkedInputBindings);
	}

	bool DataMappingGlyphGraph::IsTransformable() const {

		return ((GetRoot()->second.IsAnInputFieldBoundToAPosition()) && (!m_inputFields.empty()));
	}

	void DataMappingGlyphGraph::SetMergeRoots(bool mergeRoots) {

		m_mergeRoots = mergeRoots;
	}

	bool DataMappingGlyphGraph::GetMergeRoots() const {

		return m_mergeRoots;
	}

} //namespace SynGlyphX
