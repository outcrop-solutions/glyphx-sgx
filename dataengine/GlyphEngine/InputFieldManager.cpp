#include "InputFieldManager.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>


namespace GlyphEngine {

	const InputField& InputFieldManager::GetInputField(const std::wstring& fieldID) const
	{
		const static InputField empty;
		auto it = m_inputFields.find(fieldID);
		if (it != m_inputFields.end())
			return it->second;
		else
			return empty;
	}

	void InputFieldManager::RemoveInputFieldAndBindings(const std::wstring& fieldID)
	{
		m_inputFields.erase(fieldID);
	}

	void InputFieldManager::OnRemoveDataSource(const boost::uuids::uuid& id)
	{
		std::vector<std::wstring> toRemove;
		for (const auto& field : m_inputFields)
		{
			if (field.second.GetDatasourceID() == id)
				toRemove.push_back(field.first);
		}
		for (const auto& field : toRemove)
			RemoveInputFieldAndBindings(field);
	}

	void InputFieldManager::SetInputField(const std::wstring& fieldID, const InputField& field)
	{
		m_inputFields[fieldID] = field;
	}

	std::wstring InputFieldManager::GenerateInputFieldID(const InputField& field)
	{
		HashID id = field.GetHashID();
		std::wstring str = L"~";
		str += std::to_wstring(id);
		return str;
	}

	void InputFieldManager::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const
	{
		for (auto field : m_inputFields)
		{
			field.second.ExportToPropertyTree(propertyTree, field.first);
		}
	}

	void InputFieldManager::ImportFromPropertyTree(const boost::property_tree::wptree& propertyTree)
	{
		for (const boost::property_tree::wptree::value_type& inputField : propertyTree)
		{
			if (inputField.first == L"InputField")
			{
				m_inputFields[inputField.second.get<std::wstring>(L"<xmlattr>.name")] = InputField(inputField.second);
			}
		}
	}

	void InputFieldManager::Clear()
	{
		m_inputFields.clear();
	}

	const std::unordered_map<std::wstring, InputField>& InputFieldManager::GetFieldMap() const
	{
		return m_inputFields;
	}

} 