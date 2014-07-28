#include "inputfield.h"
#include <boost/uuid/uuid_io.hpp>

namespace SynGlyphX {

	InputField::InputField() {

	}

	InputField::InputField(const boost::uuids::uuid& datasourceID, const std::wstring& table, const std::wstring field) :
		m_datasourceID(datasourceID),
		m_table(table),
		m_field(field)
	{
	}

	InputField::InputField(const boost::property_tree::wptree& propertyTree) :
		m_datasourceID(propertyTree.get<boost::uuids::uuid>(L"<xmlattr>.id")),
		m_table(propertyTree.get<std::wstring>(L"<xmlattr>.table")),
		m_field(propertyTree.get<std::wstring>(L"<xmlattr>.field")),
		m_min(propertyTree.get<double>(L"Min")),
		m_max(propertyTree.get<double>(L"Max")) {

	}

	InputField::InputField(const InputField& inputField) :
		m_datasourceID(inputField.m_datasourceID),
		m_table(inputField.m_table),
		m_field(inputField.m_field),
		m_min(inputField.m_min),
		m_max(inputField.m_max) {

	}

	InputField::~InputField()
	{
	}

	InputField& InputField::operator=(const InputField& inputField) {

		m_datasourceID = inputField.m_datasourceID;
		m_table = inputField.m_table;
		m_field = inputField.m_field;
		m_min = inputField.m_min;
		m_max = inputField.m_max;

		return *this;
	}

	void InputField::SetMinMax(double min, double max) {

		m_min = min;
		m_max = max;
	}

	const boost::uuids::uuid& InputField::GetDatasourceID() const {

		return m_datasourceID;
	}

	const std::wstring& InputField::GetTable() const {

		return m_table;
	}

	const std::wstring& InputField::GetField() const {

		return m_field;
	}

	double InputField::GetMin() const {

		return m_min;
	}

	double InputField::GetMax() const {

		return m_max;
	}

	bool InputField::IsValid() const {

		return !(m_datasourceID.is_nil() || m_table.empty() || m_field.empty());
	}

	void InputField::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		boost::property_tree::wptree& inputFieldPropertyTree = propertyTree.add(L"InputField", L"");
		
		inputFieldPropertyTree.put(L"<xmlattr>.id", m_datasourceID);
		inputFieldPropertyTree.put(L"<xmlattr>.table", m_table);
		inputFieldPropertyTree.put(L"<xmlattr>.field", m_field);
		inputFieldPropertyTree.put(L"Min", m_min);
		inputFieldPropertyTree.put(L"Max", m_max);
	}

} //namespace SynGlyphX