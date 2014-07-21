#include "datasource.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>

namespace SynGlyphX {

	const Datasource::SourceTypeBimap Datasource::s_sourceTypeStrings = boost::assign::list_of < Datasource::SourceTypeBimap::relation >
		( Datasource::SourceType::SQLITE3, L"SQLITE3" )
		( Datasource::SourceType::ODBC, L"ODBC" )
		( Datasource::SourceType::CSV, L"CSV")
		( Datasource::SourceType::KML, L"KML/KMZ");

	Datasource::Datasource(const std::wstring& dbName, SourceType type, const std::wstring& host, unsigned int port, const std::wstring& username, const std::wstring& password) :
        m_dbName(dbName),
        m_type(type),
        m_host(host),
        m_port(port),
        m_username(username),
        m_password(password)
	{
	}

	Datasource::Datasource(const PropertyTree& propertyTree) :
		m_dbName(propertyTree.get<std::wstring>(L"Name")),
		m_type(s_sourceTypeStrings.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.type"))),
		m_host(propertyTree.get<std::wstring>(L"Host")),
		m_port(propertyTree.get<unsigned int>(L"Port", 0)),
		m_username(propertyTree.get<std::wstring>(L"Username", L"")),
		m_password(propertyTree.get<std::wstring>(L"Password", L"")) {

		boost::optional<const boost::property_tree::wptree&> tablePropertyTree = propertyTree.get_child_optional(L"Tables");

		if (tablePropertyTree.is_initialized()) {

			std::vector<std::wstring> tables;
			for (const boost::property_tree::wptree::value_type& tableValue : tablePropertyTree.get()) {

				if (tableValue.first == L"Table") {
					tables.push_back(tableValue.second.data());
				}
			}
			AddTables(tables);
		}
	}

    Datasource::Datasource(const Datasource& datasource) :
        m_dbName(datasource.m_dbName),
        m_type(datasource.m_type),
        m_host(datasource.m_host),
        m_port(datasource.m_port),
        m_username(datasource.m_username),
        m_password(datasource.m_password),
        m_tables(datasource.m_tables) {

    }

	Datasource::~Datasource()
	{
	}

    Datasource& Datasource::operator=(const Datasource& datasource){

        m_dbName = datasource.m_dbName;
        m_type = datasource.m_type;
        m_host = datasource.m_host;
        m_port = datasource.m_port;
        m_username = datasource.m_username;
        m_password = datasource.m_password;

        m_tables = datasource.m_tables;

        return *this;
    }

    const std::wstring& Datasource::GetDBName() const {

        return m_dbName;
    }

	Datasource::SourceType Datasource::GetType() const {

        return m_type;
    }

    const std::wstring& Datasource::GetHost() const {

        return m_host;
    }

    unsigned int Datasource::GetPort() const {

        return m_port;
    }

    const std::wstring& Datasource::GetUsername() const {

        return m_username;
    }

    const std::wstring& Datasource::GetPassword() const {

        return m_password;
    }

    const std::vector<std::wstring>& Datasource::GetTables() const {

        return m_tables;
    }

    void Datasource::AddTables(const std::vector<std::wstring>& tables) {

		m_tables.insert(m_tables.end(), tables.begin(), tables.end());
	}

	bool Datasource::IsOriginalDatasourceADatabase() const {

		//For now return true in all cases since we are only handling databases
		return true;
	}

	Datasource::PropertyTree& Datasource::ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree) {

		PropertyTree& propertyTree = parentPropertyTree.add(L"Datasource", L"");

		propertyTree.put(L"Name", m_dbName);
		propertyTree.put(L"<xmlattr>.type", s_sourceTypeStrings.left.at(m_type));
		propertyTree.put(L"Host", m_host);

		if (m_port != 0) {
			propertyTree.put(L"Port", m_port);
		}

		if (!m_username.empty()) {
			propertyTree.put(L"Username", m_username);
		}

		if (!m_password.empty()) {
			propertyTree.put(L"Password", m_password);
		}

		if (!m_tables.empty()) {
			boost::property_tree::wptree& tablesPropertyTree = propertyTree.add(L"Tables", L"");
			for (const std::wstring& table : m_tables) {
				tablesPropertyTree.add(L"Table", table);
			}
		}

		return propertyTree;
	}

} //namespace SynGlyphX