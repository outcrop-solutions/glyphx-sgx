#include "datasourcemaps.h"
#include <boost/uuid/uuid_io.hpp>

namespace SynGlyphX {

	DatasourceMaps::DatasourceMaps()
	{
	}

	DatasourceMaps::DatasourceMaps(const DatasourceMaps::PropertyTree& propertyTree) {

		for (const boost::property_tree::wptree::value_type& datasourceValue : propertyTree) {

			//Datasource is an option here because the tag for datasources was "Datasource" before it was broken out by type of File, ODBC, etc.
			//This is for backwards compatibility
			if ((datasourceValue.first == L"File") || (datasourceValue.first == L"Datasource")) {

				FileDatasource datasource(datasourceValue.second);
				m_fileDatasources.insert(std::pair<boost::uuids::uuid, FileDatasource>(datasourceValue.second.get<boost::uuids::uuid>(L"<xmlattr>.id"), datasource));
			}
		}
	}

	DatasourceMaps::~DatasourceMaps()
	{
	}

	bool DatasourceMaps::HasDatasources() const {

		return (!m_fileDatasources.empty());
	}

	void DatasourceMaps::Clear() {

		m_fileDatasources.clear();
	}

	bool DatasourceMaps::AddTables(const boost::uuids::uuid& id, const std::vector<std::wstring>& tables) {

		FileDatasourceMap::iterator fileDatasourceIterator = m_fileDatasources.find(id);
		if (fileDatasourceIterator != m_fileDatasources.end()) {

			fileDatasourceIterator->second.AddTables(tables);
			return true;
		}

		return false;
	}

	void DatasourceMaps::ChangeDatasourceName(const boost::uuids::uuid& id, const std::wstring& name) {

		FileDatasourceMap::iterator fileDatasourceIterator = m_fileDatasources.find(id);
		if (fileDatasourceIterator != m_fileDatasources.end()) {

			fileDatasourceIterator->second.ChangeFilename(name);
			return;
		}

		//If id wasn't found in any datasource throw invalid_argument exception
		throw std::invalid_argument("ID does not exist in datasources for this data transform mapping.");
	}

	DatasourceMaps::PropertyTree& DatasourceMaps::ExportToPropertyTree(PropertyTree& propertyTreeParent) const {

		boost::property_tree::wptree& datasourcesPropertyTree = propertyTreeParent.add(L"Datasources", L"");
		for (auto datasource : m_fileDatasources) {

			Datasource::PropertyTree& datasourcePropertyTree = datasource.second.ExportToPropertyTree(datasourcesPropertyTree);
			datasourcePropertyTree.put(L"<xmlattr>.id", datasource.first);
		}

		return datasourcesPropertyTree;
	}

	void DatasourceMaps::RemoveDatasource(const boost::uuids::uuid& id) {

		FileDatasourceMap::iterator fileIterator = m_fileDatasources.find(id);
		if (fileIterator != m_fileDatasources.end()) {

			m_fileDatasources.erase(fileIterator);
			return;
		}
		
		throw std::invalid_argument("Datasource id not found so no datasource was removed.");
	}

	const DatasourceMaps::FileDatasourceMap& DatasourceMaps::GetFileDatasources() const {

		return m_fileDatasources;
	}

	boost::uuids::uuid DatasourceMaps::AddFileDatasource(FileDatasource::SourceType type,
		const std::wstring& name,
		const std::wstring& host,
		unsigned int port,
		const std::wstring& username,
		const std::wstring& password) {

		boost::uuids::uuid id = UUIDGenerator::GetNewRandomUUID();

		FileDatasource datasource(type, name, host, port, username, password);

		m_fileDatasources.insert(std::pair<boost::uuids::uuid, FileDatasource>(id, datasource));

		return id;
	}

} //namespace SynGlyphX