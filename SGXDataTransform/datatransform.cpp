#include "datatransform.h"
#include <boost/property_tree/xml_parser.hpp>
#include <algorithm>

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

				for (boost::property_tree::wptree::value_type& tableValue : datasourceValue.second.get_child(L"Tables")) {

					if (tableValue.first == L"Table") {
						datasource.AddTable(tableValue.second.data());
					}
				}

				m_datasources.insert(std::pair<std::wstring, Datasource>(datasourceValue.second.get<std::wstring>(L"<xmlattr>.id"), datasource));
			}
		}
    }

    void DataTransform::WriteToFile(const std::string& filename) const {

		boost::property_tree::wptree dataTransformPropertyTree(L"DataTransform");

		boost::property_tree::wptree& datasourcesPropertyTree = dataTransformPropertyTree.add(L"Datasources", L"");
		AddDatasourcesToPropertyTree(datasourcesPropertyTree);

		boost::property_tree::write_xml(filename, dataTransformPropertyTree);
    }

	void DataTransform::AddDatasourcesToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		for (auto datasource : m_datasources) {

			boost::property_tree::wptree& datasourcePropertyTree = propertyTree.add(L"Datasource", L"");

			datasourcePropertyTree.put(L"<xmlattr>.id", datasource.first);
			datasourcePropertyTree.put(L"Name", datasource.second.GetDBName());
			datasourcePropertyTree.put(L"Type", datasource.second.GetType());
			datasourcePropertyTree.put(L"Host", datasource.second.GetHost());
			datasourcePropertyTree.put(L"Port", datasource.second.GetPort());
			datasourcePropertyTree.put(L"Username", datasource.second.GetUsername());
			datasourcePropertyTree.put(L"Password", datasource.second.GetPassword());

			boost::property_tree::wptree& tablesPropertyTree = datasourcePropertyTree.add(L"Tables", L"");
			for (const std::wstring& table : datasource.second.GetTables()) {
				tablesPropertyTree.put(L"Table", table);
			}
		}
	}

    const std::map<std::wstring, Datasource>& DataTransform::GetDatasources() const {

        return m_datasources;
    }

    void DataTransform::Clear() {

        m_datasources.clear();
    }

} //namespace SynGlyphX