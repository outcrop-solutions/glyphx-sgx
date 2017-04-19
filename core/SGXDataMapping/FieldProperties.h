///
/// SynGlyphX Holdings Incorporated ("COMPANY") CONFIDENTIAL
/// Copyright (c) 2013-2015 SynGlyphX Holdings Incorporated, All Rights Reserved.
///
/// NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
/// herein are proprietary to COMPANY and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
/// Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
/// from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed 
/// Confidentiality and Non-disclosure agreements explicitly covering such access.
///
/// The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes  
/// information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
/// OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE 
/// LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  
/// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
///
#pragma once
#ifndef SYNGLYPHX_FIELDPROPERTIES_H
#define SYNGLYPHX_FIELDPROPERTIES_H

#include "sgxdatamapping.h"
#include <string>
#include <boost/uuid/uuid_io.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/bimap.hpp>
#include <QtCore/QVariant>
#include <QtCore/QString>

namespace SynGlyphX {

	class SGXDATAMAPPING_API FieldProperties
	{

	public:
		enum Type {
			Default = 0,
			Number = 1,
			Currency = 2,
			Percentage = 3,
			Datetime = 4
		};

		FieldProperties(){};
		FieldProperties(boost::uuids::uuid id, std::wstring tbl, std::wstring fld, std::wstring type = L"Default", int dec = 0, std::wstring sym = L"", QString dFmt = "", QString tFmt = "");
		FieldProperties(const FieldProperties& fp);
		~FieldProperties(){};

		bool operator==(const FieldProperties& inputField) const;
		bool operator!=(const FieldProperties& inputField) const;

		void AddStatsToField(QStringList stats);
		void UpdateProperties(std::wstring type, int dec, std::wstring sym = L"", QString dFmt = "", QString tFmt = "");

		boost::uuids::uuid GetID() { return m_id; }
		std::wstring GetTable() { return m_table; }
		std::wstring GetField() { return m_field; }
		Type GetType() { return m_type; }
		int GetDecimalsToDisplay() { return m_decimals; }
		std::wstring GetSymbol() { return m_symbol; }
		QString DatetimeFormat() { return m_datetimeFmt; }
		QString DateFormat() { return m_dateFmt; }
		QString TimeFormat() { return m_timeFmt; }
		QString GetDefaultFieldType() { return m_stats.at(1); }
		QString GetMin() { return m_stats.at(2); }
		QString GetMax() { return m_stats.at(3); }
		QString GetSampleValue() { return GetMax(); }

		QString transformData(QString value);
		QString transformData(int value);
		QString transformData(double value);
		void ExportToPropertyTree(boost::property_tree::wptree& propertyTree, std::wstring hashid) const;

		static const boost::bimap<Type, std::wstring> s_fieldTypeStrings;

	private:
		boost::uuids::uuid m_id;
		std::wstring m_table;
		std::wstring m_field;
		Type m_type;
		int m_decimals;
		std::wstring m_symbol;
		QString m_datetimeFmt;
		QString m_dateFmt;
		QString m_timeFmt;
		QStringList m_stats;

	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_FIELDPROPERTIES_H
