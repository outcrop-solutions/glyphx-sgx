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

		if (m_glyphTrees != mapping.m_glyphTrees) {

			return false;
		}

		if (m_baseObjects != mapping.m_baseObjects) {

			return false;
		}

		if (m_defaults != mapping.m_defaults) {

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

				MinMaxGlyphTree::SharedPtr glyphTree(new MinMaxGlyphTree(glyphPropertyTree.second));
				m_glyphTrees.insert(std::pair<boost::uuids::uuid, MinMaxGlyphTree::SharedPtr>(glyphPropertyTree.second.get<boost::uuids::uuid>(L"<xmlattr>.id"), glyphTree));
			}
		}

		boost::optional<const boost::property_tree::wptree&> defaultsPropertyTree = dataTransformPropertyTree.get_child_optional(L"Defaults");
		if (defaultsPropertyTree.is_initialized()) {

			m_defaults = DataMappingDefaults(defaultsPropertyTree.get());
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
    }

	const DatasourceMaps& DataTransformMapping::GetDatasources() const {

		return m_datasources;
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

	boost::uuids::uuid DataTransformMapping::AddGlyphTree(const MinMaxGlyphTree::SharedPtr glyphTree) {

		boost::uuids::uuid id = UUIDGenerator::GetNewRandomUUID();

		m_glyphTrees.insert(std::pair<boost::uuids::uuid, MinMaxGlyphTree::SharedPtr>(id, glyphTree));

		return id;
	}

	const DataTransformMapping::MinMaxGlyphTreeMap& DataTransformMapping::GetGlyphTrees() const {

		return m_glyphTrees;
	}

	bool DataTransformMapping::IsTransformable() const {

		return (m_datasources.HasDatasources() && (!m_glyphTrees.empty()));
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
	
	void DataTransformMapping::SetInputField(const boost::uuids::uuid& treeID, MinMaxGlyphTree::const_iterator& node, int index, const InputField& inputfield) {

		MinMaxGlyphTree::SharedPtr glyphTree = m_glyphTrees[treeID];
		glyphTree->SetInputField(node, index, inputfield);
	}

	void DataTransformMapping::ClearInputBinding(const boost::uuids::uuid& treeID, MinMaxGlyphTree::const_iterator& node, int index) {

		MinMaxGlyphTree::SharedPtr glyphTree = m_glyphTrees[treeID];
		glyphTree->ClearInputBinding(node, index);
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

} //namespace SynGlyphX