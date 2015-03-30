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

#ifndef SYNGLYPHX_CSVCACHE_H
#define SYNGLYPHX_CSVCACHE_H

#include "sgxdatatransform_global.h"
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtSql/QSqlDatabase>
#include "csvfilereader.h"
#include "uuid.h"

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT CSVCache {

	public:
		CSVCache();
		CSVCache(const QString& cacheFilename);
		virtual ~CSVCache();

		virtual bool IsValid() const;
		void Setup(const QString& cacheFilename);
		virtual void Close();
		void UpdateCSVFile(const QString& tableName, const QString& csvFilename);

		const boost::uuids::uuid& GetConnectionID() const;

	protected:
		void SetPragmaValue(const QString& pragmaString);
		void CreateNewTableInCache(const QString& name, const QString& fieldNamesAndTypes);
		void CreateTableFromCSVHeaders(const QString& name, const CSVFileReader::CSVValues& headers, const CSVFileReader::CSVValues& types);
		virtual void DeleteTable(const QString& table);
		QDateTime GetTimestampForTable(const QString& table);
		void UpdateTimestampForTable(const QString& table, const QString& formattedName, const QDateTime& timestamp);

		boost::uuids::uuid m_connectionID;
		QSqlDatabase m_db;

		static const QString s_tableIndexName;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_CSVCACHE_H