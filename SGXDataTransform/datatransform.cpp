#include "datatransform.h"
#include <boost/property_tree/xml_parser.hpp>
#include <algorithm>
#include <boost/uuid/uuid_io.hpp>

namespace SynGlyphX {

    DataTransform::DataTransform()
    {
    }

    DataTransform::~DataTransform()
    {
    }

    void DataTransform::ReadFromFile(const std::string& filename) {

        Clear();

        boost::property_tree::wptree dataTransformPropertyTree;
		boost::property_tree::read_xml(filename, dataTransformPropertyTree);

		for (boost::property_tree::wptree::value_type& datasourceValue : dataTransformPropertyTree.get_child(L"Datasources")) {

			if (datasourceValue.first == L"Datasource") {

				Datasource datasource(datasourceValue.second);
				m_datasources.insert(std::pair<boost::uuids::uuid, Datasource>(datasourceValue.second.get<boost::uuids::uuid>(L"<xmlattr>.id"), datasource));
			}
		}

		for (boost::property_tree::wptree::value_type& glyphPropertyTree : dataTransformPropertyTree.get_child(L"Glyphs")) {

			if (glyphPropertyTree.first == L"Glyph") {

				MinMaxGlyphTree::SharedPtr glyphTree(new MinMaxGlyphTree(glyphPropertyTree.second));
				m_glyphTrees.insert(std::pair<boost::uuids::uuid, MinMaxGlyphTree::SharedPtr>(glyphPropertyTree.second.get<boost::uuids::uuid>(L"<xmlattr>.id"), glyphTree));
			}
		}
    }

    void DataTransform::WriteToFile(const std::string& filename) const {

		boost::property_tree::wptree dataTransformPropertyTreeRoot;

		boost::property_tree::wptree& datasourcesPropertyTree = dataTransformPropertyTreeRoot.add(L"Datasources", L"");
		for (auto datasource : m_datasources) {

			Datasource::PropertyTree& datasourcePropertyTree = datasource.second.ExportToPropertyTree(datasourcesPropertyTree);
			datasourcePropertyTree.put(L"<xmlattr>.id", datasource.first);
		}

		boost::property_tree::wptree& glyphTreesPropertyTree = dataTransformPropertyTreeRoot.add(L"Glyphs", L"");
		for (auto glyphTree : m_glyphTrees) {
			
			boost::property_tree::wptree& glyphPropertyTree = glyphTree.second->ExportToPropertyTree(glyphTreesPropertyTree);
			glyphPropertyTree.put(L"<xmlattr>.id", glyphTree.first);
		}

		boost::property_tree::write_xml(filename, dataTransformPropertyTreeRoot);
    }

	const DataTransform::DatasourceMap& DataTransform::GetDatasources() const {

        return m_datasources;
    }

    void DataTransform::Clear() {

        m_datasources.clear();
		m_glyphTrees.clear();
    }

	boost::uuids::uuid DataTransform::AddDatasource(const std::wstring& name,
        Datasource::SourceType type,
        const std::wstring& host,
        unsigned int port,
        const std::wstring& username,
        const std::wstring& password) {

		boost::uuids::uuid id = m_uuidGenerator();

        Datasource datasource(name, type, host, port, username, password);

		m_datasources.insert(std::pair<boost::uuids::uuid, Datasource>(id, datasource));

        return id;
    }

	void DataTransform::AddTables(const boost::uuids::uuid& id, const std::vector<std::wstring>& tables) {

        m_datasources.at(id).AddTables(tables);
    }

	boost::uuids::uuid DataTransform::AddGlyphTree(const MinMaxGlyphTree::SharedPtr glyphTree) {

		boost::uuids::uuid id = m_uuidGenerator();

		m_glyphTrees.insert(std::pair<boost::uuids::uuid, MinMaxGlyphTree::SharedPtr>(id, glyphTree));

		return id;
	}

	const DataTransform::MinMaxGlyphTreeMap& DataTransform::GetGlyphTrees() const {

		return m_glyphTrees;
	}

	bool DataTransform::IsTransformable() const {

		return ((!m_datasources.empty()) && (!m_glyphTrees.empty()));
	}

} //namespace SynGlyphX