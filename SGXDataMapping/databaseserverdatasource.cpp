#include "databaseserverdatasource.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>

namespace SynGlyphX {

	const DatabaseServerDatasource::DBTypeBimap DatabaseServerDatasource::s_dbTypeStrings = boost::assign::list_of < DatabaseServerDatasource::DBTypeBimap::relation >
		(DatabaseServerDatasource::DBType::MySQL, L"MySQL")
		(DatabaseServerDatasource::DBType::Vertica, L"Verica");

	DatabaseServerDatasource::DatabaseServerDatasource(DBType type, const std::wstring& connection, const std::wstring& schema, const std::wstring& username, const std::wstring& password) :
		Datasource(connection, username, password),
		m_schema(schema),
		m_dbType(type)
	{
	}

	DatabaseServerDatasource::DatabaseServerDatasource(const PropertyTree& propertyTree) :
		Datasource(propertyTree),
		m_dbType(s_dbTypeStrings.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.type"))),
		m_schema(propertyTree.get_optional<std::wstring>(L"schema").get_value_or(L"")) {

		
	}

	DatabaseServerDatasource::DatabaseServerDatasource(const DatabaseServerDatasource& datasource) :
		Datasource(datasource),
		m_dbType(datasource.m_dbType) {


	}

	DatabaseServerDatasource::~DatabaseServerDatasource()
	{
	}

	DatabaseServerDatasource& DatabaseServerDatasource::operator=(const DatabaseServerDatasource& datasource) {

		Datasource::operator=(datasource);
		m_dbType = datasource.m_dbType;
		m_schema = datasource.m_schema;

		return *this;
	}

	bool DatabaseServerDatasource::operator==(const DatabaseServerDatasource& datasource) const {

		if (Datasource::operator!=(datasource)) {

			return false;
		}

		if (m_dbType != datasource.m_dbType) {

			return false;
		}

		if (m_schema != datasource.m_schema) {

			return false;
		}

		return true;
	}

	bool DatabaseServerDatasource::operator!=(const DatabaseServerDatasource& datasource) const {

		return !operator==(datasource);
	}

	Datasource::SourceType DatabaseServerDatasource::GetSourceType() const {

		return Datasource::SourceType::DatabaseServer;
	}

	DatabaseServerDatasource::DBType DatabaseServerDatasource::GetDBType() const {

		return m_dbType;
	}

	const std::wstring& DatabaseServerDatasource::GetSchema() const {

		return m_schema;
	}

	bool DatabaseServerDatasource::IsOriginalDatasourceADatabase() const {

		return true;
	}

	bool DatabaseServerDatasource::CanDatasourceHaveMultipleTables() const {

		return true;
	}

	bool DatabaseServerDatasource::IsFile() const {

		return false;
	}

	bool DatabaseServerDatasource::CanDatasourceBeFound() const {

		//May want a test here to see if RDBMS can be accessed
		return true;
	}

	DatabaseServerDatasource::PropertyTree& DatabaseServerDatasource::ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree) {

		PropertyTree& propertyTree = Datasource::ExportToPropertyTree(parentPropertyTree);
		propertyTree.put(L"<xmlattr>.type", s_dbTypeStrings.left.at(m_dbType));
		if (!m_schema.empty()) {

			propertyTree.put(L"schema", m_schema);
		}

		return propertyTree;
	}

	std::wstring DatabaseServerDatasource::GetFormattedName() const {

		if (m_schema.empty()) {

			return m_host;
		}
		else {

			return m_host + L":" + m_schema;
		}
	}

	std::wstring DatabaseServerDatasource::GetDBName() const {

		return m_host;
	}

} //namespace SynGlyphX