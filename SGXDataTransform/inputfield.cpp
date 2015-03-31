#include "inputfield.h"
#include <boost/uuid/uuid_io.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>
#include "datasource.h"

namespace SynGlyphX {

	const boost::bimap<InputField::Type, std::wstring> InputField::s_fieldTypeStrings = boost::assign::list_of < boost::bimap<InputField::Type, std::wstring>::relation >
		(InputField::Type::Null, L"Null")
		(InputField::Type::Integer, L"Integer")
		(InputField::Type::Real, L"Real")
		(InputField::Type::Text, L"Text")
		(InputField::Type::Date, L"Date");

	InputField::InputField() :
		m_type(Null) {

	}

	InputField::InputField(const boost::uuids::uuid& datasourceID, const std::wstring& table, const std::wstring field, Type type) :
		m_datasourceID(datasourceID),
		m_table(table),
		m_field(field),
		m_type(type)
	{
		//Since some datasources are single table make sure table has a non-empty value
		if (m_table.empty()) {

			m_table = Datasource::SingleTableName;
		}
	}

	InputField::InputField(const boost::property_tree::wptree& propertyTree) :
		m_datasourceID(propertyTree.get<boost::uuids::uuid>(L"<xmlattr>.id")),
		m_table(propertyTree.get<std::wstring>(L"<xmlattr>.table")),
		m_field(propertyTree.get<std::wstring>(L"<xmlattr>.field")),
		m_type(s_fieldTypeStrings.right.at(propertyTree.get<std::wstring>(L"<xmlattr>.type"))) {

		//Since some datasources are single table make sure table has a non-empty value
		if (m_table.empty()) {

			m_table = Datasource::SingleTableName;
		}
	}

	InputField::InputField(const InputField& inputField) :
		m_datasourceID(inputField.m_datasourceID),
		m_table(inputField.m_table),
		m_field(inputField.m_field),
		m_type(inputField.m_type) {

	}

	InputField::~InputField()
	{
	}

	InputField& InputField::operator=(const InputField& inputField) {

		m_datasourceID = inputField.m_datasourceID;
		m_table = inputField.m_table;
		m_field = inputField.m_field;
		m_type = inputField.m_type;

		return *this;
	}

	bool InputField::operator==(const InputField& inputField) const {

		if (m_datasourceID != inputField.m_datasourceID) {

			return false;
		}

		if (m_table != inputField.m_table) {

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

	const boost::uuids::uuid& InputField::GetDatasourceID() const {

		return m_datasourceID;
	}

	const std::wstring& InputField::GetTable() const {

		return m_table;
	}

	const std::wstring& InputField::GetField() const {

		return m_field;
	}

	bool InputField::IsValid() const {

		return !(m_datasourceID.is_nil() || m_table.empty() || m_field.empty());
	}

	void InputField::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		boost::property_tree::wptree& inputFieldPropertyTree = propertyTree.add(L"InputField", L"");
		
		inputFieldPropertyTree.put(L"<xmlattr>.id", m_datasourceID);
		inputFieldPropertyTree.put(L"<xmlattr>.table", m_table);
		inputFieldPropertyTree.put(L"<xmlattr>.field", m_field);
		inputFieldPropertyTree.put(L"<xmlattr>.type", s_fieldTypeStrings.left.at(m_type));
	}

	bool InputField::IsNumeric() const {

		return ((m_type == Integer) || (m_type == Real));
	}

	InputField::HashID InputField::GetHashID() const {

		std::size_t seed = 0;

		if (IsValid()) {

			boost::hash_combine(seed, m_datasourceID);
			boost::hash_combine(seed, m_table);
			boost::hash_combine(seed, m_field);
		}

		return seed;
	}

} //namespace SynGlyphX