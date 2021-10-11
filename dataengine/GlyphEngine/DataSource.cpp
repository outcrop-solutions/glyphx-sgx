#include "DataSource.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/property_tree/ptree.hpp>
#include "DataSourceTable.h"

namespace GlyphEngine {

	const DataSource::SourceTypeBimap DataSource::s_sourceTypeStrings = boost::assign::list_of < DataSource::SourceTypeBimap::relation >
		(DataSource::SourceType::File, L"File")
		(DataSource::SourceType::DatabaseServer, L"DatabaseServer");

	const std::wstring DataSource::SingleTableName = L"OnlyTable";

	DataSource::DataSource(const std::wstring& host, const std::wstring& username, const std::wstring& password) :
        m_host(host),
        m_username(username),
        m_password(password)
	{
	}

	DataSource::DataSource(const PropertyTree& propertyTree) :
		m_host(propertyTree.get<std::wstring>(L"Host")),
		m_username(propertyTree.get<std::wstring>(L"Username", L"")),
		m_password(propertyTree.get<std::wstring>(L"Password", L"")) {

		boost::optional<const boost::property_tree::wptree&> tablePropertyTree = propertyTree.get_child_optional(L"Tables");

		if (tablePropertyTree.is_initialized()) {

			for (const boost::property_tree::wptree::value_type& tableValue : tablePropertyTree.get()) {

				if (tableValue.first == L"Table") {

					m_tables.emplace(tableValue.second.get_optional<std::wstring>(L"<xmlattr>.name").get_value_or(tableValue.second.get<std::wstring>(L"")),
						DataSourceTable(tableValue.second));
				}
			}
		}
	}

    DataSource::DataSource(const DataSource& DataSource) :
        m_host(DataSource.m_host),
        m_username(DataSource.m_username),
        m_password(DataSource.m_password),
        m_tables(DataSource.m_tables) {

    }

	DataSource::~DataSource()
	{
	}

    DataSource& DataSource::operator=(const DataSource& DataSource) {

        m_host = DataSource.m_host;
        m_username = DataSource.m_username;
        m_password = DataSource.m_password;

        m_tables = DataSource.m_tables;

        return *this;
    }

	bool DataSource::operator==(const DataSource& DataSource) const {

		if (GetSourceType() != DataSource.GetSourceType()) {

			return false;
		}

		if (m_host != DataSource.m_host) {

			return false;
		}

		if (m_username != DataSource.m_username) {

			return false;
		}

		if (m_password != DataSource.m_password) {

			return false;
		}

		if (m_tables != DataSource.m_tables) {

			return false;
		}

		return true;
	}

	bool DataSource::operator!=(const DataSource& DataSource) const {

		return !operator==(DataSource);
	}

    const std::wstring& DataSource::GetHost() const {

        return m_host;
    }

    const std::wstring& DataSource::GetUsername() const {

        return m_username;
    }

    const std::wstring& DataSource::GetPassword() const {

        return m_password;
    }

	const DataSource::Tables& DataSource::GetTables() const {

        return m_tables;
    }

	const boost::property_tree::wptree& DataSource::GetQuery(const std::wstring& table) const {
		return m_tables.at(table).GetQuery();
	}

	void DataSource::SetQuery(const std::wstring& table, const boost::property_tree::wptree& query) {
		m_tables.at(table).SetQuery(query);
	}

	void DataSource::AddTables(const Tables& tables) {

		m_tables.insert(tables.begin(), tables.end());
	}

	void DataSource::AddTables(const TableNames& tableNames) {

		for (const std::wstring& tableName : tableNames) {

			if (m_tables.count(tableName) == 0) {

				m_tables.emplace(tableName, DataSourceTable(tableName));
			}
		}
	}

	void DataSource::RemoveTable(const std::wstring& tableName) {

		if (m_tables.count(tableName) == 0) {

			throw std::invalid_argument("Table can not be removed since is not in DataSource");
		}

		m_tables.erase(tableName);
	}

	void DataSource::ClearTables() {

		m_tables.clear();
	}

	DataSource::TableNames DataSource::GetTableNames() const {

		TableNames tableNames;
		for (auto& table : m_tables) {

			tableNames.push_back(table.first);
		}

		return tableNames;
	}

	bool DataSource::DoAnyTablesHaveQueries() const {
		for (auto& table : m_tables) {

			if (!table.second.GetQuery().empty()) {

				return true;
			}
		}

		return false;
	}

	DataSource::PropertyTree& DataSource::ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree) {

		PropertyTree& propertyTree = parentPropertyTree.add(L"DataSource", L"");
		propertyTree.put(L"<xmlattr>.source", s_sourceTypeStrings.left.at(GetSourceType()));
		propertyTree.put(L"Host", m_host);

		if (!m_username.empty()) {
			propertyTree.put(L"Username", m_username);
		}

		if (!m_password.empty()) {
			propertyTree.put(L"Password", m_password);
		}

		if (!m_tables.empty() && CanDataSourceHaveMultipleTables()) {

			boost::property_tree::wptree& tablesPropertyTree = propertyTree.add(L"Tables", L"");
			for (auto& table : m_tables) {

				table.second.ExportToPropertyTree(tablesPropertyTree);
			}
		}

		return propertyTree;
	}

} //namespace SynGlyphX