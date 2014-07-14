#include "datasource.h"

namespace SynGlyphX {

    Datasource::Datasource(const std::wstring& dbName, const std::wstring& type, const std::wstring& host, unsigned int port, const std::wstring& username, const std::wstring& password) :
        m_dbName(dbName),
        m_type(type),
        m_host(host),
        m_port(port),
        m_username(username),
        m_password(password)
	{
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

    const std::wstring& Datasource::GetType() const {

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

} //namespace SynGlyphX