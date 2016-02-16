#include "rdbmsdatasource.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>

namespace SynGlyphX {

	const RDBMSDatasource::DBTypeBimap RDBMSDatasource::s_dbTypeStrings = boost::assign::list_of < RDBMSDatasource::DBTypeBimap::relation >
		(RDBMSDatasource::DBType::MySQL, L"MqSQL")
		(RDBMSDatasource::DBType::Vertica, L"Verica");

	RDBMSDatasource::RDBMSDatasource(DBType type, const std::wstring& connection, const std::wstring& host, unsigned int port, const std::wstring& username, const std::wstring& password) :
		Datasource(connection, host, port, username, password),
		m_dbType(type)
	{
	}

	RDBMSDatasource::RDBMSDatasource(const PropertyTree& propertyTree) :
		Datasource(propertyTree),
		m_dbType(s_dbTypeStrings.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.type"))) {

		
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

		return *this;
	}

	bool RDBMSDatasource::operator==(const RDBMSDatasource& datasource) const {

		if (Datasource::operator!=(datasource)) {

			return false;
		}

		if (m_dbType != datasource.m_dbType) {

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

		return propertyTree;
	}

	const std::wstring& RDBMSDatasource::GetFormattedName() const {

		return GetDBName();
	}

} //namespace SynGlyphX