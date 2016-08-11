#include "datasource.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/property_tree/ptree.hpp>
#include "datasourcetable.h"

namespace SynGlyphX {

	const Datasource::SourceTypeBimap Datasource::s_sourceTypeStrings = boost::assign::list_of < Datasource::SourceTypeBimap::relation >
		(Datasource::SourceType::File, L"File")
		(Datasource::SourceType::DatabaseServer, L"DatabaseServer");

	const std::wstring Datasource::SingleTableName = L"OnlyTable";

	Datasource::Datasource(const std::wstring& host, const std::wstring& username, const std::wstring& password) :
        m_host(host),
        m_username(username),
        m_password(password)
	{
	}

	Datasource::Datasource(const PropertyTree& propertyTree) :
		m_host(propertyTree.get<std::wstring>(L"Host")),
		m_username(propertyTree.get<std::wstring>(L"Username", L"")),
		m_password(propertyTree.get<std::wstring>(L"Password", L"")) {

		boost::optional<const boost::property_tree::wptree&> tablePropertyTree = propertyTree.get_child_optional(L"Tables");

		if (tablePropertyTree.is_initialized()) {

			for (const boost::property_tree::wptree::value_type& tableValue : tablePropertyTree.get()) {

				if (tableValue.first == L"Table") {

					m_tables.emplace(tableValue.second.get<std::wstring>(L"<xmlattr>.name"), DatasourceTable(tableValue.second));
				}
			}
		}
	}

    Datasource::Datasource(const Datasource& datasource) :
        m_host(datasource.m_host),
        m_username(datasource.m_username),
        m_password(datasource.m_password),
        m_tables(datasource.m_tables) {

    }

	Datasource::~Datasource()
	{
	}

    Datasource& Datasource::operator=(const Datasource& datasource) {

        m_host = datasource.m_host;
        m_username = datasource.m_username;
        m_password = datasource.m_password;

        m_tables = datasource.m_tables;

        return *this;
    }

	bool Datasource::operator==(const Datasource& datasource) const {

		if (GetSourceType() != datasource.GetSourceType()) {

			return false;
		}

		if (m_host != datasource.m_host) {

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

    const std::wstring& Datasource::GetHost() const {

        return m_host;
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

	const boost::property_tree::wptree& Datasource::GetQuery(const std::wstring& table) const {
		return m_tables.at(table).GetQuery();
	}

	void Datasource::SetQuery(const std::wstring& table, const boost::property_tree::wptree& query) {
		m_tables.at(table).SetQuery(query);
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

			tableNames.push_back(table.first);
		}

		return tableNames;
	}

	bool Datasource::DoAnyTablesHaveQueries() const {
		_ASSERT(0); //TODO: dont forget to remove
		for (auto& table : m_tables) {

			if (!table.second.GetQuery().empty()) {

				return true;
			}
		}

		return false;
	}

	Datasource::PropertyTree& Datasource::ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree) {

		PropertyTree& propertyTree = parentPropertyTree.add(L"Datasource", L"");
		propertyTree.put(L"<xmlattr>.source", s_sourceTypeStrings.left.at(GetSourceType()));
		propertyTree.put(L"Host", m_host);

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