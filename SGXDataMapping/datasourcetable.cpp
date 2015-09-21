#include "datasourcetable.h"

namespace SynGlyphX {

	DatasourceTable::DatasourceTable(const std::wstring& name) :
		m_name(name)
	{
	}

	DatasourceTable::DatasourceTable(const boost::property_tree::wptree& propertyTree) :
		m_name(propertyTree.data()) {

	}

	DatasourceTable::DatasourceTable(const DatasourceTable& table) :
		m_name(table.m_name) {


	}

	DatasourceTable::~DatasourceTable()
	{
	}

	DatasourceTable& DatasourceTable::operator=(const DatasourceTable& table) {

		m_name = table.m_name;

		return *this;
	}

	bool DatasourceTable::operator==(const DatasourceTable& table) const {

		if (m_name != table.m_name) {

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

	boost::property_tree::wptree& DatasourceTable::ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree) const {

		boost::property_tree::wptree& tablePropertyTree = parentPropertyTree.add(L"Table", m_name);

		return tablePropertyTree;
	}

} //namespace SynGlyphX