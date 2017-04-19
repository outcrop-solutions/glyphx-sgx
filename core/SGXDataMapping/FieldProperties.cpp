#include "FieldProperties.h"
#include <boost/assign/list_of.hpp>
#include <boost/bimap/list_of.hpp>
#include <QtCore/QDateTime>

namespace SynGlyphX {

	const boost::bimap<FieldProperties::Type, std::wstring> FieldProperties::s_fieldTypeStrings = boost::assign::list_of < boost::bimap<FieldProperties::Type, std::wstring>::relation >
		(FieldProperties::Type::Default, L"Default")
		(FieldProperties::Type::Number, L"Number")
		(FieldProperties::Type::Currency, L"Currency")
		(FieldProperties::Type::Percentage, L"Percentage")
		(FieldProperties::Type::Datetime, L"Datetime");

	FieldProperties::FieldProperties(boost::uuids::uuid id, std::wstring tbl, std::wstring fld, std::wstring type, int dec, std::wstring sym, QString dFmt, QString tFmt) :
		m_id(id), m_table(tbl), m_field(fld)
	{
		UpdateProperties(type, dec, sym, dFmt, tFmt);
	}

	FieldProperties::FieldProperties(const FieldProperties& fp) :
		m_id(fp.m_id), 
		m_table(fp.m_table), 
		m_field(fp.m_field),
		m_stats(fp.m_stats)
	{
		UpdateProperties(s_fieldTypeStrings.left.at(fp.m_type), fp.m_decimals, fp.m_symbol, fp.m_dateFmt, fp.m_timeFmt);
	}

	bool FieldProperties::operator==(const FieldProperties& inputField) const {

		if (m_id != inputField.m_id ||
			m_table != inputField.m_table ||
			m_field != inputField.m_field ||
			m_type != inputField.m_type ||
			m_decimals != inputField.m_decimals ||
			m_symbol != inputField.m_symbol ||
			m_datetimeFmt != inputField.m_datetimeFmt ||
			m_dateFmt != inputField.m_dateFmt ||
			m_timeFmt != inputField.m_timeFmt ||
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

	void FieldProperties::UpdateProperties(std::wstring type, int dec, std::wstring sym, QString dFmt, QString tFmt)
	{
		m_type = s_fieldTypeStrings.right.at(type);
		m_decimals = dec;
		m_symbol = sym;
		m_dateFmt = dFmt == "" ? "N/A" : dFmt;
		m_timeFmt = tFmt == "" ? "N/A" : tFmt;
		m_datetimeFmt = dFmt + (tFmt != "" ? " " : "") + tFmt;
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
			if (!m_stats.empty()){
				if (m_stats.at(2).toDouble() == value){ return m_stats.at(2); }
				else if (m_stats.at(3).toDouble() == value){ return m_stats.at(3); }
			}
			return QString::number(value);
		}
		else if (m_type == FieldProperties::Type::Currency){
			first = QString::fromStdWString(m_symbol);
		}
		else if (m_type == FieldProperties::Type::Percentage){
			value *= 100;
			last = "%";
		}
		else if (m_type == FieldProperties::Type::Datetime){
			qint64 ts = value;
			QDateTime* dt = new QDateTime();
			if (QDateTime::currentMSecsSinceEpoch() / 1000.0 < ts){
				dt->setMSecsSinceEpoch(ts);
			}
			else{
				dt->setMSecsSinceEpoch(ts * 1000.0);
			}
			return dt->toString(m_datetimeFmt);
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
		propertyTree.put(L"<xmlattr>.format", m_datetimeFmt.toStdWString());
	}

}
