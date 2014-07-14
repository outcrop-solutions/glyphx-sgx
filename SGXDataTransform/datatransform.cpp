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

				Datasource datasource(datasourceValue.second.get<std::wstring>(L"Name"), 
					datasourceValue.second.get<std::wstring>(L"Type"),
					datasourceValue.second.get<std::wstring>(L"Host"),
					datasourceValue.second.get<unsigned int>(L"Port"),
					datasourceValue.second.get<std::wstring>(L"Username"),
					datasourceValue.second.get<std::wstring>(L"Password"));

                std::vector<std::wstring> tables;
				for (boost::property_tree::wptree::value_type& tableValue : datasourceValue.second.get_child(L"Tables")) {

					if (tableValue.first == L"Table") {
                        tables.push_back(tableValue.second.data());
					}
				}
                datasource.AddTables(tables);

				m_datasources.insert(std::pair<std::wstring, Datasource>(datasourceValue.second.get<std::wstring>(L"<xmlattr>.id"), datasource));
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
			datasourcePropertyTree.put(L"Type", datasource.second.GetType());
			datasourcePropertyTree.put(L"Host", datasource.second.GetHost());
			datasourcePropertyTree.put(L"Port", datasource.second.GetPort());
			datasourcePropertyTree.put(L"Username", datasource.second.GetUsername());
			datasourcePropertyTree.put(L"Password", datasource.second.GetPassword());

			boost::property_tree::wptree& tablesPropertyTree = datasourcePropertyTree.add(L"Tables", L"");
			for (const std::wstring& table : datasource.second.GetTables()) {
				tablesPropertyTree.add(L"Table", table);
			}
		}
	}

	const std::unordered_map<std::wstring, Datasource>& DataTransform::GetDatasources() const {

        return m_datasources;
    }

    void DataTransform::Clear() {

        m_datasources.clear();
    }

    std::wstring DataTransform::AddDatasource(const std::wstring& name,
        const std::wstring& type,
        const std::wstring& host,
        const unsigned int port,
        const std::wstring& username,
        const std::wstring& password) {

        std::wstring id = boost::uuids::to_wstring(m_uuidGenerator());

        Datasource datasource(name, type, host, port, username, password);

        m_datasources.insert(std::pair<std::wstring, Datasource>(id, datasource));

        return id;
    }

    void DataTransform::AddTables(const std::wstring& id, const std::vector<std::wstring>& tables) {

        m_datasources.at(id).AddTables(tables);
    }

} //namespace SynGlyphX