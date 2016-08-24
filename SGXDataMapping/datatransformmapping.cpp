#include "datatransformmapping.h"
#include <boost/property_tree/xml_parser.hpp>
#include <algorithm>
#include <stdexcept>
#include <boost/uuid/uuid_io.hpp>
#include <boost/property_tree/ptree.hpp>
#include "userdefinedbaseimageproperties.h"
#include "Link.h"
#include "Alias.h"
#include "legend.h"
#include "baseimage.h"
#include "datamappingglyphgraph.h"

namespace SynGlyphX {

	const InputField& InputFieldManager::GetInputField(const std::wstring& fieldID) const
	{
		const static InputField empty;
		auto it = m_inputFields.find(fieldID);
		if (it != m_inputFields.end())
			return it->second;
		else
			return empty;
	}

	void InputFieldManager::RemoveInputFieldAndBindings(const std::wstring& fieldID)
	{
		m_inputFields.erase(fieldID);
		m_dataTransformMapping->ClearInputFieldBindings(fieldID);
		const auto& links = m_dataTransformMapping->GetLinks();
		//need to do it in reverse so removing will not affect indexes
		for (int i = links.size() - 1; i >= 0; --i) 
		{
			if (links[i].m_start.m_inputFieldId == fieldID ||
				links[i].m_end.m_inputFieldId == fieldID)
			{
				m_dataTransformMapping->RemoveLink((unsigned)i);
			}
		}
	}

	void InputFieldManager::OnRemoveDataSource(const boost::uuids::uuid& id)
	{
		std::vector<std::wstring> toRemove;
		for (const auto& field : m_inputFields)
		{
			if (field.second.GetDatasourceID() == id)
				toRemove.push_back(field.first);
		}
		for (const auto& field : toRemove)
			RemoveInputFieldAndBindings(field);
	}

	void InputFieldManager::SetInputField(const std::wstring& fieldID, const InputField& field)
	{
		m_inputFields[fieldID] = field;
	}
	
	std::wstring InputFieldManager::GenerateInputFieldID(const InputField& field) 
	{
		HashID id = field.GetHashID();
		std::wstring str = L"~";
		str += std::to_wstring(id);
		return str;
	}
	
	void InputFieldManager::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const 
	{
		for (auto field : m_inputFields)
		{
			field.second.ExportToPropertyTree(propertyTree, field.first);
		}
	}

	void InputFieldManager::ImportFromPropertyTree(const boost::property_tree::wptree& propertyTree)
	{
		for (const boost::property_tree::wptree::value_type& inputField : propertyTree) 
		{
			if (inputField.first == L"InputField") 
			{	
				m_inputFields[inputField.second.get<std::wstring>(L"<xmlattr>.name")] = InputField(inputField.second);
			}
		}
	}

	void InputFieldManager::Clear()
	{
		m_inputFields.clear();
	}

	const std::unordered_map<std::wstring, InputField>& InputFieldManager::GetFieldMap() const
	{ 
		return m_inputFields;  
	}

