#include "DataSourceTable.h"
#include <boost/property_tree/ptree.hpp>

namespace GlyphEngine {

	DataSourceTable::DataSourceTable(const std::wstring& name, const boost::property_tree::wptree& query) :
		m_name(name),
		m_query(query)
	{
	}

	DataSourceTable::DataSourceTable(const boost::property_tree::wptree& propertyTree) :
		m_name(propertyTree.get_optional<std::wstring>(L"<xmlattr>.name").get_value_or(propertyTree.get<std::wstring>(L"")))
		//m_query(propertyTree.get_child_optional(L"Query").get_value_or(boost::property_tree::wptree())) //broken after upgrading to new boost version
		//"binding rvalue references to optional lvalue references is disallowed"
	{
		auto query = propertyTree.get_child_optional(L"Query");
		if (query.is_initialized())
		{
			m_query = query.get();
		}
	}

	DataSourceTable::DataSourceTable(const DataSourceTable& table) :
		m_name(table.m_name),
		m_query(table.m_query) {


	}

	DataSourceTable::~DataSourceTable()
	{
	}

	DataSourceTable& DataSourceTable::operator=(const DataSourceTable& table) {

		m_name = table.m_name;
		m_query = table.m_query;

		return *this;
	}

	bool DataSourceTable::operator==(const DataSourceTable& table) const {

		if (m_name != table.m_name) {

			return false;
		}

		if (m_query != table.m_query) {

			return false;
		}

		return true;
	}

	bool DataSourceTable::operator!=(const DataSourceTable& table) const {

		return !operator==(table);
	}

	const std::wstring& DataSourceTable::GetName() const {

		return m_name;
	}

	const boost::property_tree::wptree& DataSourceTable::GetQuery() const {

		return m_query;
	}

	void DataSourceTable::SetQuery(const boost::property_tree::wptree& query)
	{
		m_query = query;
	}

	boost::property_tree::wptree& DataSourceTable::ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree) const {

		boost::property_tree::wptree& tablePropertyTree = parentPropertyTree.add(L"Table", L"");
		tablePropertyTree.put(L"<xmlattr>.name", m_name);
		if (!m_query.empty()) {
			tablePropertyTree.add_child(L"Query", m_query);
		}

		return tablePropertyTree;
	}

} 