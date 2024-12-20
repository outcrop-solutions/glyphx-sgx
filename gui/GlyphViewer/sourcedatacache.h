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

#include <QtCore/QSet>
#include <map>
#include "utilitytypes.h"
#include "inputfield.h"
#include <QtSql/QSqlQuery>
#include <memory>
#include <QtSql/QSqlTableModel>
#include <boost/bimap.hpp>
#include "filedatasource.h"
#include "datatransformmapping.h"
#include "interval.h"
#include "filteringparameters.h"

class SourceDataCache
{
public:
	typedef std::vector<std::pair<QString, SynGlyphX::InputField::Type>> TableColumns;
	typedef QSharedPointer<QSqlQuery> SharedSQLQuery;

	typedef std::shared_ptr<SourceDataCache> SharedPtr;
	typedef std::shared_ptr<const SourceDataCache> ConstSharedPtr;

	typedef std::map<QString, QString> TableNameMap;

	typedef std::vector<std::pair<QString, SynGlyphX::IndexSet>> DistinctValueIndexMap;

	static const QString s_emptyString;

	SourceDataCache();
	SourceDataCache(const QString& filename);
	virtual ~SourceDataCache();

	bool IsValid() const;
	void Close();

	void Setup(const QString& filename);

	const TableNameMap& GetFormattedNames() const;
	TableColumns GetColumnsForTable(const QString& table) const;
	TableColumns GetColumnsForTable(const boost::uuids::uuid& datasourceID, const QString& originalTablename) const;
	TableColumns GetColumnsForTable(const SynGlyphX::InputTable& inputTable) const;

	boost::uuids::uuid GetConnectionID() const;

	bool IsTableInCache(const boost::uuids::uuid& datasourceID, const QString& originalTablename) const;
	bool IsTableInCache(const SynGlyphX::InputTable& inputTable) const;

	SharedSQLQuery CreateSelectFieldQueryAscending(const SynGlyphX::InputField& inputField) const;
	SharedSQLQuery CreateMinMaxQuery(const SynGlyphX::InputField& inputField) const;
	SynGlyphX::DegenerateInterval GetMinMax(const SynGlyphX::InputField& inputField, const FilteringParameters::ColumnRangeFilterMap&  otherRanges = FilteringParameters::ColumnRangeFilterMap()) const;
	std::set<double> GetSortedNumericDistictValues(const SynGlyphX::InputField& inputField, const FilteringParameters::ColumnRangeFilterMap& otherRanges = FilteringParameters::ColumnRangeFilterMap()) const;
	QStringList GetSortedDistinctValuesAsStrings(const QString& tableName, const QString& columnName, const QString& whereClause = "") const;
		
	SharedSQLQuery CreateSelectQuery(const QString& tableName, const TableColumns& columns, const SynGlyphX::IndexSet& indexSet = SynGlyphX::IndexSet()) const;
	SharedSQLQuery CreateDistinctValueQuery(const QString& tableName, const QString& columnName, const SynGlyphX::IndexSet& indexSet = SynGlyphX::IndexSet()) const;
	SharedSQLQuery CreateDistinctValueAndCountQuery(const QString& tableName, const QString& columnName, const SynGlyphX::IndexSet& indexSet = SynGlyphX::IndexSet()) const;
	SharedSQLQuery CreateDistinctValueAndCountQuery(const QString& tableName, const QString& columnName, const QString& whereClause = "") const;
	SharedSQLQuery CreateOrderedValueAndRowQuery(const QString& tableName, const QString& columnName, const SynGlyphX::IndexSet& indexSet) const;
	unsigned long GetValueCount(const QString& tableName, const QString& columnName, const QString& value, const SynGlyphX::IndexSet& indexSet = SynGlyphX::IndexSet()) const;
	unsigned long GetNumberOfRowsInTable(const SynGlyphX::InputTable& inputTable) const;
	unsigned long GetNumberOfRowsInTable(const QString& table) const;
	SynGlyphX::IndexSet GetIndexesFromTableWithSelectedValues(const QString& tableName, const FilteringParameters::ColumnDistinctValuesFilterMap& selectedValues, const SynGlyphX::IndexSet& previousSelection = SynGlyphX::IndexSet()) const;
	SynGlyphX::IndexSet GetIndexesFromTableThatPassFilters(const QString& tableName, const FilteringParameters& filters) const;
	SynGlyphX::IndexSet GetIndexesBasedOnQuery(const QString& queryString) const;

	void CreateIndex(const QString& tableName, const QString& columnName) const;
	DistinctValueIndexMap GetIndexesOrderedByDistinctValue(const QString& tableName, const QString& columnName) const;

	static QString CreateTablename(const SynGlyphX::InputTable& inputTable);
	static QString CreateTablename(const QString& datasourceID, const QString& originalTablename);
	static QString CreateTablename(const boost::uuids::uuid& id, const QString& originalTablename);

	bool IsCacheOutOfDate(const SynGlyphX::DataTransformMapping::DatasourceMap& datasources) const;

	bool ExportFilteredDataToCSV(const QString& filename, const QString& tableName, const FilteringParameters& filters) const;

	static QString CreateEscapedString(const QString& string);
	static QString CreateInString(const QString& columnName, const QSet<QString>& values);

	std::vector<float> GetNumericValuesForField(const SynGlyphX::InputField& inputField) const;
	QList<QVariant> GetValuesForRow(const SynGlyphX::InputTable& table, const QStringList& fields, unsigned long row) const;

protected:
	SynGlyphX::InputField::Type GetSourceDataFieldType(QVariant::Type fieldType) const;
	int GetLastIndexOfTable(const QString& tableName);
		
	void AddTableToMap(const QString& tableName, const QString& formattedName);
	void RebuildTableMap();
	bool IsInputFieldInCache(const SynGlyphX::InputField& inputField) const;

	virtual void DeleteTable(const QString& table);
	void DeleteTables(const QStringList& tables);
	void DeleteTables(const boost::uuids::uuid& id);

	QString GetFormattedNameFromCache(const QString& table);

	QString CreateWhereString(const SynGlyphX::IndexSet& indexSet) const;
	QString CreateInString(const SynGlyphX::IndexSet& indexSet) const;
	QString CreateBetweenString(const QString& columnName, const SynGlyphX::DegenerateInterval& minMax) const;
	QString CreateBetweenString(const QString& columnName, const SynGlyphX::DegenerateIntervalUnion& intervals) const;
	QString CreateKeywordFilterString(const QString& columnName, const KeywordFilter& filter) const;
	QString CreateKeywordFilterString(const QString& columnName, const MultiKeywordFilter& filter) const;

	void SetPragmaValue(const QString& pragmaString);
	void CreateNewTable(const QString& name, const QString& fieldNamesAndTypes);
	void RunDeleteTableQuery(const QString& table);
	QStringList GetListOfTablesWithoutAutogeneratedTables();
	bool DoesFileDatasourceNeedUpdate(const boost::uuids::uuid& id, const SynGlyphX::FileDatasource& datasource) const;
	bool DoesFileDatabaseNeedUpdate(const boost::uuids::uuid& id, const SynGlyphX::FileDatasource& datasource) const;
	QDateTime GetTimestampForTable(const QString& table) const;

	QString CreateFilterString(const FilteringParameters& filters) const;

	TableNameMap m_tableNameMap;
	boost::uuids::uuid m_connectionID;
	QSqlDatabase m_db;

	static const QString IndexColumnName;
	static const QString s_tableIndexName;
};


#endif //SYNGLYPHX_SOURCEDATACACHE_H