#include "datasourcetable.h"
#include <boost/property_tree/ptree.hpp>

namespace SynGlyphX {

	DatasourceTable::DatasourceTable(const std::wstring& name, const boost::property_tree::wptree& query) :
		m_name(name),
		m_query(query)
	{
	}

	DatasourceTable::DatasourceTable(const boost::property_tree::wptree& propertyTree) :
		m_name(propertyTree.get_optional<std::wstring>(L"<xmlattr>.name").get_value_or(propertyTree.get<std::wstring>(L""))),
		m_query(propertyTree.get_child_optional(L"Query").get_value_or(boost::property_tree::wptree())) {

	}

	DatasourceTable::DatasourceTable(const DatasourceTable& table) :
		m_name(table.m_name),
		m_query(table.m_query) {


	}

	DatasourceTable::~DatasourceTable()
	{
	}

	DatasourceTable& DatasourceTable::operator=(const DatasourceTable& table) {

		m_name = table.m_name;
		m_query = table.m_query;

		return *this;
	}

	bool DatasourceTable::operator==(const DatasourceTable& table) const {

		if (m_name != table.m_name) {

			return false;
		}

		if (m_query != table.m_query) {

			return false;
		}

		return true;
	}

	bool DatasourceTable::operator!=(const DatasourceTable& table) const {

		return !operator==(table);
	}

	const std::wstring& DatasourceTable::GetName() const {

		return m_name;
	}

	const boost::property_tree::wptree& DatasourceTable::GetQuery() const {

		return m_query;
	}

	void DatasourceTable::SetQuery(const boost::property_tree::wptree& query)
	{
		m_query = query;
	}

	boost::property_tree::wptree& DatasourceTable::ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree) const {

		boost::property_tree::wptree& tablePropertyTree = parentPropertyTree.add(L"Table", L"");
		tablePropertyTree.put(L"<xmlattr>.name", m_name);
		if (!m_query.empty()) {
			tablePropertyTree.add_child(L"Query", m_query);
		}

		return tablePropertyTree;
	}

} //namespace SynGlyphX