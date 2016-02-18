#include "rdbmsdatasource.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>

namespace SynGlyphX {

	const RDBMSDatasource::DBTypeBimap RDBMSDatasource::s_dbTypeStrings = boost::assign::list_of < RDBMSDatasource::DBTypeBimap::relation >
		(RDBMSDatasource::DBType::MySQL, L"MySQL")
		(RDBMSDatasource::DBType::Vertica, L"Verica");

	RDBMSDatasource::RDBMSDatasource(DBType type, const std::wstring& connection, const std::wstring& schema, const std::wstring& username, const std::wstring& password) :
		Datasource(connection, username, password),
		m_schema(schema),
		m_dbType(type)
	{
	}

	RDBMSDatasource::RDBMSDatasource(const PropertyTree& propertyTree) :
		Datasource(propertyTree),
		m_dbType(s_dbTypeStrings.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.type"))),
		m_schema(propertyTree.get_optional<std::wstring>(L"schema").get_value_or(L"")) {

		
	}

	RDBMSDatasource::RDBMSDatasource(const RDBMSDatasource& datasource) :
		Datasource(datasource),
		m_dbType(datasource.m_dbType) {


	}

	RDBMSDatasource::~RDBMSDatasource()
	{
	}

	RDBMSDatasource& RDBMSDatasource::operator=(const RDBMSDatasource& datasource) {

		Datasource::operator=(datasource);
		m_dbType = datasource.m_dbType;
		m_schema = datasource.m_schema;

		return *this;
	}

	bool RDBMSDatasource::operator==(const RDBMSDatasource& datasource) const {

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

	bool RDBMSDatasource::operator!=(const RDBMSDatasource& datasource) const {

		return !operator==(datasource);
	}

	Datasource::SourceType RDBMSDatasource::GetSourceType() const {

		return Datasource::SourceType::RDBMS;
	}

	RDBMSDatasource::DBType RDBMSDatasource::GetDBType() const {

		return m_dbType;
	}

	const std::wstring& RDBMSDatasource::GetSchema() const {

		return m_schema;
	}

	bool RDBMSDatasource::IsOriginalDatasourceADatabase() const {

		return true;
	}

	bool RDBMSDatasource::CanDatasourceHaveMultipleTables() const {

		return true;
	}

	bool RDBMSDatasource::IsFile() const {

		return false;
	}

	bool RDBMSDatasource::CanDatasourceBeFound() const {

		//May want a test here to see if RDBMS can be accessed
		return true;
	}

	RDBMSDatasource::PropertyTree& RDBMSDatasource::ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree) {

		PropertyTree& propertyTree = Datasource::ExportToPropertyTree(parentPropertyTree);
		propertyTree.put(L"<xmlattr>.type", s_dbTypeStrings.left.at(m_dbType));
		if (!m_schema.empty()) {

			propertyTree.put(L"schema", m_schema);
		}

		return propertyTree;
	}

	std::wstring RDBMSDatasource::GetFormattedName() const {

		if (m_schema.empty()) {

			return m_host;
		}
		else {

			return m_host + L":" + m_schema;
		}
	}

	std::wstring RDBMSDatasource::GetDBName() const {

		return m_host;
	}

} //namespace SynGlyphX