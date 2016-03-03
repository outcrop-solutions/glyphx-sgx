#include "datasourcetable.h"

namespace SynGlyphX {

	DatasourceTable::DatasourceTable(const std::wstring& name, const std::wstring& query) :
		m_name(name),
		m_query(query)
	{
	}

	DatasourceTable::DatasourceTable(const boost::property_tree::wptree& propertyTree) :
		m_name(propertyTree.data()),
		m_query(propertyTree.get_optional<std::wstring>(L"<xmlattr>.query").get_value_or(L"")) {

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

	const std::wstring& DatasourceTable::GetQuery() const {

		return m_query;
	}

	boost::property_tree::wptree& DatasourceTable::ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree) const {

		boost::property_tree::wptree& tablePropertyTree = parentPropertyTree.add(L"Table", m_name);

		if (!m_query.empty()) {

			tablePropertyTree.put(L"<xmlattr>.query", m_query);
		}

		return tablePropertyTree;
	}

} //namespace SynGlyphX