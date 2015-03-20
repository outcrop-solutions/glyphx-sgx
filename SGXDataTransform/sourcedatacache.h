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
#include <set>
#include "inputfield.h"
#include <QtSql/QSqlQuery>
#include <memory>
#include <QtSql/QSqlTableModel>

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT SourceDataCache : protected CSVCache
	{
	public:
		typedef std::shared_ptr<SourceDataCache> SharedPtr;
		typedef std::shared_ptr<const SourceDataCache> ConstSharedPtr;

		typedef std::map<unsigned long, QString> TableIndexMap;
		typedef std::map<QString, QString> TableNameMap;

		typedef std::set<unsigned long> IndexSet;
		typedef std::map<QString, IndexSet> IndexSetMap;

		SourceDataCache();
		SourceDataCache(const QString& filename);
		virtual ~SourceDataCache();

		virtual bool IsValid() const;
		virtual void Close();

		void Setup(const QString& filename);
		void AddDatasourcesToCache(const DatasourceMaps& datasources);
		void AddFileDatasourceToCache(const boost::uuids::uuid& id, const FileDatasource& datasource);

		const TableIndexMap& GetTablesIndexMap() const;
		const TableNameMap& GetFormattedNames() const;
		QStringList GetColumnsForTable(const QString& table) const;

		QSqlQuery CreateSelectFieldQueryAscending(const InputField& inputfield) const;
		QSqlQuery CreateMinMaxQuery(const InputField& inputfield) const;
		
		IndexSetMap SplitIndexSet(const IndexSet& indexSet) const;
		QSqlQuery CreateSelectQueryForIndexSet(const QString& tableName, const QStringList& columns, const IndexSet& indexSet) const;

	private:
		void CreateNewIndexedTableInCache(const QString& name, const QString& fieldNamesAndTypes);
		int GetLastIndexOfTable(const QString& tableName);
		
		void AddDBTablesToCache(const boost::uuids::uuid& id, const Datasource& datasource, const QString& dbType);
		void AddDBTableToCache(QSqlDatabase& db, const QString& sourceTable, const QString& formattedSourceName, const QString& cacheTable);
		void AddTableToMap(const QString& tableName, const QString& formattedName);
		void RebuildTableMap();
		bool IsInputfieldInCache(const InputField& inputfield) const;

		virtual void DeleteTable(const QString& table);
		void DeleteTables(const QStringList& tables);

		QString CreateTablename(const InputField& inputfield) const;
		QString CreateTablename(const QString& datasourceID, const QString& originalTablename) const;
		QString GetFormattedNameFromCache(const QString& table);

		static const QString IndexColumnName;

		TableIndexMap m_tableIndexMap;
		TableNameMap m_tableNameMap;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_SOURCEDATACACHE_H