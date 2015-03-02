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

#ifndef SYNGLYPHX_SOURCEDATACACHE_H
#define SYNGLYPHX_SOURCEDATACACHE_H

#include "sgxdatatransform_global.h"
#include "csvcache.h"
#include "datasourcemaps.h"
#include <map>

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT SourceDataCache : protected CSVCache
	{
	public:
		typedef std::map<unsigned long, QString> TableMap;

		SourceDataCache();
		SourceDataCache(const QString& filename);
		virtual ~SourceDataCache();

		virtual bool IsValid() const;

		void Setup(const QString& filename);
		void AddDatasourcesToCache(const DatasourceMaps& datasources);
		void AddFileDatasourceToCache(const boost::uuids::uuid& id, const FileDatasource& datasource);

		TableMap GetTables() const;
		QStringList GetColumnsForTable(const QString& table) const;
		//QVariantList GetDataAtIndex(unsigned long index) const;

	private:
		virtual void CreateNewTableInCache(const QString& name, const QString& fieldNamesAndTypes);
		int GetLastIndexOfTable(const QString& tableName);
		
		void AddDBTablesToCache(const boost::uuids::uuid& id, const Datasource& datasource, const QString& dbType);
		void AddDBTableToCache(QSqlDatabase& db, const QString& sourceTable, const QString& cacheTable);
		void AddTableToMap(const QString& tableName);

		static const QString IndexColumnName;

		TableMap m_tables;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_SOURCEDATACACHE_H