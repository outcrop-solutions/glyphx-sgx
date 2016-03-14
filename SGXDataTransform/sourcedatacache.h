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
#include <map>
#include "utilitytypes.h"
#include "databaseinfo.h"
#include "inputfield.h"
#include <QtSql/QSqlQuery>
#include <memory>
#include <QtSql/QSqlTableModel>
#include <boost/bimap.hpp>
#include "filedatasource.h"
#include "datatransformmapping.h"

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT SourceDataCache : protected CSVCache
	{
	public:
		typedef std::shared_ptr<SourceDataCache> SharedPtr;
		typedef std::shared_ptr<const SourceDataCache> ConstSharedPtr;

		typedef std::map<QString, QString> TableNameMap;

		typedef std::vector<std::pair<QString, IndexSet>> DistinctValueIndexMap;

		typedef std::map<QString, std::set<QString>> ColumnValueData;
		typedef std::multimap<QString, std::pair<double, double>> ColumnMinMaxMap;

		SourceDataCache();
		SourceDataCache(const QString& filename);
		virtual ~SourceDataCache();

		virtual bool IsValid() const;
		virtual void Close();

		virtual void Setup(const QString& filename);
		//void AddDatasourcesToCache(const DatasourceMaps& datasources);
		void AddFileDatasourceToCache(const boost::uuids::uuid& id, const FileDatasource& datasource);

		const TableNameMap& GetFormattedNames() const;
		TableColumns GetColumnsForTable(const QString& table) const;
		TableColumns GetColumnsForTable(const boost::uuids::uuid& datasourceID, const QString& originalTablename) const;

		bool IsTableInCache(const boost::uuids::uuid& datasourceID, const QString& originalTablename) const;

		SharedSQLQuery CreateSelectFieldQueryAscending(const InputField& inputfield) const;
		SharedSQLQuery CreateMinMaxQuery(const InputField& inputfield) const;
		
		SharedSQLQuery CreateSelectQueryForIndexSet(const QString& tableName, const TableColumns& columns, const IndexSet& indexSet) const;
		SharedSQLQuery CreateDistinctValueQuery(const QString& tableName, const QString& columnName, const IndexSet& indexSet = IndexSet()) const;
		SharedSQLQuery CreateDistinctValueAndCountQuery(const QString& tableName, const QString& columnName, const IndexSet& indexSet = IndexSet()) const;
		SharedSQLQuery CreateOrderedValueAndRowQuery(const QString& tableName, const QString& columnName, const IndexSet& indexSet) const;
		unsigned long GetValueCount(const QString& tableName, const QString& columnName, const QString& value, const IndexSet& indexSet = IndexSet()) const;
		unsigned long GetNumberOfRowsInTable(const InputTable& inputTable) const;
		unsigned long GetNumberOfRowsInTable(const QString& table) const;
		IndexSet GetIndexesFromTableWithSelectedValues(const QString& tableName, const ColumnValueData& selectedValues, const IndexSet& previousSelection = IndexSet()) const;
		IndexSet GetIndexesFromTableInRanges(const QString& tableName, const ColumnMinMaxMap& columnRanges) const;

		bool IsCacheOutOfDate(const DataTransformMapping::DatasourceMap& datasources) const;

		DistinctValueIndexMap GetIndexesOrderedByDistinctValue(const QString& tableName, const QString& columnName) const;

		static QString CreateTablename(const InputTable& inputTable);
		static QString CreateTablename(const QString& datasourceID, const QString& originalTablename);

	private:
		SourceDataFieldType GetSourceDataFieldType(QVariant::Type fieldType) const;
		virtual void CreateNewCacheTable(const QString& name, const QString& fieldNamesAndTypes);
		int GetLastIndexOfTable(const QString& tableName);
		
		void AddDBTablesToCache(const boost::uuids::uuid& id, const Datasource& datasource, const QString& dbType);
		void AddDBTableToCache(QSqlDatabase& db, const QString& sourceTable, const QString& formattedSourceName, const QString& cacheTable);
		void AddTableToMap(const QString& tableName, const QString& formattedName);
		void RebuildTableMap();
		bool IsInputfieldInCache(const InputField& inputfield) const;

		virtual void DeleteTable(const QString& table);
		void DeleteTables(const QStringList& tables);
		void DeleteTables(const boost::uuids::uuid& id);

		QString GetFormattedNameFromCache(const QString& table);

		QString CreateWhereString(const IndexSet& indexSet) const;
		QString CreateInString(const IndexSet& indexSet) const;
		QString CreateInString(const QString& columnName, const std::set<QString>& values) const;
		QString CreateBetweenString(const QString& columnName, const std::pair<double, double>& minMax) const;

		bool DoesFileDatasourceNeedUpdate(const boost::uuids::uuid& id, const FileDatasource& datasource) const;
		bool DoesFileDatabaseNeedUpdate(const boost::uuids::uuid& id, const FileDatasource& datasource) const;

		static const QString IndexColumnName;

		TableNameMap m_tableNameMap;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_SOURCEDATACACHE_H