#include "datasource.h"

namespace SynGlyphX {

	const std::wstring Datasource::SingleTableName = L"OnlyTable";

	Datasource::Datasource(const std::wstring& dbName, const std::wstring& host, unsigned int port, const std::wstring& username, const std::wstring& password) :
        m_dbName(dbName),
        m_host(host),
        m_port(port),
        m_username(username),
        m_password(password)
	{
	}

	Datasource::Datasource(const PropertyTree& propertyTree) :
		m_dbName(propertyTree.get<std::wstring>(L"Name")),
		m_host(propertyTree.get<std::wstring>(L"Host")),
		m_port(propertyTree.get<unsigned int>(L"Port", 0)),
		m_username(propertyTree.get<std::wstring>(L"Username", L"")),
		m_password(propertyTree.get<std::wstring>(L"Password", L"")) {

		boost::optional<const boost::property_tree::wptree&> tablePropertyTree = propertyTree.get_child_optional(L"Tables");

		if (tablePropertyTree.is_initialized()) {

			TableSet tables;
			for (const boost::property_tree::wptree::value_type& tableValue : tablePropertyTree.get()) {

				if (tableValue.first == L"Table") {
					tables.insert(tableValue.second.data());
				}
			}
			EnableTables(tables, true);
		}
	}

    Datasource::Datasource(const Datasource& datasource) :
        m_dbName(datasource.m_dbName),
        m_host(datasource.m_host),
        m_port(datasource.m_port),
        m_username(datasource.m_username),
        m_password(datasource.m_password),
        m_tables(datasource.m_tables) {

    }

	Datasource::~Datasource()
	{
	}

    Datasource& Datasource::operator=(const Datasource& datasource) {

        m_dbName = datasource.m_dbName;
        m_host = datasource.m_host;
        m_port = datasource.m_port;
        m_username = datasource.m_username;
        m_password = datasource.m_password;

        m_tables = datasource.m_tables;

        return *this;
    }

	bool Datasource::operator==(const Datasource& datasource) const {

		if (m_dbName != datasource.m_dbName) {

			return false;
		}

		if (m_host != datasource.m_host) {

			return false;
		}

		if (m_port != datasource.m_port) {

			return false;
		}

		if (m_username != datasource.m_username) {

			return false;
		}

		if (m_password != datasource.m_password) {

			return false;
		}

		if (m_tables != datasource.m_tables) {

			return false;
		}

		return true;
	}

	bool Datasource::operator!=(const Datasource& datasource) const {

		return !operator==(datasource);
	}

    const std::wstring& Datasource::GetDBName() const {

        return m_dbName;
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

	const Datasource::TableSet& Datasource::GetTables() const {

        return m_tables;
    }

	void Datasource::EnableTables(const TableSet& tables, bool enable) {

		if (enable) {
			
			m_tables.insert(tables.begin(), tables.end());
		}
		else {

			m_tables.erase(tables.begin(), tables.end());
		}
	}

	Datasource::PropertyTree& Datasource::ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree, const std::wstring& parentName) {

		PropertyTree& propertyTree = parentPropertyTree.add(parentName, L"");

		propertyTree.put(L"Name", m_dbName);
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

		if (!m_tables.empty() && CanDatasourceHaveMultipleTables()) {

			boost::property_tree::wptree& tablesPropertyTree = propertyTree.add(L"Tables", L"");
			for (const std::wstring& table : m_tables) {

				tablesPropertyTree.add(L"Table", table);
			}
		}

		return propertyTree;
	}

} //namespace SynGlyphX