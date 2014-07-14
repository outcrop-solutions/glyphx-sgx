#include "inputfield.h"

namespace SynGlyphX {

	InputField::InputField(const boost::uuids::uuid& uuid, const std::wstring& table, const std::wstring field) :
		m_uuid(uuid),
		m_table(table),
		m_field(field)
	{
	}

	InputField::InputField(const InputField& inputField) :
		m_uuid(inputField.m_uuid),
		m_table(inputField.m_table),
		m_field(inputField.m_field) {

	}

	InputField::~InputField()
	{
	}

	InputField& InputField::operator=(const InputField& inputField) {

		m_uuid = inputField.m_uuid;
		m_table = inputField.m_table;
		m_field = inputField.m_field;

		return *this;
	}

	void InputField::SetMinMax(double min, double max) {

		m_min = min;
		m_max = max;
	}

	const boost::uuids::uuid& InputField::GetUUID() const {

		return m_uuid;
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

} //namespace SynGlyphX