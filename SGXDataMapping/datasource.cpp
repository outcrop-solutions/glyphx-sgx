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

			Tables tables;
			for (const boost::property_tree::wptree::value_type& tableValue : tablePropertyTree.get()) {

				if (tableValue.first == L"Table") {

					tables.emplace(tableValue.second.data(), DatasourceTable(tableValue.second));
				}
			}
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

	const Datasource::Tables& Datasource::GetTables() const {

        return m_tables;
    }

	void Datasource::AddTables(const Tables& tables) {

		m_tables.insert(tables.begin(), tables.end());
	}

	void Datasource::AddTables(const TableNames& tableNames) {

		for (const std::wstring& tableName : tableNames) {

			if (m_tables.count(tableName) == 0) {

				m_tables.emplace(tableName, DatasourceTable(tableName));
			}
		}
	}

	void Datasource::RemoveTable(const std::wstring& tableName) {

		if (m_tables.count(tableName) == 0) {

			throw std::invalid_argument("Table can not be removed since is not in datasource");
		}

		m_tables.erase(tableName);
	}

	void Datasource::ClearTables() {

		m_tables.clear();
	}

	Datasource::TableNames Datasource::GetTableNames() const {

		TableNames tableNames;
		for (auto& table : m_tables) {

			tableNames.insert(table.first);
		}

		return tableNames;
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
			for (auto& table : m_tables) {

				table.second.ExportToPropertyTree(tablesPropertyTree);
			}
		}

		return propertyTree;
	}

} //namespace SynGlyphX