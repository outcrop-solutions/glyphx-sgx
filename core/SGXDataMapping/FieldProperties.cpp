#include "FieldProperties.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>

namespace SynGlyphX {

	const boost::bimap<FieldProperties::Type, std::wstring> FieldProperties::s_fieldTypeStrings = boost::assign::list_of < boost::bimap<FieldProperties::Type, std::wstring>::relation >
		(FieldProperties::Type::Default, L"Default")
		(FieldProperties::Type::Number, L"Number")
		(FieldProperties::Type::Currency, L"Currency")
		(FieldProperties::Type::Percentage, L"Percentage");

	FieldProperties::FieldProperties(boost::uuids::uuid id, std::wstring tbl, std::wstring fld, std::wstring type, int dec, std::wstring sym) :
		m_id(id), m_table(tbl), m_field(fld),
		m_type(s_fieldTypeStrings.right.at(type)),
		m_decimals(dec), m_symbol(sym)
	{
	}

	FieldProperties::FieldProperties(const FieldProperties& fp) :
		m_id(fp.m_id), 
		m_table(fp.m_table), 
		m_field(fp.m_field),
		m_type(fp.m_type),
		m_decimals(fp.m_decimals), 
		m_symbol(fp.m_symbol),
		m_stats(fp.m_stats)
	{
	}

	bool FieldProperties::operator==(const FieldProperties& inputField) const {

		if (m_id != inputField.m_id ||
			m_table != inputField.m_table ||
			m_field != inputField.m_field ||
			m_type != inputField.m_type ||
			m_decimals != inputField.m_decimals ||
			m_symbol != inputField.m_symbol ||
			m_stats != inputField.m_stats) 
		{
			return false;
		}

		return true;
	}

	bool FieldProperties::operator!=(const FieldProperties& inputField) const {

		return !operator==(inputField);
	}


	void FieldProperties::AddStatsToField(QStringList stats)
	{
		m_stats = stats;
	}

	void FieldProperties::UpdateProperties(std::wstring type, int dec, std::wstring sym)
	{
		m_type = s_fieldTypeStrings.right.at(type);
		m_decimals = dec;
		m_symbol = sym;
	}

	QString FieldProperties::transformData(QString value){

		return transformData(value.toDouble());
	}

	QString FieldProperties::transformData(int value){

		return transformData(m_stats.at(value).toDouble());
	}

	QString FieldProperties::transformData(double value){

		QString first = "";
		QString last = "";

		if (m_type == FieldProperties::Type::Default){
			if (m_stats.at(2).toDouble() == value){ return m_stats.at(2); }
			else if (m_stats.at(3).toDouble() == value){ return m_stats.at(3); }
			return QString::number(value);
		}
		else if (m_type == FieldProperties::Type::Currency){
			first = QString::fromStdWString(m_symbol);
		}
		else if (m_type == FieldProperties::Type::Percentage){
			value *= 100;
			last = "%";
		}

		return first + QString::number(value, 'f', m_decimals) + last;
	}

	void FieldProperties::ExportToPropertyTree(boost::property_tree::wptree& propertyTree, std::wstring hashid) const {
		propertyTree.put(L"<xmlattr>.hashid", hashid);
		propertyTree.put(L"<xmlattr>.id", m_id);
		propertyTree.put(L"<xmlattr>.table", m_table);
		propertyTree.put(L"<xmlattr>.field", m_field);
		propertyTree.put(L"<xmlattr>.type", s_fieldTypeStrings.left.at(m_type));
		propertyTree.put(L"<xmlattr>.dec", m_decimals);
		propertyTree.put(L"<xmlattr>.sym", m_symbol);
	}

}