#include "FieldProperties.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>

namespace SynGlyphX {

	const boost::bimap<FieldProperties::Type, std::wstring> FieldProperties::s_fieldTypeStrings = boost::assign::list_of < boost::bimap<FieldProperties::Type, std::wstring>::relation >
		(FieldProperties::Type::Number, L"Number")
		(FieldProperties::Type::Currency, L"Currency")
		(FieldProperties::Type::Percentage, L"Percentage");

	FieldProperties::FieldProperties(boost::uuids::uuid id, std::wstring tbl, std::wstring fld, std::wstring type, int dec, std::wstring sym) :
		m_id(id), m_table(tbl), m_field(fld),
		m_type(s_fieldTypeStrings.right.at(type)),
		m_decimals(dec), m_symbol(sym)
	{
	}

	FieldProperties::~FieldProperties()
	{
	}

	void FieldProperties::UpdateProperties(std::wstring type, int dec, std::wstring sym)
	{
		m_type = s_fieldTypeStrings.right.at(type);
		m_decimals = dec;
		m_symbol = sym;
	}
}