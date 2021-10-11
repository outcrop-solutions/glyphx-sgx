#include "DatabaseServerDataSource.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/property_tree/ptree.hpp>
#include "DataSourceTable.h"

namespace GlyphEngine {

	const DatabaseServerDataSource::DBTypeBimap DatabaseServerDataSource::s_dbTypeStrings = boost::assign::list_of < DatabaseServerDataSource::DBTypeBimap::relation >
		(DatabaseServerDataSource::DBType::MySQL, L"MySQL")
		(DatabaseServerDataSource::DBType::Vertica, L"Vertica");

	const DatabaseServerDataSource::DBTypeBimap DatabaseServerDataSource::s_dbTypePrefixes = boost::assign::list_of < DatabaseServerDataSource::DBTypeBimap::relation >
		(DatabaseServerDataSource::DBType::MySQL, L"mysql")
		(DatabaseServerDataSource::DBType::Vertica, L"vertica");

	const std::wstring DatabaseServerDataSource::s_prefixSeparator = L"://";

	DatabaseServerDataSource::DatabaseServerDataSource(DBType type, const std::wstring& connection, const std::wstring& schema, const std::wstring& username, const std::wstring& password) :
		DataSource(connection, username, password),
        m_dbType(type),
        m_schema(schema)
	{
	}

	DatabaseServerDataSource::DatabaseServerDataSource(const PropertyTree& propertyTree) :
		DataSource(propertyTree),
		m_dbType(s_dbTypeStrings.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.type"))),
		m_schema(propertyTree.get_optional<std::wstring>(L"schema").get_value_or(L"")) {

		
	}

	DatabaseServerDataSource::DatabaseServerDataSource(const DatabaseServerDataSource& datasource) :
		DataSource(datasource),
		m_dbType(datasource.m_dbType),
		m_schema(datasource.m_schema) {


	}

	DatabaseServerDataSource::~DatabaseServerDataSource()
	{
	}

	DatabaseServerDataSource& DatabaseServerDataSource::operator=(const DatabaseServerDataSource& datasource) {

		DataSource::operator=(datasource);
		m_dbType = datasource.m_dbType;
		m_schema = datasource.m_schema;

		return *this;
	}

	bool DatabaseServerDataSource::operator==(const DatabaseServerDataSource& datasource) const {

		if (DataSource::operator!=(datasource)) {

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

	bool DatabaseServerDataSource::operator!=(const DatabaseServerDataSource& datasource) const {

		return !operator==(datasource);
	}

	DataSource::SourceType DatabaseServerDataSource::GetSourceType() const {

		return DataSource::SourceType::DatabaseServer;
	}

	DatabaseServerDataSource::DBType DatabaseServerDataSource::GetDBType() const {

		return m_dbType;
	}

	const std::wstring& DatabaseServerDataSource::GetSchema() const {

		return m_schema;
	}

	bool DatabaseServerDataSource::IsOriginalDataSourceADatabase() const {

		return true;
	}

	bool DatabaseServerDataSource::CanDataSourceHaveMultipleTables() const {

		return true;
	}

	bool DatabaseServerDataSource::IsFile() const {

		return false;
	}

	bool DatabaseServerDataSource::CanDataSourceBeFound() const {

		//May want a test here to see if RDBMS can be accessed
		return true;
	}

	DatabaseServerDataSource::PropertyTree& DatabaseServerDataSource::ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree) {

		PropertyTree& propertyTree = DataSource::ExportToPropertyTree(parentPropertyTree);
		propertyTree.put(L"<xmlattr>.type", s_dbTypeStrings.left.at(m_dbType));
		if (!m_schema.empty()) {

			propertyTree.put(L"schema", m_schema);
		}

		return propertyTree;
	}

	std::wstring DatabaseServerDataSource::GetFormattedName() const {

		if (m_schema.empty()) {

			return m_host;
		}
		else {

			return m_host + L":" + m_schema;
		}
	}

	std::wstring DatabaseServerDataSource::GetDBName() const {

		return m_host;
	}

	std::wstring DatabaseServerDataSource::GetFullJDBCConnectionString() const {

		return GetHost();
	}

} //namespace SynGlyphX
