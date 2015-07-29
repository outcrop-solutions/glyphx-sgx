#include "datatransformmapping.h"
#include <boost/property_tree/xml_parser.hpp>
#include <algorithm>
#include <boost/uuid/uuid_io.hpp>
#include <QtCore/QVariant>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include "sourcedatamanager.h"

namespace SynGlyphX {

	DataTransformMapping::DataTransformMapping() :
		XMLPropertyTreeFile(),
		m_id(UUIDGenerator::GetNewRandomUUID())
    {
		//There will always be at least one base object
		m_baseObjects.push_back(BaseImage(nullptr));
    }

	DataTransformMapping::~DataTransformMapping()
    {
    }

	bool DataTransformMapping::operator==(const DataTransformMapping& mapping) const {

		if (m_id != mapping.m_id) {

			return false;
		}

		if (m_datasources != mapping.m_datasources) {

			return false;
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

		m_datasources = DatasourceMaps(dataTransformPropertyTree.get_child(L"Datasources"));

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
    }

	void DataTransformMapping::ExportToPropertyTree(boost::property_tree::wptree& filePropertyTree) const {

		boost::property_tree::wptree& dataTransformPropertyTreeRoot = filePropertyTree.add(L"Transform", L"");
		dataTransformPropertyTreeRoot.put(L"<xmlattr>.id", m_id);

		boost::property_tree::wptree& baseObjectsPropertyTree = dataTransformPropertyTreeRoot.add(L"BaseObjects", L"");
		for (const BaseImage& baseObject : m_baseObjects) {

			baseObject.ExportToPropertyTree(baseObjectsPropertyTree);
		}

		m_datasources.ExportToPropertyTree(dataTransformPropertyTreeRoot);

		boost::property_tree::wptree& glyphTreesPropertyTree = dataTransformPropertyTreeRoot.add(L"Glyphs", L"");
		for (auto glyphTree : m_glyphTrees) {
			
			boost::property_tree::wptree& glyphPropertyTree = glyphTree.second->ExportToPropertyTree(glyphTreesPropertyTree);
			glyphPropertyTree.put(L"<xmlattr>.id", glyphTree.first);
		}

		m_defaults.ExportToPropertyTree(dataTransformPropertyTreeRoot);
		m_sceneProperties.ExportToPropertyTree(dataTransformPropertyTreeRoot);
    }

	const DatasourceMaps& DataTransformMapping::GetDatasources() const {

		return m_datasources;
	}

	DatasourceMaps DataTransformMapping::GetDatasourcesInUse() const {

		DatasourceMaps datasourceMaps;

		std::unordered_map<boost::uuids::uuid, Datasource::TableSet, SynGlyphX::UUIDHash> datasourcesAndTablesInUse;
		for (auto glyphTree : m_glyphTrees) {

			const InputField& inputField = glyphTree.second->GetInputFields().begin()->second;
			datasourcesAndTablesInUse[inputField.GetDatasourceID()].insert(inputField.GetTable());
		}

		for (auto datasourceTables : datasourcesAndTablesInUse) {

			FileDatasource datasource = m_datasources.GetFileDatasources().at(datasourceTables.first);
			datasource.ClearTables();
			datasourceMaps.AddFileDatasource(datasourceTables.first, datasource);
			datasourceMaps.EnableTables(datasourceTables.first, datasourceTables.second);
		}

		return datasourceMaps;
	}

	void DataTransformMapping::Clear() {

		Clear(true);
	}
		
	void DataTransformMapping::Clear(bool addADefaultBaseObjectAfterClear) {

		m_datasources.Clear();
		m_glyphTrees.clear();
		m_defaults.Clear();
		m_baseObjects.clear();
		m_id = UUIDGenerator::GetNewRandomUUID();

		if (addADefaultBaseObjectAfterClear) {

			m_baseObjects.push_back(BaseImage(nullptr));
		}
    }

	void DataTransformMapping::RemoveDatasource(const boost::uuids::uuid& id) {

		try {
			m_datasources.RemoveDatasource(id);
		}
		catch (const std::invalid_argument& e) {
			throw;
		}
	}

	boost::uuids::uuid DataTransformMapping::AddFileDatasource(FileDatasource::SourceType type,
		const std::wstring& name,
        const std::wstring& host,
        unsigned int port,
        const std::wstring& username,
        const std::wstring& password) {

		boost::uuids::uuid id = m_datasources.AddFileDatasource(type, name, host, port, username, password);

        return id;
    }

	void DataTransformMapping::EnableTables(const boost::uuids::uuid& id, const Datasource::TableSet& tables, bool enable) {

		m_datasources.EnableTables(id, tables, enable);
    }

	boost::uuids::uuid DataTransformMapping::AddGlyphTree(const DataMappingGlyphGraph::SharedPtr glyphTree) {

		boost::uuids::uuid id = UUIDGenerator::GetNewRandomUUID();

		m_glyphTrees.insert(std::pair<boost::uuids::uuid, DataMappingGlyphGraph::SharedPtr>(id, glyphTree));

		return id;
	}

	const DataTransformMapping::DataMappingGlyphGraphMap& DataTransformMapping::GetGlyphGraphs() const {

		return m_glyphTrees;
	}

	bool DataTransformMapping::IsTransformable() const {

		return (m_datasources.HasDatasources() && DoesAtLeastOneGlyphGraphHaveBindingsOnPosition());
	}

	bool DataTransformMapping::DoesAtLeastOneGlyphGraphHaveBindingsOnPosition() const {

		unsigned short glyphsWithPositionBindings = 0;
		for (auto glyphGraphPair : m_glyphTrees) {

			DataMappingGlyphGraph::SharedPtr glyphGraph = glyphGraphPair.second;
			if (glyphGraph->GetInputFields().empty()) {

				continue;
			}

			if (!glyphGraph->GetRoot()->IsAnInputFieldBoundToAPosition()) {

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
	
	void DataTransformMapping::SetInputField(const boost::uuids::uuid& treeID, DataMappingGlyphGraph::ConstGlyphIterator& node, DataMappingGlyph::MappableField field, const InputField& inputfield) {

		DataMappingGlyphGraph::SharedPtr glyphTree = m_glyphTrees[treeID];
		glyphTree->SetInputField(node, field, inputfield);
	}

	void DataTransformMapping::ClearInputBinding(const boost::uuids::uuid& treeID, DataMappingGlyphGraph::ConstGlyphIterator& node, DataMappingGlyph::MappableField field) {

		DataMappingGlyphGraph::SharedPtr glyphTree = m_glyphTrees[treeID];
		glyphTree->ClearInputBinding(node, field);
	}

	const boost::uuids::uuid& DataTransformMapping::GetID() const {

		return m_id;
	}

	void DataTransformMapping::ResetID() {

		m_id = UUIDGenerator::GetNewRandomUUID();
	}

	void DataTransformMapping::UpdateDatasourceName(const boost::uuids::uuid& id, const std::wstring& name) {

		try {

			m_datasources.ChangeDatasourceName(id, name);
		}
		catch (const std::invalid_argument& e) {

			throw;
		}
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

			newPosition = m_glyphTrees[treeId]->GetChild(parent, startingNumberOfChildren - 1)->GetMinGlyph().GetPosition();
		}

		for (int i = 0; i < numberOfChildren; ++i) {

			SynGlyphX::DataMappingGlyphGraph::GlyphIterator newChildGlyph = m_glyphTrees[treeId]->AddChildGlyph(parent, glyphTemplate);

			//For now, update position to 15.0 less than the last x coordinate.  This follows what ANTz does
			newPosition[0] -= 15.0;
			newChildGlyph->GetPosition()[0].GetValue().SetMinDiff(newPosition[0], 0.0);
			newChildGlyph->GetPosition()[1].GetValue().SetMinDiff(newPosition[1], 0.0);
			newChildGlyph->GetPosition()[2].GetValue().SetMinDiff(newPosition[2], 0.0);
		}
	}

	void DataTransformMapping::AddChildTree(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::GlyphIterator& parent, const stlplus::ntree<SynGlyphX::DataMappingGlyph>& glyphGraph) {

		if (!parent.valid()) {

			throw std::invalid_argument("Can't append children to invalid parent");
		}

		unsigned int startingNumberOfChildren = m_glyphTrees[treeId]->ChildCount(parent.constify());
		SynGlyphX::Vector3 newPosition = { { 15.0, 0.0, 0.0 } };
		if (startingNumberOfChildren > 0) {

			newPosition = m_glyphTrees[treeId]->GetChild(parent, startingNumberOfChildren - 1)->GetMinGlyph().GetPosition();
			//For now, update position to 15.0 less than the last x coordinate.  This follows what ANTz does
			newPosition[0] -= 15.0;
		}

		SynGlyphX::DataMappingGlyphGraph::GlyphIterator newChildGlyph = m_glyphTrees[treeId]->AddChildGlyphGraph(parent, glyphGraph);
		newChildGlyph->GetPosition()[0].GetValue().SetMinDiff(newPosition[0], 0.0);
		newChildGlyph->GetPosition()[1].GetValue().SetMinDiff(newPosition[1], 0.0);
		newChildGlyph->GetPosition()[2].GetValue().SetMinDiff(newPosition[2], 0.0);
	}

	void DataTransformMapping::RemoveGlyph(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::ConstGlyphIterator& parent, int child) {

		m_glyphTrees[treeId]->RemoveChild(parent.deconstify(), child);
	}

	const SceneProperties& DataTransformMapping::GetSceneProperties() const {

		return m_sceneProperties;
	}

	void DataTransformMapping::SetSceneProperties(const SceneProperties& sceneProperties) {

		m_sceneProperties = sceneProperties;
	}

} //namespace SynGlyphX