#include "datatransformmapping.h"
#include <boost/property_tree/xml_parser.hpp>
#include <algorithm>
#include <stdexcept>
#include <boost/uuid/uuid_io.hpp>
#include "userdefinedbaseimageproperties.h"

namespace SynGlyphX {

	DataTransformMapping::DataTransformMapping() :
		XMLPropertyTreeFile(true),
		m_id(UUIDGenerator::GetNewRandomUUID())
    {
		//There will always be at least one base object
		m_baseObjects.push_back(BaseImage(nullptr));
    }

	DataTransformMapping::DataTransformMapping(const DataTransformMapping& mapping) :
		XMLPropertyTreeFile(),
		m_id(UUIDGenerator::GetNewRandomUUID()),
		m_datasources(mapping.m_datasources),
		m_glyphTrees(mapping.m_glyphTrees), 
		m_baseObjects(mapping.m_baseObjects),
		m_defaults(mapping.m_defaults),
		m_sceneProperties(mapping.m_sceneProperties),
		m_fieldGroups(mapping.m_fieldGroups),
		m_legends(mapping.m_legends) {

	}

	DataTransformMapping::~DataTransformMapping()
    {
    }

	bool DataTransformMapping::operator==(const DataTransformMapping& mapping) const {

		if (m_id != mapping.m_id) {

			return false;
		}

		if (m_datasources.size() != mapping.m_datasources.size()) {

			return false;
		}

		DatasourceMap::const_iterator thisDatasource = m_datasources.begin();
		DatasourceMap::const_iterator otherDatasource = mapping.m_datasources.begin();
		while (thisDatasource != m_datasources.end()) {

			if ((thisDatasource->first != otherDatasource->first) || (thisDatasource->second->operator!=(*otherDatasource->second))) {

				return false;
			}
			std::advance(thisDatasource, 1);
			std::advance(otherDatasource, 1);
		}

		if (m_glyphTrees.size() != mapping.m_glyphTrees.size()) {

			return false;
		}

		DataMappingGlyphGraphMap::const_iterator thisGlyphTree = m_glyphTrees.begin();
		DataMappingGlyphGraphMap::const_iterator otherGlyphTree = mapping.m_glyphTrees.begin();
		while (thisGlyphTree != m_glyphTrees.end()) {

			if ((thisGlyphTree->first != otherGlyphTree->first) || (thisGlyphTree->second->operator!=(*otherGlyphTree->second))) {

				return false;
			}
			std::advance(thisGlyphTree, 1);
			std::advance(otherGlyphTree, 1);
		}

		if (m_baseObjects != mapping.m_baseObjects) {

			return false;
		}

		if (m_defaults != mapping.m_defaults) {

			return false;
		}

		if (m_sceneProperties != mapping.m_sceneProperties) {

			return false;
		}

		if (m_fieldGroups != mapping.m_fieldGroups) {

			return false;
		}

		if (m_legends != mapping.m_legends) {

			return false;
		}

		return true;
	}

	bool DataTransformMapping::operator!=(const DataTransformMapping& mapping) const {

		return !operator==(mapping);
	}

