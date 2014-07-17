#include "datatransform.h"
#include <boost/property_tree/xml_parser.hpp>
#include <algorithm>
#include <boost/uuid/uuid_io.hpp>

namespace SynGlyphX {

    DataTransform::DataTransform()
    {
    }

	DataTransform::DataTransform(const GlyphTree& glyphTree) {


	}

    DataTransform::~DataTransform()
    {
    }

    void DataTransform::ReadFromFile(const std::string& filename) {

        Clear();

        boost::property_tree::wptree dataTransformPropertyTree;
		boost::property_tree::read_xml(filename, dataTransformPropertyTree);

		CreateDatasourcesFromPropertyTree(dataTransformPropertyTree);
    }

	void DataTransform::CreateDatasourcesFromPropertyTree(boost::property_tree::wptree& propertyTree) {

		for (boost::property_tree::wptree::value_type& datasourceValue : propertyTree.get_child(L"Datasources")) {

			if (datasourceValue.first == L"Datasource") {

				Datasource datasource(datasourceValue.second.get<std::wstring>(L"Name"),
					Datasource::s_sourceTypeStrings.right.at(datasourceValue.second.get<std::wstring>(L"<xmlattr>.type")),
					datasourceValue.second.get<std::wstring>(L"Host"),
					datasourceValue.second.get<unsigned int>(L"Port", 0),
					datasourceValue.second.get<std::wstring>(L"Username", L""),
					datasourceValue.second.get<std::wstring>(L"Password", L""));

				boost::optional<boost::property_tree::wptree&> tablePropertyTree = datasourceValue.second.get_child_optional(L"Tables");

				if (tablePropertyTree.is_initialized()) {
					std::vector<std::wstring> tables;
					for (boost::property_tree::wptree::value_type& tableValue : tablePropertyTree.get()) {

						if (tableValue.first == L"Table") {
							tables.push_back(tableValue.second.data());
						}
					}
					datasource.AddTables(tables);
				}

				m_datasources.insert(std::pair<boost::uuids::uuid, Datasource>(datasourceValue.second.get<boost::uuids::uuid>(L"<xmlattr>.id"), datasource));
			}
		}
	}

    void DataTransform::WriteToFile(const std::string& filename) const {

		boost::property_tree::wptree dataTransformPropertyTreeRoot;

		AddDatasourcesToPropertyTree(dataTransformPropertyTreeRoot);

		boost::property_tree::write_xml(filename, dataTransformPropertyTreeRoot);
    }

	void DataTransform::AddDatasourcesToPropertyTree(boost::property_tree::wptree& propertyTreeRoot) const {

		boost::property_tree::wptree& datasourcesPropertyTree = propertyTreeRoot.add(L"Datasources", L"");
		for (auto datasource : m_datasources) {

			boost::property_tree::wptree& datasourcePropertyTree = datasourcesPropertyTree.add(L"Datasource", L"");

			datasourcePropertyTree.put(L"<xmlattr>.id", datasource.first);
			datasourcePropertyTree.put(L"Name", datasource.second.GetDBName());
			datasourcePropertyTree.put(L"<xmlattr>.type", Datasource::s_sourceTypeStrings.left.at(datasource.second.GetType()));
			datasourcePropertyTree.put(L"Host", datasource.second.GetHost());

			unsigned int port = datasource.second.GetPort();
			if (port != 0) {
				datasourcePropertyTree.put(L"Port", port);
			}
			
			const std::wstring& username = datasource.second.GetUsername();
			if (!username.empty()) {
				datasourcePropertyTree.put(L"Username", username);
			}

			const std::wstring& password = datasource.second.GetPassword();
			if (!password.empty()) {
				datasourcePropertyTree.put(L"Password", password);
			}

			const SynGlyphX::WStringVector& tables = datasource.second.GetTables();
			if (!tables.empty()) {
				boost::property_tree::wptree& tablesPropertyTree = datasourcePropertyTree.add(L"Tables", L"");
				for (const std::wstring& table : tables) {
					tablesPropertyTree.add(L"Table", table);
				}
			}
		}
	}

	const DataTransform::DatasourceMap& DataTransform::GetDatasources() const {

        return m_datasources;
    }

    void DataTransform::Clear() {

        m_datasources.clear();
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

} //namespace SynGlyphX