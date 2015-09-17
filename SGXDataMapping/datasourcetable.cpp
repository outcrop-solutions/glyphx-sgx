#include "datasourcetable.h"

namespace SynGlyphX {

	DatasourceTable::DatasourceTable(const std::wstring& name) :
		m_name(name)
	{
	}

	DatasourceTable::DatasourceTable(const boost::property_tree::wptree& propertyTree) :
		m_name(propertyTree.data()) {

		auto fieldGroups = propertyTree.equal_range(L"FieldGroup");
		for (boost::property_tree::wptree::const_assoc_iterator fieldGroupPropertyTree = fieldGroups.first; fieldGroupPropertyTree != fieldGroups.second; ++fieldGroupPropertyTree) {

			FieldGroup fieldGroup;
			auto fields = fieldGroupPropertyTree->second.equal_range(L"Field");
			for (boost::property_tree::wptree::const_assoc_iterator fieldPropertyTree = fields.first; fieldPropertyTree != fields.second; ++fieldPropertyTree) {

				fieldGroup.insert(fieldPropertyTree->second.data());
			}
			UpdateFieldGroup(fieldGroupPropertyTree->second.data(), fieldGroup);
		}
	}

	DatasourceTable::DatasourceTable(const DatasourceTable& table) :
		m_name(table.m_name),
		m_fieldGroups(table.m_fieldGroups) {


	}

	DatasourceTable::~DatasourceTable()
	{
	}

	DatasourceTable& DatasourceTable::operator=(const DatasourceTable& table) {

		m_name = table.m_name;
		m_fieldGroups = table.m_fieldGroups;

		return *this;
	}

	bool DatasourceTable::operator==(const DatasourceTable& table) const {

		if (m_name != table.m_name) {

			return false;
		}

		if (m_fieldGroups != table.m_fieldGroups) {

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

	const std::unordered_map<std::wstring, DatasourceTable::FieldGroup>& DatasourceTable::GetFieldGroups() const {

		return m_fieldGroups;
	}

	const DatasourceTable::FieldGroup& DatasourceTable::GetFieldGroup(const FieldGroupName& groupName) const {

		if (m_fieldGroups.count(groupName) == 0) {

			throw std::invalid_argument("Given group name doesn't exist");
		}

		return m_fieldGroups.at(groupName);
	}

	void DatasourceTable::UpdateFieldGroup(const FieldGroupName& groupName, const FieldGroup& group) {

		if (group.empty()) {

			throw std::invalid_argument("Can not add or update field group with an empty list of fields");
		}

		m_fieldGroups[groupName] = group;
	}

	void DatasourceTable::RemoveFieldGroup(const FieldGroupName& groupName) {

		if (m_fieldGroups.count(groupName) > 0) {

			m_fieldGroups.erase(groupName);
		}
	}

	boost::property_tree::wptree& DatasourceTable::ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree) const {

		boost::property_tree::wptree& tablePropertyTree = parentPropertyTree.add(L"Table", m_name);
		for (auto& fieldGroup : m_fieldGroups) {

			boost::property_tree::wptree& groupPropertyTree = tablePropertyTree.add(L"FieldGroup", L"");
			groupPropertyTree.put(L"<xmlattr>.name", fieldGroup.first);
			for (auto& field : fieldGroup.second) {

				groupPropertyTree.add(L"Field", field);
			}
		}

		return tablePropertyTree;
	}

} //namespace SynGlyphX