	DataTransformMapping::DataTransformMapping() :
		XMLPropertyTreeFile(true),
		m_id(UUIDGenerator::GetNewRandomUUID()),
		m_inputFieldManager(this)
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
		m_legends(mapping.m_legends),
		m_links(mapping.m_links),
		m_inputFieldManager(mapping.m_inputFieldManager),
		m_frontEndFilters(mapping.m_frontEndFilters) {

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

		if (m_frontEndFilters != mapping.m_frontEndFilters) {

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
				//This is for backwards compatibility, TODO: remove in future versions
				boost::optional<const boost::property_tree::wptree&> inputFieldsPropertyTree = glyphPropertyTree.second.get_child_optional(L"InputFields");
				if (inputFieldsPropertyTree.is_initialized()) {

					for (const boost::property_tree::wptree::value_type& inputfieldProperties : inputFieldsPropertyTree.get()) {

						if (inputfieldProperties.first == L"InputField") {

							InputField inputfield(inputfieldProperties.second);
							m_inputFieldManager.SetInputField(m_inputFieldManager.GenerateInputFieldID(inputfield), inputfield);
							//m_inputFields[inputfield.GetHashID()] = inputfield;
							//m_inputFieldReferenceCounts[inputfield.GetHashID()] = 0;
						}
					}
				}
				boost::uuids::uuid treeID = glyphPropertyTree.second.get<boost::uuids::uuid>(L"<xmlattr>.id");
				m_glyphTrees.insert(std::pair<boost::uuids::uuid, DataMappingGlyphGraph::SharedPtr>(treeID, glyphGraph));

				boost::optional<const boost::property_tree::wptree&> frontEndFieldsPropertyTree = glyphPropertyTree.second.get_child_optional(L"FrontEnd");
				if (frontEndFieldsPropertyTree.is_initialized()) {

					//Bryan needs the front end filters in the glyph, but we only need it once so if it has already been read in, skip it
					InputTable table(frontEndFieldsPropertyTree.get().get_child(L"FilterField"));
					if (m_frontEndFilters.count(table) == 0) {

						SingleTableFrontEndFilters filters;
						for (const boost::property_tree::wptree::value_type& frontEndfieldProperties : frontEndFieldsPropertyTree.get()) {

							if (frontEndfieldProperties.first == L"FilterField") {

								std::wstring inputfield = frontEndfieldProperties.second.get<std::wstring>(L"<xmlattr>.field");
								filters.Insert(inputfield, FrontEndFilterOptions(frontEndfieldProperties.second.get<bool>(L"<xmlattr>.required"),
									frontEndfieldProperties.second.get<bool>(L"<xmlattr>.selectall")));
							}
						}

						if (!filters.empty()) {

							m_frontEndFilters[table] = filters;
						}
					}
				}
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

					m_links.emplace_back(linksPropertyTree.second);
				}
			}
		}
		
		boost::optional<const boost::property_tree::wptree&> inputFieldsTree = dataTransformPropertyTree.get_child_optional(L"InputFields");
		if (inputFieldsTree.is_initialized())
			m_inputFieldManager.ImportFromPropertyTree(inputFieldsTree.get());

		//This is for backwards compatibility, TODO: remove in future versions
		for (auto& link : m_links)
		{
			InputField s = m_inputFieldManager.GetInputField(link.m_start.m_inputFieldId);
			if (!s.IsValid())
			{
				std::wstring newId = L"~" + link.m_start.m_inputFieldId;
				s = m_inputFieldManager.GetInputField(newId);
				if (s.IsValid())
					link.m_start.m_inputFieldId = newId;
			}

			InputField e = m_inputFieldManager.GetInputField(link.m_end.m_inputFieldId);
			if (!e.IsValid())
			{
				std::wstring newId = L"~" + link.m_end.m_inputFieldId;
				e = m_inputFieldManager.GetInputField(newId);
				if (e.IsValid())
					link.m_end.m_inputFieldId = newId;
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

			InputTable table = GetInputTalbe(glyphTree.first);
			if (m_frontEndFilters.count(table) != 0) {

				boost::property_tree::wptree& frontEndFiltersPropertyTree = glyphPropertyTree.add(L"FrontEnd", L"");
				for (const auto& field : m_frontEndFilters.at(table)) {

					boost::property_tree::wptree& filterFieldPropertyTree = frontEndFiltersPropertyTree.add(L"FilterField", L"");
					filterFieldPropertyTree.put(L"<xmlattr>.id", table.GetDatasourceID());
					filterFieldPropertyTree.put(L"<xmlattr>.table", table.GetTable());
					filterFieldPropertyTree.put(L"<xmlattr>.field", field.first);
					filterFieldPropertyTree.put(L"<xmlattr>.required", field.second.IsRequired());
					filterFieldPropertyTree.put(L"<xmlattr>.selectall", field.second.IsMultiselectAllowed());
				}
			}
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

		{
			boost::property_tree::wptree& inputFieldsPropertyTree = dataTransformPropertyTreeRoot.add(L"InputFields", L"");
			m_inputFieldManager.ExportToPropertyTree(inputFieldsPropertyTree);
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
		//for (auto glyphTree : m_glyphTrees) {

		//	const InputField& inputField = glyphTree.second->GetInputFields().begin()->second;
		//	datasourcesAndTablesInUse[inputField.GetDatasourceID()].push_back(inputField.GetTable());
		//}
		for (const auto& field : m_inputFieldManager.GetFieldMap())
		{
			datasourcesAndTablesInUse[field.second.GetDatasourceID()].push_back(field.second.GetTable());
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
	Datasource::SharedPtr DataTransformMapping::GetDataSource(boost::uuids::uuid sourceId) {

		return m_datasources.at(sourceId);
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
		m_inputFieldManager.Clear();
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
			m_inputFieldManager.OnRemoveDataSource(id);
			
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
		AddGlyphTree(id, glyphTree);
		return id;
	}

	void DataTransformMapping::AddGlyphTree(const boost::uuids::uuid& id, const DataMappingGlyphGraph::SharedPtr glyphTree) {

		m_glyphTrees.insert(std::pair<boost::uuids::uuid, DataMappingGlyphGraph::SharedPtr>(id, glyphTree));
	}

	const DataTransformMapping::DataMappingGlyphGraphMap& DataTransformMapping::GetGlyphGraphs() const {

		return m_glyphTrees;
	}

	void DataTransformMapping::SetGlyphGraphMap(const DataMappingGlyphGraphMap& glyphGraphs) {
		// create deep copy of glyphTrees
		m_glyphTrees.clear();
		for (auto glyphTree : glyphGraphs) {
			m_glyphTrees.insert(std::pair<boost::uuids::uuid, DataMappingGlyphGraph::SharedPtr>(glyphTree.first, std::make_shared<SynGlyphX::DataMappingGlyphGraph>(*glyphTree.second)));
		}
	
	}

	DataMappingGlyphGraph DataTransformMapping::GetSubgraph(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::ConstGlyphIterator& vertex, bool includeChildren) {

		DataMappingGlyphGraph::SharedPtr glyphTree = m_glyphTrees[treeId];
		return glyphTree->GetSubgraph(vertex.deconstify(), includeChildren);
	}

	const InputTable& DataTransformMapping::GetInputTable(const boost::uuids::uuid& treeId) const {
		DataMappingGlyphGraph::SharedPtr glyphTree = m_glyphTrees.at(treeId);
		return GetInputTable(*glyphTree.get());
	}

	const InputTable& DataTransformMapping::GetInputTable(const DataMappingGlyphGraph& graph) const {
		static const InputTable empty;		
		for (auto i = graph.prefix_begin(); i != graph.prefix_end(); i++){
			auto glyph = i->second;
			for (unsigned int field = 0; field < DataMappingGlyph::MappableField::MappableFieldSize; ++field) {

				InputBinding& binding = const_cast<InputBinding&>(glyph.GetInputBinding(static_cast< DataMappingGlyph::MappableField>(field)));
				if (binding.IsBoundToInputField())
					return m_inputFieldManager.GetInputField(binding.GetInputFieldID());
			}
		}
		return empty;
	}

	bool DataTransformMapping::IsTransformable() const {

		return ((!m_datasources.empty()) && DoesAtLeastOneGlyphGraphHaveBindingsOnPosition());
	}

	bool DataTransformMapping::DoesAtLeastOneGlyphGraphHaveBindingsOnPosition() const {

		unsigned short glyphsWithPositionBindings = 0;
		for (auto glyphGraphPair : m_glyphTrees) {

			DataMappingGlyphGraph::SharedPtr glyphGraph = glyphGraphPair.second;
			//if (glyphGraph->GetInputFields().empty()) {

			//	continue;
			//}

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
	
	void DataTransformMapping::SetInputField(const boost::uuids::uuid& treeID, DataMappingGlyphGraph::ConstGlyphIterator node, DataMappingGlyph::MappableField field, const std::wstring& inputfield) {

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

	void DataTransformMapping::ClearInputFieldBindings(const boost::uuids::uuid& treeID, const std::wstring& inputfield) {
		DataMappingGlyphGraph::SharedPtr glyphTree = m_glyphTrees[treeID];
		glyphTree->ClearInputFieldBindings(inputfield);
	}

	void DataTransformMapping::ClearInputFieldBindings(const std::wstring& inputfield) {
		for (auto glyphTree : m_glyphTrees)
			glyphTree.second->ClearInputFieldBindings(inputfield);
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

		if (GetInputTable(treeId).GetDatasourceID() == GetInputTable(glyphGraph).GetDatasourceID()) {

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

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator newChildGlyph;
		if (GetInputTable(treeId).GetDatasourceID() == GetInputTable(glyphGraph).GetDatasourceID()) {

			SynGlyphX::DataMappingGlyphGraph clearedGlyphGraph = glyphGraph;
			clearedGlyphGraph.ClearAllInputBindings();
			newChildGlyph = m_glyphTrees[treeId]->AddChildGlyphGraph(parent, clearedGlyphGraph);
		}
		else {

			newChildGlyph = m_glyphTrees[treeId]->AddChildGlyphGraph(parent, glyphGraph);
		}
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

		InputFieldManager* newInputFieldManager = subsetMapping->GetInputFieldManager();
		std::unordered_map<std::wstring, std::wstring> oldToNewFieldIDMap;
		for (const auto& field : m_inputFieldManager.GetFieldMap()) {

			InputField newInputField(datasourceID, FileDatasource::SingleTableName, field.second.GetField(), field.second.GetType());
			if (field.first[0] == '~') {

				std::wstring newFieldID = newInputFieldManager->GenerateInputFieldID(newInputField);
				newInputFieldManager->SetInputField(newFieldID, newInputField);
				oldToNewFieldIDMap[field.first] = newFieldID;
			}
			else {

				newInputFieldManager->SetInputField(field.first, newInputField);
				oldToNewFieldIDMap[field.first] = field.first;
			}
		}

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

			if (inputTable == GetInputTable(glyphGraphPair.first)) {

				DataMappingGlyphGraph::SharedPtr glyphGraph(new DataMappingGlyphGraph(*glyphGraphPair.second.get()));
				CopyInputBindingsForSubsetMapping(glyphGraph, glyphGraph->GetRoot(), oldToNewFieldIDMap);

				subsetMapping->AddGlyphTree(glyphGraphPair.first, glyphGraph);
			}
		}

		subsetMapping->m_links = m_links;
		for (auto& link : subsetMapping->m_links) {

			link.m_start.m_inputFieldId = oldToNewFieldIDMap[link.m_start.m_inputFieldId];
			link.m_end.m_inputFieldId = oldToNewFieldIDMap[link.m_end.m_inputFieldId];
		}

		return subsetMapping;
	}

	void DataTransformMapping::CopyInputBindingsForSubsetMapping(DataMappingGlyphGraph::SharedPtr newGlyphGraph, 
																 DataMappingGlyphGraph::GlyphIterator newNode, 
																 const std::unordered_map<std::wstring, std::wstring>& oldToNewIDMap) const {

		for (unsigned int i = 0; i < DataMappingGlyph::MappableField::MappableFieldSize; ++i) {

			DataMappingGlyph::MappableField mappableField = static_cast<DataMappingGlyph::MappableField>(i);
			const InputBinding& inputBinding = newNode->second.GetInputBinding(mappableField);
			if (inputBinding.IsBoundToInputField()) {

				newGlyphGraph->SetInputField(newNode.constify(), mappableField, oldToNewIDMap.at(inputBinding.GetInputFieldID()));
			}
		}

		for (unsigned int j = 0; j < newGlyphGraph->ChildCount(newNode.constify()); ++j) {

			CopyInputBindingsForSubsetMapping(newGlyphGraph, newGlyphGraph->GetChild(newNode, j), oldToNewIDMap);
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

	void DataTransformMapping::InsertLink(unsigned int index, const Link& link) {
		if (index < m_links.size()) {
			m_links.insert(m_links.begin() + index, link);
		}
		else if (index == m_links.size()) {
			m_links.push_back(link);
		}
		else {

			throw std::invalid_argument("Index is greater than size of links");
		}
	}

	void DataTransformMapping::RemoveLink(unsigned int index) {
		if (index < m_links.size()) {

			m_links.erase(m_links.begin() + index);
		}
		else {

			throw std::invalid_argument("Index is greater than size of links");
		}
	}

	void DataTransformMapping::SetLink(unsigned int index, const Link& link) {

		if (index < m_links.size()) {

			m_links[index] = link;
		}
		else {

			throw std::invalid_argument("Index is greater than size of links");
		}
	}

	void DataTransformMapping::SetLinks(const std::vector<Link>& links) {
		m_links = links; 
	}

	std::unordered_map<std::wstring, std::wstring> DataTransformMapping::GetFieldToAliasMapForTable(const InputTable& table) const {

		std::unordered_map<std::wstring, std::wstring> fieldToAliasMap;

		for (const auto& field : m_inputFieldManager.GetFieldMap()) {

			if ((table == field.second) && (field.first[0] != '~')) {

				fieldToAliasMap[field.second.GetField()] = field.first;
			}
		}

		return fieldToAliasMap;
	}

	std::wstring DataTransformMapping::GetFormattedName(const boost::uuids::uuid& datasourceID, const std::wstring& table) const {

		if (m_datasources.count(datasourceID) == 0) {

			throw std::invalid_argument("Can't get formatted name since datasource ID does not exist in this mapping.");
		}

		std::wstring formattedName = m_datasources.at(datasourceID)->GetFormattedName();
		if (table != Datasource::SingleTableName) {

			formattedName += L":";
			formattedName += table;
		}

		return formattedName;
	}
} //namespace SynGlyphX
