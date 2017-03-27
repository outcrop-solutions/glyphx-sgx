#include "inputfield.h"
#include <boost/uuid/uuid_io.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>
#include <boost/property_tree/ptree.hpp>
#include "datasource.h"

namespace SynGlyphX {

	const boost::bimap<InputField::Type, std::wstring> InputField::s_fieldTypeStrings = boost::assign::list_of < boost::bimap<InputField::Type, std::wstring>::relation >
		(InputField::Type::Null, L"Null")
		(InputField::Type::Integer, L"Integer")
		(InputField::Type::Real, L"Real")
		(InputField::Type::Text, L"Text")
		(InputField::Type::DateTime, L"DateTime")
		(InputField::Type::Other, L"Other");

	InputField::InputField() :
		InputTable(),
		m_type(Null) {

	}

	InputField::InputField(const boost::uuids::uuid& datasourceID, const std::wstring& table, const std::wstring field, Type type) :
		InputTable(datasourceID, table),
		m_field(field),
		m_type(type)
	{
		
	}

	InputField::InputField(const boost::property_tree::wptree& propertyTree) :
		InputTable(propertyTree),
		m_field(propertyTree.get<std::wstring>(L"<xmlattr>.field")),
		m_type(s_fieldTypeStrings.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.type")))
	{

	}

	InputField::InputField(const InputField& inputField) :
		InputTable(inputField),
		m_field(inputField.m_field),
		m_type(inputField.m_type)
	{

	}

	//InputField::InputField(const InputFieldAlias& inputFieldAlias) :
	//	InputTable(inputFieldAlias.GetInputField()),
	//	m_field(inputFieldAlias.GetInputField().m_field),
	//	m_type(inputFieldAlias.GetInputField().m_type) {

	//}

	InputField::~InputField()
	{
	}

	InputField& InputField::operator=(const InputField& inputField) {

		InputTable::operator=(inputField);
		m_field = inputField.m_field;
		m_type = inputField.m_type;

		return *this;
	}

	bool InputField::operator==(const InputField& inputField) const {

		if (InputTable::operator!=(inputField)) {

			return false;
		}

		if (m_field != inputField.m_field) {

			return false;
		}

		if (m_type != inputField.m_type) {

			return false;
		}

		return true;
	}

	bool InputField::operator!=(const InputField& inputField) const {

		return !operator==(inputField);
	}

	const std::wstring& InputField::GetField() const {

		return m_field;
	}

	bool InputField::IsValid() const {

		return (InputTable::IsValid() && !m_field.empty());
	}

	void InputField::ExportToPropertyTree(boost::property_tree::wptree& propertyTree, const std::wstring& name /*= std::wstring()*/) const {
		boost::property_tree::wptree& inputFieldPropertyTree = propertyTree.add(L"InputField", L"");
		if (!name.empty()) {
			inputFieldPropertyTree.put(L"<xmlattr>.name", name);
		}

		inputFieldPropertyTree.put(L"<xmlattr>.id", m_datasourceID);
		inputFieldPropertyTree.put(L"<xmlattr>.table", m_table);
		inputFieldPropertyTree.put(L"<xmlattr>.field", m_field);
		inputFieldPropertyTree.put(L"<xmlattr>.type", s_fieldTypeStrings.left.at(m_type));
	}

	void InputField::ExportToPropertyTreeInternal(boost::property_tree::wptree& propertyTree) const {

		propertyTree.put(L"<xmlattr>.id", m_datasourceID);
		propertyTree.put(L"<xmlattr>.table", m_table);
		propertyTree.put(L"<xmlattr>.field", m_field);
		propertyTree.put(L"<xmlattr>.type", s_fieldTypeStrings.left.at(m_type));
	}

	bool InputField::IsNumeric() const {

		return ((m_type == Integer) || (m_type == Real));
	}

	InputField::Type InputField::GetType() const {

		return m_type;
	}

	HashID InputField::GetHashID() const {

		HashID seed = InputTable::GetHashID();

		if (IsValid()) {

			CombineHashID(seed, m_field);
		}

		return seed;
	}

	//InputFieldAlias::InputFieldAlias() : m_imputField(), m_name(L"")  {}
	//InputFieldAlias::InputFieldAlias(const InputField& inputField) : m_imputField(inputField), m_name(L"")  {}
} //namespace SynGlyphX