	void DataTransformMapping::ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree) {

        Clear(false);

		const boost::property_tree::wptree& dataTransformPropertyTree = filePropertyTree.get_child(L"Transform");

		m_id = dataTransformPropertyTree.get<boost::uuids::uuid>(L"<xmlattr>.id");

		boost::optional<const boost::property_tree::wptree&> baseObjectsPropertyTree = dataTransformPropertyTree.get_child_optional(L"BaseObjects");
		if (baseObjectsPropertyTree.is_initialized()) {

			for (const boost::property_tree::wptree::value_type& baseObjectPropertyTree : baseObjectsPropertyTree.get()) {

				if (baseObjectPropertyTree.first == L"BaseObject") {

					m_baseObjects.push_back(BaseImage(baseObjectPropertyTree.second));
				}
			}
		}
		else {

			m_baseObjects.push_back(BaseImage(dataTransformPropertyTree.get_child(L"BaseImage")));
		}

		boost::optional<const boost::property_tree::wptree&> datasourcesPropertyTree = dataTransformPropertyTree.get_child_optional(L"Datasources");
		if (datasourcesPropertyTree.is_initialized()) {

			for (const boost::property_tree::wptree::value_type& datasourcePropertyTree : datasourcesPropertyTree.get()) {

				std::wstring fileSourceString = Datasource::s_sourceTypeStrings.left.at(Datasource::SourceType::File);
				Datasource::SharedPtr newDatasource = nullptr;
				if ((datasourcePropertyTree.first == fileSourceString) || (datasourcePropertyTree.first == L"Datasource")) {

					Datasource::SourceType source = Datasource::s_sourceTypeStrings.right.at(datasourcePropertyTree.second.get_optional<std::wstring>(L"<xmlattr>.source").get_value_or(fileSourceString));

					if (source == Datasource::SourceType::File) {

						newDatasource = std::make_shared<FileDatasource>(datasourcePropertyTree.second);
					}
					else if (source == Datasource::SourceType::DatabaseServer) {

						newDatasource = std::make_shared<DatabaseServerDatasource>(datasourcePropertyTree.second);
					}
				}

				if (newDatasource != nullptr) {

					m_datasources.insert(std::pair<boost::uuids::uuid, Datasource::SharedPtr>(datasourcePropertyTree.second.get<boost::uuids::uuid>(L"<xmlattr>.id"), newDatasource));
				}
			}
		}

		for (const boost::property_tree::wptree::value_type& glyphPropertyTree : dataTransformPropertyTree.get_child(L"Glyphs")) {

			if (glyphPropertyTree.first == L"Glyph") {

				DataMappingGlyphGraph::SharedPtr glyphGraph = std::make_shared<DataMappingGlyphGraph>(glyphPropertyTree.second);
				m_glyphTrees.insert(std::pair<boost::uuids::uuid, DataMappingGlyphGraph::SharedPtr>(glyphPropertyTree.second.get<boost::uuids::uuid>(L"<xmlattr>.id"), glyphGraph));
			}
		}

		boost::optional<const boost::property_tree::wptree&> defaultsPropertyTree = dataTransformPropertyTree.get_child_optional(L"Defaults");
		if (defaultsPropertyTree.is_initialized()) {

			m_defaults = DataMappingDefaults(defaultsPropertyTree.get());
		}
		else {

			m_defaults = DataMappingDefaults();
		}

		boost::optional<const boost::property_tree::wptree&> scenePropertyTree = dataTransformPropertyTree.get_child_optional(SceneProperties::s_propertyTreeName);
		if (scenePropertyTree.is_initialized()) {

			m_sceneProperties = SceneProperties(scenePropertyTree.get());
		}
		else {

			m_sceneProperties = SceneProperties();
		}

		boost::optional<const boost::property_tree::wptree&> fieldGroupsPropertyTree = dataTransformPropertyTree.get_child_optional(L"FieldGroups");
		if (fieldGroupsPropertyTree.is_initialized()) {

			for (const boost::property_tree::wptree::value_type& fieldGroupPropertyTree : fieldGroupsPropertyTree.get()) {

				if (fieldGroupPropertyTree.first == L"FieldGroup") {

					FieldGroup fieldGroup;

					for (const boost::property_tree::wptree::value_type& fieldPropertyTree : fieldGroupPropertyTree.second) {

						if (fieldPropertyTree.first == L"InputField") {
						
							fieldGroup.insert(InputField(fieldPropertyTree.second));
						}
					}

					if (!fieldGroup.empty()) {

						m_fieldGroups[fieldGroupPropertyTree.second.get<FieldGroupName>(L"<xmlattr>.name")] = fieldGroup;
					}
				}
			}
		}

		boost::optional<const boost::property_tree::wptree&> legendsTree = dataTransformPropertyTree.get_child_optional(L"Legends");
		if (legendsTree.is_initialized()) {

			for (const boost::property_tree::wptree::value_type& legendPropertyTree : legendsTree.get()) {

				if (legendPropertyTree.first == Legend::s_propertyTreeName) {

					m_legends.emplace_back(legendPropertyTree.second);
				}
			}
		}

		boost::optional<const boost::property_tree::wptree&> linksTree = dataTransformPropertyTree.get_child_optional(L"Links");
		if (linksTree.is_initialized()) {

			for (const boost::property_tree::wptree::value_type& linksPropertyTree : linksTree.get()) {

				if (linksPropertyTree.first == L"Link") {

					m_links.emplace_back(Link(linksPropertyTree.second));
				}
			}
		}
    }

	void DataTransformMapping::ExportToPropertyTree(boost::property_tree::wptree& filePropertyTree) const {

		boost::property_tree::wptree& dataTransformPropertyTreeRoot = filePropertyTree.add(L"Transform", L"");
		dataTransformPropertyTreeRoot.put(L"<xmlattr>.id", m_id);

		boost::property_tree::wptree& baseObjectsPropertyTree = dataTransformPropertyTreeRoot.add(L"BaseObjects", L"");
		for (const BaseImage& baseObject : m_baseObjects) {

			baseObject.ExportToPropertyTree(baseObjectsPropertyTree);
		}

		boost::property_tree::wptree& datasourcesPropertyTree = dataTransformPropertyTreeRoot.add(L"Datasources", L"");
		for (auto datasource : m_datasources) {

			Datasource::PropertyTree& datasourcePropertyTree = datasource.second->ExportToPropertyTree(datasourcesPropertyTree);
			datasourcePropertyTree.put(L"<xmlattr>.id", datasource.first);
		}

		boost::property_tree::wptree& glyphTreesPropertyTree = dataTransformPropertyTreeRoot.add(L"Glyphs", L"");
		for (auto glyphTree : m_glyphTrees) {
			
			boost::property_tree::wptree& glyphPropertyTree = glyphTree.second->ExportToPropertyTree(glyphTreesPropertyTree);
			glyphPropertyTree.put(L"<xmlattr>.id", glyphTree.first);
		}

		m_defaults.ExportToPropertyTree(dataTransformPropertyTreeRoot);
		m_sceneProperties.ExportToPropertyTree(dataTransformPropertyTreeRoot);

		boost::property_tree::wptree& fieldGroupsPropertyTree = dataTransformPropertyTreeRoot.add(L"FieldGroups", L"");
		for (const auto& fieldGroup : m_fieldGroups) {

			boost::property_tree::wptree& fieldGroupPropertyTree = fieldGroupsPropertyTree.add(L"FieldGroup", L"");
			fieldGroupPropertyTree.put(L"<xmlattr>.name", fieldGroup.first);
			for (const InputField& field : fieldGroup.second) {

				field.ExportToPropertyTree(fieldGroupPropertyTree);
			}
		}

		if (!m_legends.empty()) {

			boost::property_tree::wptree& legendsPropertyTree = dataTransformPropertyTreeRoot.add(L"Legends", L"");
			for (const auto& legend : m_legends) {

				legend.ExportToPropertyTree(legendsPropertyTree);
			}
		}

		if (!m_links.empty()){
			boost::property_tree::wptree& linksPropertyTree = dataTransformPropertyTreeRoot.add(L"Links", L"");
			for (const auto& link : m_links) {

				link.ExportToPropertyTree(linksPropertyTree);
			}
		}
    }

	bool DataTransformMapping::IsDifferentFromGivenPropertyTree(const boost::property_tree::wptree& originalPropertyTree) const {

		DataTransformMapping origininalPropertyTreeMapping;
		origininalPropertyTreeMapping.ImportFromPropertyTree(originalPropertyTree);

		return operator!=(origininalPropertyTreeMapping);
	}

	const DataTransformMapping::DatasourceMap& DataTransformMapping::GetDatasources() const {

		return m_datasources;
	}

	DataTransformMapping::DatasourceMap DataTransformMapping::GetDatasourcesInUse() const {

		DatasourceMap datasourceMap;

		std::unordered_map<boost::uuids::uuid, Datasource::TableNames, SynGlyphX::UUIDHash> datasourcesAndTablesInUse;
		for (auto glyphTree : m_glyphTrees) {

			const InputField& inputField = glyphTree.second->GetInputFields().begin()->second;
			datasourcesAndTablesInUse[inputField.GetDatasourceID()].push_back(inputField.GetTable());
		}

		for (auto datasourceTables : datasourcesAndTablesInUse) {

			Datasource::SharedPtr copiedDatasource = nullptr;
			Datasource::SharedPtr origDatasource = m_datasources.at(datasourceTables.first);
			if (origDatasource->GetSourceType() == Datasource::SourceType::File) {

				copiedDatasource = std::make_shared<FileDatasource>(*std::dynamic_pointer_cast<FileDatasource>(origDatasource));
			}
			else if (origDatasource->GetSourceType() == Datasource::SourceType::DatabaseServer) {

				copiedDatasource = std::make_shared<DatabaseServerDatasource>(*std::dynamic_pointer_cast<DatabaseServerDatasource>(origDatasource));
			}
			copiedDatasource->ClearTables();
			copiedDatasource->AddTables(datasourceTables.second);
			datasourceMap.insert(std::pair<boost::uuids::uuid, Datasource::SharedPtr>(datasourceTables.first, copiedDatasource));
		}

		return datasourceMap;
	}

	UUIDUnorderedSet DataTransformMapping::GetDatasourcesBySourceType(Datasource::SourceType type) const {

		UUIDUnorderedSet datasourceIDs;
		for (auto& datasource : m_datasources) {

			if (datasource.second->GetSourceType() == type) {

				datasourceIDs.insert(datasource.first);
			}
		}

		return datasourceIDs;
	}

	void DataTransformMapping::Clear() {

		Clear(true);
	}
		
	void DataTransformMapping::Clear(bool addADefaultBaseObjectAfterClear) {

		m_fieldGroups.clear();
		m_datasources.clear();
		m_glyphTrees.clear();
		m_defaults.Clear();
		m_baseObjects.clear();
		m_legends.clear();
		m_links.clear();
		m_id = UUIDGenerator::GetNewRandomUUID();

		if (addADefaultBaseObjectAfterClear) {

			m_baseObjects.push_back(BaseImage(nullptr));
			//Default to showing grid lines for the first base object
			m_baseObjects[0].SetGridLineCounts({ { 5, 11 } });
		}
    }

	bool DataTransformMapping::HasDatasourceWithId(const boost::uuids::uuid& id) const {

		return (m_datasources.count(id) > 0);
	}

	void DataTransformMapping::RemoveDatasource(const boost::uuids::uuid& id) {

		try {

			//Clear all input bindings that use the datasource before removing the datasource
			for (auto& glyphTree : m_glyphTrees) {

				if (glyphTree.second->GetInputFields().begin()->second.GetDatasourceID() == id) {

					glyphTree.second->ClearAllInputBindings();
				}
			}

			//Clear all field groups that use the datasource before removing the datasource
			std::vector<FieldGroupName> fieldGroupsToBeRemoved;
			for (auto& fieldGroup : m_fieldGroups) {

				if (DoesFieldGroupHaveFieldsFromDatasource(fieldGroup.first, id)) {

					fieldGroupsToBeRemoved.push_back(fieldGroup.first);
				}
			}

			for (const FieldGroupName& name : fieldGroupsToBeRemoved) {

				RemoveFieldGroup(name);
			}

			m_datasources.erase(id);
		}
		catch (const std::invalid_argument& e) {
			
			throw;
		}
	}

	boost::uuids::uuid DataTransformMapping::AddFileDatasource(const FileDatasource& datasource) {

		boost::uuids::uuid id = UUIDGenerator::GetNewRandomUUID();
		m_datasources.insert(std::pair<boost::uuids::uuid, Datasource::SharedPtr>(id, std::make_shared<FileDatasource>(datasource)));

        return id;
    }

	boost::uuids::uuid DataTransformMapping::AddDatabaseServer(const DatabaseServerDatasource& datasource) {

		boost::uuids::uuid id = UUIDGenerator::GetNewRandomUUID();
		m_datasources.insert(std::pair<boost::uuids::uuid, Datasource::SharedPtr>(id, std::make_shared<DatabaseServerDatasource>(datasource)));

		return id;
	}

	void DataTransformMapping::EnableTables(const boost::uuids::uuid& id, const Datasource::TableNames& tables, bool enable) {

		DatasourceMap::iterator datasourceIterator = m_datasources.find(id);
		if (datasourceIterator != m_datasources.end()) {

			if (enable) {

				datasourceIterator->second->AddTables(tables);
			}
			else {

				for (const std::wstring& table : tables) {

					datasourceIterator->second->RemoveTable(table);
				}
			}
		}
    }

	boost::uuids::uuid DataTransformMapping::AddGlyphTree(const DataMappingGlyphGraph::SharedPtr glyphTree) {

		boost::uuids::uuid id = UUIDGenerator::GetNewRandomUUID();

		m_glyphTrees.insert(std::pair<boost::uuids::uuid, DataMappingGlyphGraph::SharedPtr>(id, glyphTree));

		return id;
	}

	const DataTransformMapping::DataMappingGlyphGraphMap& DataTransformMapping::GetGlyphGraphs() const {

		return m_glyphTrees;
	}

	DataMappingGlyphGraph DataTransformMapping::GetSubgraph(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::ConstGlyphIterator& vertex, bool includeChildren) {

		DataMappingGlyphGraph::SharedPtr glyphTree = m_glyphTrees[treeId];
		return glyphTree->GetSubgraph(vertex.deconstify(), includeChildren);
	}

	bool DataTransformMapping::IsTransformable() const {

		return ((!m_datasources.empty()) && DoesAtLeastOneGlyphGraphHaveBindingsOnPosition());
	}

	bool DataTransformMapping::DoesAtLeastOneGlyphGraphHaveBindingsOnPosition() const {

		unsigned short glyphsWithPositionBindings = 0;
		for (auto glyphGraphPair : m_glyphTrees) {

			DataMappingGlyphGraph::SharedPtr glyphGraph = glyphGraphPair.second;
			if (glyphGraph->GetInputFields().empty()) {

				continue;
			}

			if (!glyphGraph->GetRoot()->second.IsAnInputFieldBoundToAPosition()) {

				return false;
			}

			++glyphsWithPositionBindings;
		}

		return (glyphsWithPositionBindings > 0);
	}

	void DataTransformMapping::AddBaseObject(const BaseImage& baseObject) {

		m_baseObjects.push_back(baseObject);
	}

	void DataTransformMapping::RemoveBaseObject(unsigned int index) {

		if (index < m_baseObjects.size()) {

			std::vector<BaseImage>::iterator baseObject = m_baseObjects.begin();
			std::advance(baseObject, index);
			m_baseObjects.erase(baseObject);
		}
		else {

			throw std::invalid_argument("Index is greater than size of base objects");
		}
	}

	void DataTransformMapping::SetBaseObject(unsigned int index, const BaseImage& baseObject) {

		if (index < m_baseObjects.size()) {

			m_baseObjects[index] = baseObject;
		}
		else {

			throw std::invalid_argument("Index is greater than size of base objects");
		}
	}

	const std::vector<BaseImage>& DataTransformMapping::GetBaseObjects() const {

		return m_baseObjects;
	}
	
	void DataTransformMapping::SetInputField(const boost::uuids::uuid& treeID, DataMappingGlyphGraph::ConstGlyphIterator node, DataMappingGlyph::MappableField field, const InputField& inputfield) {

		DataMappingGlyphGraph::SharedPtr glyphTree = m_glyphTrees[treeID];
		glyphTree->SetInputField(node, field, inputfield);
	}

	void DataTransformMapping::ClearInputBinding(const boost::uuids::uuid& treeID, DataMappingGlyphGraph::ConstGlyphIterator& node, DataMappingGlyph::MappableField field) {

		DataMappingGlyphGraph::SharedPtr glyphTree = m_glyphTrees[treeID];
		glyphTree->ClearInputBinding(node, field);
	}

	void DataTransformMapping::ClearAllInputBindings(const boost::uuids::uuid& treeID, DataMappingGlyphGraph::ConstGlyphIterator& node) {

		DataMappingGlyphGraph::SharedPtr glyphTree = m_glyphTrees[treeID];
		glyphTree->ClearAllInputBindings(node);
	}

	void DataTransformMapping::ClearInputFieldBindings(const boost::uuids::uuid& treeID, const InputField& inputfield) {
		DataMappingGlyphGraph::SharedPtr glyphTree = m_glyphTrees[treeID];
		glyphTree->ClearInputFieldBindings(inputfield);
	}

	const boost::uuids::uuid& DataTransformMapping::GetID() const {

		return m_id;
	}

	void DataTransformMapping::ResetID() {

		m_id = UUIDGenerator::GetNewRandomUUID();
	}

	void DataTransformMapping::UpdateDatasourceName(const boost::uuids::uuid& id, const std::wstring& name) {

		try {
			
			DatasourceMap::iterator datasourceIterator = m_datasources.find(id);
			if (datasourceIterator != m_datasources.end()) {

				if (datasourceIterator->second->GetSourceType() == Datasource::SourceType::File) {

					FileDatasource::SharedPtr fileDatasource = std::dynamic_pointer_cast<FileDatasource>(datasourceIterator->second);
					fileDatasource->ChangeFilename(name);
				}
				else {

					throw std::invalid_argument("ID was for a datasource that was not a file datasource.");
				}
			}
			else {

				//If id wasn't found in any datasource throw invalid_argument exception
				throw std::invalid_argument("ID does not exist in datasources for this data transform mapping.");
			}
		}
		catch (const std::invalid_argument& e) {

			throw;
		}
	}

	const DataTransformMapping::FieldGroupMap& DataTransformMapping::GetFieldGroupMap() const {

		return m_fieldGroups;
	}

	void DataTransformMapping::UpdateFieldGroup(const FieldGroupName& groupName, const FieldGroup& fieldGroup) {

		if (groupName.empty()) {

			throw std::invalid_argument("Can not add or update field group with an empty name");
		}

		if (fieldGroup.empty()) {

			throw std::invalid_argument("Can not add or update field group with an empty list of fields");
		}

		m_fieldGroups[groupName] = fieldGroup;
	}

	void DataTransformMapping::RemoveFieldGroup(const FieldGroupName& groupName) {

		if (m_fieldGroups.count(groupName) > 0) {

			for (auto& glyphGraph : m_glyphTrees) {

				glyphGraph.second->ClearFieldGroup(groupName);
			}
			m_fieldGroups.erase(groupName);
		}
	}

	bool DataTransformMapping::DoesFieldGroupHaveFieldsFromDatasource(const FieldGroupName& groupName, const boost::uuids::uuid& datasourceId) const {

		for (const InputField& field : m_fieldGroups.at(groupName)) {

			if (field.GetDatasourceID() == datasourceId) {

				return true;
			}
		}

		return false;
	}

	void DataTransformMapping::RemoveGlyphTree(const boost::uuids::uuid& id) {

		if (m_glyphTrees.erase(id) == 0) {

			throw std::invalid_argument("Glyph tree was not in list so it wasn't erased");
		}
	}

	const DataMappingDefaults& DataTransformMapping::GetDefaults() const {

		return m_defaults;
	}

	void DataTransformMapping::SetDefaults(const DataMappingDefaults& defaults) {

		m_defaults = defaults;
	}

	void DataTransformMapping::AddChildGlyph(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::GlyphIterator& parent, const DataMappingGlyph& glyphTemplate, unsigned int numberOfChildren) {

		if (!parent.valid()) {

			throw std::invalid_argument("Can't append children to invalid parent");
		}

		if (numberOfChildren == 0) {

			throw std::invalid_argument("Can't append 0 children");
		}

		unsigned int startingNumberOfChildren = m_glyphTrees[treeId]->ChildCount(parent.constify());
		SynGlyphX::Vector3 newPosition = { { 15.0, 0.0, 0.0 } };
		if (startingNumberOfChildren > 0) {

			newPosition = m_glyphTrees[treeId]->GetChild(parent, startingNumberOfChildren - 1)->second.GetMinGlyph().GetPosition();
		}

		for (int i = 0; i < numberOfChildren; ++i) {

			SynGlyphX::DataMappingGlyphGraph::GlyphIterator newChildGlyph = m_glyphTrees[treeId]->AddChildGlyph(parent, glyphTemplate);

			//For now, update position to 15.0 less than the last x coordinate.  This follows what ANTz does
			newPosition[0] -= 15.0;
			newChildGlyph->second.GetPosition()[0].GetValue().SetMinDiff(newPosition[0], 0.0);
			newChildGlyph->second.GetPosition()[1].GetValue().SetMinDiff(newPosition[1], 0.0);
			newChildGlyph->second.GetPosition()[2].GetValue().SetMinDiff(newPosition[2], 0.0);
		}
	}

	void DataTransformMapping::AddChildTree(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::GlyphIterator& parent, const SynGlyphX::DataMappingGlyphGraph& glyphGraph) {

		if (!parent.valid()) {

			throw std::invalid_argument("Can't append children to invalid parent");
		}

		if ((!glyphGraph.GetInputFields().empty()) && (!HasDatasourceWithId(glyphGraph.GetInputFields().begin()->second.GetDatasourceID()))) {

			SynGlyphX::DataMappingGlyphGraph clearedGlyphGraph = glyphGraph;
			clearedGlyphGraph.ClearAllInputBindings();
			m_glyphTrees[treeId]->AddChildGlyphGraph(parent, clearedGlyphGraph);
		}
		else {

			m_glyphTrees[treeId]->AddChildGlyphGraph(parent, glyphGraph);
		}
	}

	void DataTransformMapping::AddChildTreeResetPosition(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::GlyphIterator parent, const SynGlyphX::DataMappingGlyphGraph& glyphGraph) {

		if (!parent.valid()) {

			throw std::invalid_argument("Can't append children to invalid parent");
		}

		unsigned int startingNumberOfChildren = m_glyphTrees[treeId]->ChildCount(parent.constify());
		SynGlyphX::Vector3 newPosition = { { 15.0, 0.0, 0.0 } };
		if (startingNumberOfChildren > 0) {

			newPosition = m_glyphTrees[treeId]->GetChild(parent, startingNumberOfChildren - 1)->second.GetMinGlyph().GetPosition();
			//For now, update position to 15.0 less than the last x coordinate.  This follows what ANTz does
			newPosition[0] -= 15.0;
		}

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator newChildGlyph = m_glyphTrees[treeId]->AddChildGlyphGraph(parent, glyphGraph);
		newChildGlyph->second.GetPosition()[0].GetValue().SetMinDiff(newPosition[0], 0.0);
		newChildGlyph->second.GetPosition()[1].GetValue().SetMinDiff(newPosition[1], 0.0);
		newChildGlyph->second.GetPosition()[2].GetValue().SetMinDiff(newPosition[2], 0.0);
	}

	void DataTransformMapping::RemoveGlyph(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::ConstGlyphIterator& parent, int child) {

		m_glyphTrees[treeId]->RemoveChild(parent.deconstify(), child);
	}

	void DataTransformMapping::UpdateGlyph(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::GlyphIterator& vertex, const DataMappingGlyph& glyph) {

		m_glyphTrees[treeId]->UpdateGlyph(vertex, glyph);
	}

	const SceneProperties& DataTransformMapping::GetSceneProperties() const {

		return m_sceneProperties;
	}

	void DataTransformMapping::SetSceneProperties(const SceneProperties& sceneProperties) {

		m_sceneProperties = sceneProperties;
	}

	std::vector<boost::uuids::uuid> DataTransformMapping::GetFileDatasourcesWithInvalidFiles(bool onlyUseDatasourcesInUse) const {

		DatasourceMap datasources;
		if (onlyUseDatasourcesInUse) {
		
			datasources = GetDatasourcesInUse();
		}
		else {

			datasources = m_datasources;
		}
		std::vector<boost::uuids::uuid> fileDatasourcesToBeUpdated;
		for (DatasourceMap::const_iterator datasource = datasources.begin(); datasource != datasources.end(); ++datasource) {

			if ((!datasource->second->CanDatasourceBeFound()) && (datasource->second->GetSourceType() == Datasource::SourceType::File)) {

				fileDatasourcesToBeUpdated.push_back(datasource->first);
			}
		}

		return fileDatasourcesToBeUpdated;
	}

	std::vector<unsigned int> DataTransformMapping::GetFileBaseObjectsWithInvalidFiles() const {

		std::vector<unsigned int> missingLocalBaseImages;

		for (unsigned int i = 0; i < GetBaseObjects().size(); ++i) {

			UserDefinedBaseImageProperties::ConstSharedPtr properties = std::dynamic_pointer_cast<const SynGlyphX::UserDefinedBaseImageProperties>(GetBaseObjects()[i].GetProperties());
			if ((properties != nullptr) && (!properties->CanFileBeFound())) {

				missingLocalBaseImages.push_back(i);
			}
		}

		return missingLocalBaseImages;
	}

	std::vector<unsigned int> DataTransformMapping::GetLegendsWithInvalidFiles() const {

		std::vector<unsigned int> missingImages;

		for (unsigned int i = 0; i < m_legends.size(); ++i) {

			if (!m_legends[i].CanFileBeFound()) {

				missingImages.push_back(i);
			}
		}

		return missingImages;
	}

	DataTransformMapping::ConstSharedPtr DataTransformMapping::CreateSubsetMappingWithSingleTable(const InputTable& inputTable, const std::wstring& csvFilename) const {

		if (!HasDatasourceWithId(inputTable.GetDatasourceID())) {

			throw std::runtime_error("Can't create subset of mapping with a datasource ID that does not exist in the mapping.");
		}

		SharedPtr subsetMapping(new DataTransformMapping());
		subsetMapping->SetSceneProperties(m_sceneProperties);
		subsetMapping->SetDefaults(m_defaults);
		subsetMapping->SetLegends(m_legends);

		if (!subsetMapping->GetBaseObjects().empty()) {

			subsetMapping->RemoveBaseObject(0);
		}
		for (auto baseImage : m_baseObjects) {

			subsetMapping->AddBaseObject(baseImage);
		}

		SynGlyphX::FileDatasource newDatasource(FileDatasource::FileType::CSV, csvFilename);
		boost::uuids::uuid datasourceID = subsetMapping->AddFileDatasource(newDatasource);

		for (auto fieldGroupPair : m_fieldGroups) {

			if (inputTable == *fieldGroupPair.second.begin()) {

				FieldGroup fieldGroup;
				for (auto field : fieldGroupPair.second) {

					fieldGroup.insert(InputField(datasourceID, FileDatasource::SingleTableName, field.GetField(), field.GetType()));
				}

				subsetMapping->UpdateFieldGroup(fieldGroupPair.first, fieldGroup);
			}
		}

		for (auto glyphGraphPair : m_glyphTrees) {

			if (inputTable == glyphGraphPair.second->GetInputFields().begin()->second) {

				DataMappingGlyphGraph::SharedPtr glyphGraph(new DataMappingGlyphGraph(*glyphGraphPair.second.get()));
				glyphGraph->ClearAllInputBindings();
				CopyInputBindingsForSubsetMapping(glyphGraph, glyphGraph->GetRoot(), glyphGraphPair.second, glyphGraphPair.second->GetRoot().constify(), datasourceID);
				subsetMapping->AddGlyphTree(glyphGraph);
			}
		}

		return subsetMapping;
	}

	void DataTransformMapping::CopyInputBindingsForSubsetMapping(DataMappingGlyphGraph::SharedPtr newGlyphGraph, 
																 DataMappingGlyphGraph::GlyphIterator newNode, 
																 DataMappingGlyphGraph::ConstSharedPtr oldGlyphGraph, 
																 DataMappingGlyphGraph::ConstGlyphIterator oldNode,
																 const boost::uuids::uuid& datasourceID) const {

		for (unsigned int i = 0; i < DataMappingGlyph::MappableField::MappableFieldSize; ++i) {

			DataMappingGlyph::MappableField mappableField = static_cast<DataMappingGlyph::MappableField>(i);
			const InputBinding& inputBinding = oldNode->second.GetInputBinding(mappableField);
			if (inputBinding.IsBoundToInputField()) {

				const InputField& oldInputField = oldGlyphGraph->GetInputFields().at(inputBinding.GetInputFieldID());
				newGlyphGraph->SetInputField(newNode.constify(), mappableField, InputField(datasourceID, FileDatasource::SingleTableName, oldInputField.GetField(), oldInputField.GetType()));
			}
		}

		for (unsigned int j = 0; j < oldGlyphGraph->ChildCount(oldNode); ++j) {

			CopyInputBindingsForSubsetMapping(newGlyphGraph, newGlyphGraph->GetChild(newNode, j), oldGlyphGraph, oldGlyphGraph->GetChild(oldNode, j), datasourceID);
		}
	}

	void DataTransformMapping::AddLegend(const Legend& legend) {

		m_legends.push_back(legend);
	}

	void DataTransformMapping::RemoveLegend(unsigned int index) {

		if (index < m_legends.size()) {

			std::vector<Legend>::iterator legend = m_legends.begin();
			std::advance(legend, index);
			m_legends.erase(legend);
		}
		else {

			throw std::invalid_argument("Index is greater than size of base objects");
		}
	}

	void DataTransformMapping::SetLegend(unsigned int index, const Legend& legend) {

		if (index < m_legends.size()) {

			m_legends[index] = legend;
		}
		else {

			throw std::invalid_argument("Index is greater than size of legends");
		}
	}

	const std::vector<Legend>& DataTransformMapping::GetLegends() const {

		return m_legends;
	}

	void DataTransformMapping::SetLegends(const std::vector<Legend>& legends) {

		m_legends = legends;
	}

	void DataTransformMapping::AddLink(const SynGlyphX::Link& link) {
		m_links.push_back(link);
	}

	void DataTransformMapping::RemoveLink(unsigned int index) {
		if (index < m_links.size()) {

			m_links.erase(m_links.begin() + index);
		}
		else {

			throw std::invalid_argument("Index is greater than size of base objects");
		}

	}

} //namespace SynGlyphX
