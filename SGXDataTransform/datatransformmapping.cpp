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
		m_baseImage(nullptr)
    {
		m_id = UUIDGenerator::GetNewRandomUUID();
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

		return true;
	}

	bool DataTransformMapping::operator!=(const DataTransformMapping& mapping) const {

		return !operator==(mapping);
	}

	void DataTransformMapping::ReadFromFile(const std::string& filename) {

        Clear();

        boost::property_tree::wptree filePropertyTree;
		boost::property_tree::read_xml(filename, filePropertyTree);

		boost::property_tree::wptree& dataTransformPropertyTree = filePropertyTree.get_child(L"Transform");

		m_id = dataTransformPropertyTree.get<boost::uuids::uuid>(L"<xmlattr>.id");

		m_baseImage = BaseImage(dataTransformPropertyTree.get_child(L"BaseImage"));

		m_datasources = DatasourceMaps(dataTransformPropertyTree.get_child(L"Datasources"));

		for (boost::property_tree::wptree::value_type& glyphPropertyTree : dataTransformPropertyTree.get_child(L"Glyphs")) {

			if (glyphPropertyTree.first == L"Glyph") {

				MinMaxGlyphTree::SharedPtr glyphTree(new MinMaxGlyphTree(glyphPropertyTree.second));
				m_glyphTrees.insert(std::pair<boost::uuids::uuid, MinMaxGlyphTree::SharedPtr>(glyphPropertyTree.second.get<boost::uuids::uuid>(L"<xmlattr>.id"), glyphTree));
			}
		}
    }

	void DataTransformMapping::WriteToFile(const std::string& filename) const {

		boost::property_tree::wptree filePropertyTree;

		boost::property_tree::wptree& dataTransformPropertyTreeRoot = filePropertyTree.add(L"Transform", L"");
		dataTransformPropertyTreeRoot.put(L"<xmlattr>.id", m_id);

		m_baseImage.ExportToPropertyTree(dataTransformPropertyTreeRoot);

		m_datasources.ExportToPropertyTree(dataTransformPropertyTreeRoot);

		boost::property_tree::wptree& glyphTreesPropertyTree = dataTransformPropertyTreeRoot.add(L"Glyphs", L"");
		for (auto glyphTree : m_glyphTrees) {
			
			boost::property_tree::wptree& glyphPropertyTree = glyphTree.second->ExportToPropertyTree(glyphTreesPropertyTree);
			glyphPropertyTree.put(L"<xmlattr>.id", glyphTree.first);
		}

		boost::property_tree::write_xml(filename, filePropertyTree);
    }

	const DatasourceMaps& DataTransformMapping::GetDatasources() const {

		return m_datasources;
	}

	void DataTransformMapping::Clear() {

		m_datasources.Clear();
		m_glyphTrees.clear();
		m_baseImage = BaseImage(nullptr);
    }

	void DataTransformMapping::RemoveDatasource(const boost::uuids::uuid& id) {

		try {
			RemoveDatasource(id);
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

	void DataTransformMapping::SetBaseImage(const BaseImage& baseImage) {

		m_baseImage = baseImage;
	}

	const BaseImage& DataTransformMapping::GetBaseImage() const {

		return m_baseImage;
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

	void DataTransformMapping::UpdateDatasourceName(const boost::uuids::uuid& id, const std::wstring& name) {

		try {

			m_datasources.ChangeDatasourceName(id, name);
		}
		catch (const std::invalid_argument& e) {

			throw;
		}
	}

} //namespace SynGlyphX