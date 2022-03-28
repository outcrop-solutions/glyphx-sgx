#include "sourcedatacache.h"
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlField>
#include <QtSql/QSqlError>
#include <QtCore/QVariant>
#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include <QtCore/QSharedPointer>
#include <boost/uuid/uuid_io.hpp>
#include "uuid.h"
#include "csvfilewriter.h"

const QString SourceDataCache::IndexColumnName = "rowid";
const QString SourceDataCache::s_tableIndexName = "TableIndex";
const QString SourceDataCache::s_emptyString = QChar(128);

SourceDataCache::SourceDataCache() {

}

SourceDataCache::SourceDataCache(const QString& filename)
{
	Setup(filename);
}

SourceDataCache::~SourceDataCache()
{
		
}

bool SourceDataCache::IsValid() const {

	return m_db.isOpen();
}

void SourceDataCache::Close() {

	if (!m_connectionID.is_nil()) {

		m_db.close();
		QSqlDatabase::removeDatabase(QString::fromStdWString(boost::uuids::to_wstring(m_connectionID)));
		m_connectionID = boost::uuids::uuid();
	}
	m_tableNameMap.clear();
}

void SourceDataCache::Setup(const QString& filename) {

	if (IsValid()) {

		Close();
	}

	m_connectionID = SynGlyphX::UUIDGenerator::GetNewRandomUUID();
	m_db = QSqlDatabase::addDatabase("QSQLITE", QString::fromStdWString(boost::uuids::to_wstring(m_connectionID)));
	m_db.setDatabaseName(filename);

	if (!m_db.open()) {

		Close();
		throw std::runtime_error("Cache db failed to open.");
	}

	SetPragmaValue("page_size = 4096");
	SetPragmaValue("cache_size = 16384");
	SetPragmaValue("temp_store = MEMORY");
	SetPragmaValue("journal_mode = OFF");
	SetPragmaValue("locking_mode = EXCLUSIVE");
	SetPragmaValue("synchronous = OFF");

	if (!m_db.tables().contains(s_tableIndexName)) {

		try {

			CreateNewTable(s_tableIndexName, "TableName TEXT NOT NULL PRIMARY KEY UNIQUE,\nFormattedName TEXT NOT NULL,\nTimestamp INTEGER NOT NULL");
		}
		catch (const std::exception&) {

			throw;
		}
	}

	RebuildTableMap();
}

void SourceDataCache::RebuildTableMap() {

	m_tableNameMap.clear();
	QSqlQuery query(m_db);
	query.prepare("SELECT TableName,FormattedName FROM \"" + s_tableIndexName + "\"");
	if (query.exec()) {

		while (query.next()) {

			AddTableToMap(query.value(0).toString(), query.value(1).toString());
		}

	}
	else {

		throw std::runtime_error((QObject::tr("Failure to get tables and formatted names from cache: ") + m_db.lastError().text()).toStdString().c_str());
	}
}

SourceDataCache::TableColumns SourceDataCache::GetColumnsForTable(const QString& table) const {

	TableColumns columnNames;
	QSqlRecord columns = m_db.record(table);
	columnNames.reserve(columns.count());
	for (int i = 0; i < columns.count(); ++i) {

		QSqlField field = columns.field(i);
		QString fieldName = field.name();
		if (fieldName != IndexColumnName) {

			columnNames.insert(columnNames.end(), std::pair<QString, SynGlyphX::InputField::Type>(fieldName, GetSourceDataFieldType(field.type())));
		}
	}

	if (columnNames.empty()) {

		throw std::invalid_argument("Source Data Cache was asked for column names for a table that does not exist");
	}

	return columnNames;
}

SourceDataCache::TableColumns SourceDataCache::GetColumnsForTable(const boost::uuids::uuid& datasourceID, const QString& originalTablename) const {

	QString tableName = CreateTablename(QString::fromStdString(boost::uuids::to_string(datasourceID)), originalTablename);
	return GetColumnsForTable(tableName);
}

SourceDataCache::TableColumns SourceDataCache::GetColumnsForTable(const SynGlyphX::InputTable& inputTable) const {

	return GetColumnsForTable(inputTable.GetDatasourceID(), QString::fromStdWString(inputTable.GetTable()));
}

boost::uuids::uuid SourceDataCache::GetConnectionID() const {

	return m_connectionID;
}

bool SourceDataCache::IsTableInCache(const boost::uuids::uuid& datasourceID, const QString& originalTablename) const {

	QString tableName = CreateTablename(QString::fromStdString(boost::uuids::to_string(datasourceID)), originalTablename);
	return (m_tableNameMap.count(tableName) != 0);
}

bool SourceDataCache::IsTableInCache(const SynGlyphX::InputTable& inputTable) const {

	return IsTableInCache(inputTable.GetDatasourceID(), QString::fromStdWString(inputTable.GetTable()));
}

int SourceDataCache::GetLastIndexOfTable(const QString& tableName) {

	QSqlQuery query(m_db);
	query.prepare("SELECT \"" + IndexColumnName + "\" FROM \"" + tableName + "\" ORDER BY \"" + IndexColumnName + "\" DESC LIMIT 1");
	if (query.exec()) {

		query.first();
		return query.value(0).toInt() - 1;
	}
	else {

		throw std::runtime_error((QObject::tr("Table indexing failure: ") + m_db.lastError().text()).toStdString().c_str());
	}
}

void SourceDataCache::AddTableToMap(const QString& tableName, const QString& formattedName) {

	m_tableNameMap[tableName] = formattedName;
}

SourceDataCache::SharedSQLQuery SourceDataCache::CreateSelectFieldQueryAscending(const SynGlyphX::InputField& inputfield) const {

	if (!IsInputFieldInCache(inputfield)) {

		throw std::invalid_argument("Can not create SQL query for input field that isn't in the source data cache");
	}

	SharedSQLQuery query(new QSqlQuery(m_db));
	query->prepare("SELECT \"" + QString::fromStdWString(inputfield.GetField()) + "\" FROM \"" + CreateTablename(inputfield) + "\" ORDER BY \"" + IndexColumnName + "\" ASC");
	return query;
}

QString SourceDataCache::CreateTablename(const SynGlyphX::InputTable& inputTable) {

	return CreateTablename(inputTable.GetDatasourceID(), QString::fromStdWString(inputTable.GetTable()));
}
		
QString SourceDataCache::CreateTablename(const QString& datasourceID, const QString& originalTablename) {

	if ((originalTablename.isEmpty()) || (originalTablename.toStdWString() == SynGlyphX::Datasource::SingleTableName)) {

		return datasourceID;
	}
	else {

		return datasourceID + ":" + originalTablename;
	}
}

QString SourceDataCache::CreateTablename(const boost::uuids::uuid& id, const QString& originalTablename) {

	return CreateTablename(QString::fromStdString(boost::uuids::to_string(id)), originalTablename);
}

SourceDataCache::SharedSQLQuery SourceDataCache::CreateMinMaxQuery(const SynGlyphX::InputField& inputfield) const {

	if (!IsInputFieldInCache(inputfield)) {

		throw std::invalid_argument("Can not create SQL query for input field that isn't in the source data cache");
	}

	SharedSQLQuery query(new QSqlQuery(m_db));
	QString queryString = QString("SELECT MIN(%1), MAX(%1) FROM ").arg("\"" + QString::fromStdWString(inputfield.GetField()) + "\"") + "\"" + CreateTablename(inputfield) + "\"";
	query->prepare(queryString);

	return query;
}

bool SourceDataCache::IsInputFieldInCache(const SynGlyphX::InputField& inputfield) const {

	QString inputFieldTable = QString::fromStdWString(boost::uuids::to_wstring(inputfield.GetDatasourceID()));
	if ((!inputfield.GetTable().empty()) && (inputfield.GetTable() != SynGlyphX::Datasource::SingleTableName)) {

		inputFieldTable += ":" + QString::fromStdWString(inputfield.GetTable());
	}

	return (m_tableNameMap.count(inputFieldTable) > 0);
}

void SourceDataCache::DeleteTable(const QString& table) {

	RunDeleteTableQuery(table);
	RebuildTableMap();
}

void SourceDataCache::DeleteTables(const QStringList& tables) {

	if (tables.isEmpty()) {

		return;
	}

	Q_FOREACH(const QString& table, tables) {

		RunDeleteTableQuery(table);
	}
	RebuildTableMap();
}

void SourceDataCache::DeleteTables(const boost::uuids::uuid& id) {

	QString datasourceId = QString::fromStdString(boost::uuids::to_string(id));
	QStringList tablesToDelete;
	for (auto table : m_tableNameMap) {

		if (table.first.startsWith(datasourceId)) {

			tablesToDelete.push_back(table.first);
		}
	}
	DeleteTables(tablesToDelete);
}

const SourceDataCache::TableNameMap& SourceDataCache::GetFormattedNames() const {

	return m_tableNameMap;
}

QString SourceDataCache::GetFormattedNameFromCache(const QString& table) {

	QSqlQuery query(m_db);
	query.prepare("SELECT \"FormattedName\" FROM \"" + s_tableIndexName + "\" WHERE \"TableName\"=\"" + table + "\"");
	if (query.exec()) {

		query.first();
		return query.value(0).toString();
	}
	else {

		throw std::runtime_error((QObject::tr("Failure to get formatted name from cache: ") + m_db.lastError().text()).toStdString().c_str());
	}
}

SourceDataCache::SharedSQLQuery SourceDataCache::CreateSelectQuery(const QString& tableName, const TableColumns& columns, const SynGlyphX::IndexSet& indexSet) const {

	TableColumns::const_iterator column = columns.begin();
	QString columnNameString = "\"" + column->first;
	++column;
	for (; column != columns.end(); ++column) {

		columnNameString += "\", \"" + column->first;
	}
	columnNameString += "\"";

	QString queryString = "SELECT " + columnNameString + " FROM \"" + tableName + "\" ";
	if (!indexSet.empty()) {

		queryString += CreateWhereString(indexSet);
	}

	SharedSQLQuery query(new QSqlQuery(m_db));
	query->prepare(queryString);

	return query;
}

SourceDataCache::SharedSQLQuery SourceDataCache::CreateDistinctValueQuery(const QString& tableName, const QString& columnName, const SynGlyphX::IndexSet& indexSet) const {

	QString queryString = "SELECT DISTINCT \"" + columnName + "\" FROM \"" + tableName + "\" ";

	if (!indexSet.empty()) {

		queryString += CreateWhereString(indexSet);
	}

	SharedSQLQuery query(new QSqlQuery(m_db));
	query->prepare(queryString);

	return query;
}

QStringList SourceDataCache::GetSortedDistinctValuesAsStrings(const QString& tableName, const QString& columnName, const QString& whereClause) const {

	QString queryString = "SELECT DISTINCT \"" + columnName + "\" FROM \"" + tableName + "\" ";

	if (!whereClause.isEmpty()) {

		queryString += "WHERE " + whereClause;
	}

	SharedSQLQuery distinctQuery(new QSqlQuery(m_db));
	distinctQuery->prepare(queryString);

	QStringList results;
	distinctQuery->exec();
	while (distinctQuery->next()) {

		results.push_back(distinctQuery->value(0).toString());
	}

	results.sort(Qt::CaseInsensitive);
	return results;
}

SourceDataCache::SharedSQLQuery SourceDataCache::CreateDistinctValueAndCountQuery(const QString& tableName, const QString& columnName, const SynGlyphX::IndexSet& indexSet) const {

	QString queryString = "SELECT \"" + columnName + "\", COUNT(*) FROM \"" + tableName + "\" ";

	if (!indexSet.empty()) {

		queryString += CreateWhereString(indexSet) + " ";
	}

	queryString += "GROUP BY \"" + columnName + "\" ";

	SharedSQLQuery query(new QSqlQuery(m_db));
	query->prepare(queryString);

	return query;
}

SourceDataCache::SharedSQLQuery SourceDataCache::CreateDistinctValueAndCountQuery(const QString& tableName, const QString& columnName, const QString& whereClause) const {

	QString queryString = "SELECT \"" + columnName + "\", COUNT(*) FROM \"" + tableName + "\" ";

	if (!whereClause.isEmpty()) {

		queryString += "WHERE " + whereClause + " ";
	}

	queryString += "GROUP BY \"" + columnName + "\" ";

	SharedSQLQuery query(new QSqlQuery(m_db));
	query->prepare(queryString);

	return query;
}

SourceDataCache::SharedSQLQuery SourceDataCache::CreateOrderedValueAndRowQuery(const QString& tableName, const QString& columnName, const SynGlyphX::IndexSet& indexSet) const {

	QString queryString = "SELECT \"" + columnName + "\", \"" + IndexColumnName + "\" FROM \"" + tableName + "\" ";

	if (!indexSet.empty()) {

		queryString += CreateWhereString(indexSet) + " ";
	}

	queryString += "ORDER BY \"" + columnName + "\" ASC";

	SharedSQLQuery query(new QSqlQuery(m_db));
	query->prepare(queryString);

	return query;
}

unsigned long SourceDataCache::GetValueCount(const QString& tableName, const QString& columnName, const QString& value, const SynGlyphX::IndexSet& indexSet) const {

	QString queryString = "SELECT COUNT(*) FROM \"" + tableName + "\" ";

	if (!indexSet.empty()) {

		queryString += CreateWhereString(indexSet) + " AND ";
	}
	else {

		queryString += " WHERE ";
	}

	queryString += "\"" + columnName + "\"=\"" + value + "\"";

	QSqlQuery query(m_db);
	query.prepare(queryString);
	query.exec();
	query.first();

	return query.value(0).toULongLong();
}

unsigned long SourceDataCache::GetNumberOfRowsInTable(const SynGlyphX::InputTable& inputTable) const {

	return GetNumberOfRowsInTable(CreateTablename(inputTable));
}

unsigned long SourceDataCache::GetNumberOfRowsInTable(const QString& table) const {

	QString queryString = "SELECT COUNT(*) FROM \"" + table + "\"";

	QSqlQuery query(m_db);
	query.prepare(queryString);
	query.exec();
	query.first();

	return query.value(0).toULongLong();
}

QString SourceDataCache::CreateWhereString(const SynGlyphX::IndexSet& indexSet) const {

	return "WHERE " + CreateInString(indexSet);
}

QString SourceDataCache::CreateInString(const SynGlyphX::IndexSet& indexSet) const {

	SynGlyphX::IndexSet::const_iterator iT = indexSet.begin();
	QString whereString = "\"" + IndexColumnName + "\" IN (" + QString::number(*iT + 1);
	++iT;
	while (iT != indexSet.end()) {

		whereString += ", " + QString::number(*iT + 1);
		++iT;
	}
	whereString += ")";

	return whereString;
}

QString SourceDataCache::CreateInString(const QString& columnName, const QSet<QString>& values) {

	QSet<QString>::const_iterator iT = values.begin();
	QString inString = "\"" + columnName + "\" IN (" + CreateEscapedString(*iT);
	++iT;
	while (iT != values.end()) {

		inString += ", " + CreateEscapedString(*iT);
		++iT;
	}

	inString += ")";

	return inString;
}

SynGlyphX::IndexSet SourceDataCache::GetIndexesFromTableWithSelectedValues(const QString& tableName, 
																		   const FilteringParameters::ColumnDistinctValuesFilterMap& selectedValues, 
																		   const SynGlyphX::IndexSet& previousSelection) const {

	QString queryString = "SELECT \"" + IndexColumnName + "\" FROM \"" + tableName + "\" WHERE ";

	FilteringParameters::ColumnDistinctValuesFilterMap::const_iterator iT = selectedValues.begin();
	queryString += CreateInString(iT->first, iT->second);
	++iT;
	while (iT != selectedValues.end()) {

		queryString += " AND " + CreateInString(iT->first, iT->second);
		++iT;
	}

	if (!previousSelection.empty()) {

		queryString += " AND " + CreateInString(previousSelection);
	}

	QSqlQuery query(m_db);
	query.prepare(queryString);
	query.exec();
	if (!query.exec()) {

		throw std::runtime_error((QObject::tr("Failed to get selected indexes from cache: ") + m_db.lastError().text()).toStdString().c_str());
	}

	SynGlyphX::IndexSet indexSet;
	while (query.next()) {

		indexSet.insert(query.value(0).toULongLong() - 1);
	}

	return indexSet;
}

QString SourceDataCache::CreateFilterString(const FilteringParameters& filters) const {

	QString filteringString = "WHERE ";

	const FilteringParameters::ColumnRangeFilterMap& rangeFilters = filters.GetRangeFilters();
	if (!rangeFilters.empty()) {

		FilteringParameters::ColumnRangeFilterMap::const_iterator rangeFilter = rangeFilters.begin();
		filteringString += CreateBetweenString(rangeFilter->first, rangeFilter->second);
		++rangeFilter;
		for (; rangeFilter != rangeFilters.end(); ++rangeFilter) {

			filteringString += " AND " + CreateBetweenString(rangeFilter->first, rangeFilter->second);
		}
	}

	const FilteringParameters::ColumnDistinctValuesFilterMap& distinctValueFilters = filters.GetDistinctValueFilters();
	if (!distinctValueFilters.empty()) {

		FilteringParameters::ColumnDistinctValuesFilterMap::const_iterator distinctValueFilter = distinctValueFilters.begin();
		if (!rangeFilters.empty()) {

			filteringString += " AND ";
		}
		filteringString += CreateInString(distinctValueFilter->first, distinctValueFilter->second);
		++distinctValueFilter;
		for (; distinctValueFilter != distinctValueFilters.end(); ++distinctValueFilter) {

			filteringString += " AND " + CreateInString(distinctValueFilter->first, distinctValueFilter->second);
		}
	}

	const FilteringParameters::ColumnKeywordFilterMap& keywordFilters = filters.GetKeywordFilters();
	if (!keywordFilters.empty()) {

		FilteringParameters::ColumnKeywordFilterMap::const_iterator keywordFilter = keywordFilters.begin();
		if (!rangeFilters.empty() || !distinctValueFilters.empty()) {

			filteringString += " AND ";
		}
		filteringString += CreateKeywordFilterString(keywordFilter->first, keywordFilter->second);
		++keywordFilter;
		for (; keywordFilter != keywordFilters.end(); ++keywordFilter) {

			filteringString += " AND " + CreateKeywordFilterString(keywordFilter->first, keywordFilter->second);
		}
	}

	return filteringString;
}

SynGlyphX::IndexSet SourceDataCache::GetIndexesFromTableThatPassFilters(const QString& tableName, const FilteringParameters& filters) const {

	if (!filters.HasFilters()) {

		throw std::invalid_argument("Can not get indexes without at least one filter.");
	}

	QString queryString = "SELECT \"" + IndexColumnName + "\" FROM \"" + tableName + "\" " + CreateFilterString(filters);

	QSqlQuery query(m_db);
	query.prepare(queryString);
	if (!query.exec()) {

		throw std::runtime_error((QObject::tr("Failed to get selected indexes from cache: ") + m_db.lastError().text()).toStdString().c_str());
	}

	SynGlyphX::IndexSet indexSet;
	while (query.next()) {

		indexSet.insert(query.value(0).toULongLong() - 1);
	}

	return indexSet;
}

SynGlyphX::IndexSet SourceDataCache::GetIndexesBasedOnQuery(const QString& queryString) const {

	QSqlQuery query(m_db);
	query.prepare(queryString);
	if (!query.exec()) {

		throw std::runtime_error((QObject::tr("Failed to get selected indexes from cache: ") + m_db.lastError().text()).toStdString().c_str());
	}

	SynGlyphX::IndexSet indexSet;
	while (query.next()) {

		indexSet.insert(query.value(0).toULongLong() - 1);
	}

	return indexSet;
}

QString SourceDataCache::CreateKeywordFilterString(const QString& columnName, const KeywordFilter& filter) const {

	QString filterString = "\"" + columnName + "\"";

	if (filter.GetCaseSensitive()) {

		filterString += " GLOB ";
	}
	else {

		filterString += " LIKE ";
	}

	if (filter.GetExactMatch()) {

		filterString += CreateEscapedString(filter.GetKeyword());
	}
	else {

		QString wildcard;
		if (filter.GetCaseSensitive()) {

			wildcard = '*';
		}
		else {

			wildcard = '%';
		}
		filterString += CreateEscapedString(wildcard + filter.GetKeyword() + wildcard);
	}

	return filterString;
}

QString SourceDataCache::CreateKeywordFilterString(const QString& columnName, const MultiKeywordFilter& filter) const {

	int numberOfKeywords = int(filter.size());
	if (numberOfKeywords == 0) {

		throw std::invalid_argument("Keyword filter string can not be created for zero keywords.");
	}
	else {

		QString keywordsString = CreateKeywordFilterString(columnName, filter[0]);
		if (numberOfKeywords != 1) {

			for (int i = 1; i < numberOfKeywords; ++i) {

				keywordsString += " OR " + CreateKeywordFilterString(columnName, filter[i]);
			}

			keywordsString.prepend("(");
			keywordsString.append(")");
		}

		return keywordsString;
	}
}

SynGlyphX::DegenerateInterval SourceDataCache::GetMinMax(const SynGlyphX::InputField& inputfield, const FilteringParameters::ColumnRangeFilterMap& otherRanges) const {

	if (!inputfield.IsNumeric()) {

		throw std::invalid_argument("SourceDataCache can not get numeric min and max for a field that is not numeric.");
	}

	SharedSQLQuery query(new QSqlQuery(m_db));
	QString queryString = QString("SELECT MIN(%1), MAX(%1) FROM ").arg("\"" + QString::fromStdWString(inputfield.GetField()) + "\"") + "\"" + CreateTablename(inputfield) + "\"";

	if (!otherRanges.empty()) {

		queryString += " WHERE ";
		FilteringParameters::ColumnRangeFilterMap::const_iterator otherRange = otherRanges.begin();
		queryString += CreateBetweenString(otherRange->first, otherRange->second);

		++otherRange;
		for (; otherRange != otherRanges.end(); ++otherRange) {

			queryString += " AND " + CreateBetweenString(otherRange->first, otherRange->second);
		}
	}

	query->prepare(queryString);
	query->exec();
	query->first();

	return SynGlyphX::DegenerateInterval(query->value(0).toDouble(), query->value(1).toDouble());
}

std::set<double> SourceDataCache::GetSortedNumericDistictValues(const SynGlyphX::InputField& inputField, const FilteringParameters::ColumnRangeFilterMap& otherRanges) const {

	if (!inputField.IsNumeric()) {

		throw std::invalid_argument("SourceDataCache can not get numeric min and max for a field that is not numeric.");
	}

	QString field = "\"" + QString::fromStdWString(inputField.GetField()) + "\"";
	QString table = "\"" + CreateTablename(inputField) + "\"";

	QSqlQuery query(m_db);
	QString queryString = QString("SELECT DISTINCT %1 FROM %2").arg(field).arg(table);

	if (!otherRanges.empty()) {

		queryString += " WHERE ";
		FilteringParameters::ColumnRangeFilterMap::const_iterator otherRange = otherRanges.begin();
		queryString += CreateBetweenString(otherRange->first, otherRange->second);

		++otherRange;
		for (; otherRange != otherRanges.end(); ++otherRange) {

			queryString += " AND " + CreateBetweenString(otherRange->first, otherRange->second);
		}
	}
	queryString += QString(" ORDER BY %1 ASC").arg(field);

	std::set<double> sortedDistinctValues;

	query.prepare(queryString);
	if (!query.exec()) {

		throw std::runtime_error((QObject::tr("Failed to get sorted numeric distinct values: ") + m_db.lastError().text()).toStdString().c_str());
	}
	while (query.next()) {

		sortedDistinctValues.insert(sortedDistinctValues.end(), query.value(0).toDouble());
	}

	return sortedDistinctValues;
}

QString SourceDataCache::CreateBetweenString(const QString& columnName, const SynGlyphX::DegenerateInterval& minMax) const {

	return ("(\"" + columnName + "\" BETWEEN " + QString::number(minMax.GetMin(), 'f') + " AND " + QString::number(minMax.GetMax(), 'f') + ")");
}

QString SourceDataCache::CreateBetweenString(const QString& columnName, const SynGlyphX::DegenerateIntervalUnion& intervals) const {

	int numberOfIntervals = int(intervals.size());
	if (numberOfIntervals == 0) {

		throw std::invalid_argument("Between string can not be created for zero intervals.");
	}
	else {

		QString intervalsString = CreateBetweenString(columnName, intervals[0]);
		if (numberOfIntervals != 1) {

			for (int i = 1; i < numberOfIntervals; ++i) {

				intervalsString += " OR " + CreateBetweenString(columnName, intervals[i]);
			}

			intervalsString.prepend("(");
			intervalsString.append(")");
		}

		return intervalsString;
	}
}

void SourceDataCache::CreateIndex(const QString& tableName, const QString& columnName) const {

	QSqlQuery().exec("DROP INDEX IF EXISTS tf_index");
	QString queryString = "CREATE INDEX tf_index ON \"" + tableName + "\" (\"" + columnName + "\")";
	QSqlQuery query(m_db);
	query.prepare(queryString);
	query.exec();
}

SourceDataCache::DistinctValueIndexMap SourceDataCache::GetIndexesOrderedByDistinctValue(const QString& tableName, const QString& columnName) const {

	DistinctValueIndexMap distinctValueIndexMap;

	QString queryString = "SELECT \"" + columnName + "\", \"" + IndexColumnName + "\" FROM \"" + tableName + "\" ORDER BY \"" + columnName + "\" ASC";
	QSqlQuery query(m_db);
	query.prepare(queryString);
	query.exec();
	if (!query.exec()) {

		throw std::runtime_error((QObject::tr("Failed to get ordered indexes: ") + m_db.lastError().text()).toStdString().c_str());
	}

	QString previousDistinctValue(QString::null);
	while (query.next()) {

		QString currentDistinctValue = query.value(0).toString();
		if (currentDistinctValue != previousDistinctValue) {

			previousDistinctValue = currentDistinctValue;
			distinctValueIndexMap.push_back({ currentDistinctValue, SynGlyphX::IndexSet() });
		}

		distinctValueIndexMap.rbegin()->second.insert(query.value(1).toULongLong() - 1);
	}
	query.finish();

	return distinctValueIndexMap;
}

SynGlyphX::InputField::Type SourceDataCache::GetSourceDataFieldType(QVariant::Type fieldType) const {

	if ((fieldType == QVariant::Type::Char) || (fieldType == QVariant::Type::String) || (fieldType == QVariant::Type::Url)) {

		return SynGlyphX::InputField::Type::Text;
	}
	else if ((fieldType == QVariant::Type::Double) || (fieldType == QVariant::Type::Int) || (fieldType == QVariant::Type::UInt) || (fieldType == QVariant::Type::LongLong) || (fieldType == QVariant::Type::ULongLong)) {

		return SynGlyphX::InputField::Type::Real;
	}
	else if ((fieldType == QVariant::Type::Date) || (fieldType == QVariant::Type::DateTime) || (fieldType == QVariant::Type::Time)) {

		return SynGlyphX::InputField::Type::DateTime;
	}
	else {

		return SynGlyphX::InputField::Type::Other;
	}
}

void SourceDataCache::SetPragmaValue(const QString& pragmaString) {

	QSqlQuery pragmaQuery(m_db);
	pragmaQuery.prepare("PRAGMA " + pragmaString);
	if (!pragmaQuery.exec()) {

		throw std::runtime_error(("Pragma Error: " + m_db.lastError().text()).toStdString().c_str());
	}
	pragmaQuery.finish();
}

void SourceDataCache::CreateNewTable(const QString& name, const QString& fieldNamesAndTypes) {

	QString createTableQueryString = "CREATE TABLE \"" + name + "\" (\n" + fieldNamesAndTypes + "\n);";

	QSqlQuery createTableQuery(m_db);
	createTableQuery.prepare(createTableQueryString);
	bool querySucceeded = createTableQuery.exec();

	if (!querySucceeded) {

		throw std::runtime_error((QObject::tr("Create table in CSV cache failed: ") + m_db.lastError().text()).toStdString().c_str());
	}

	createTableQuery.finish();
}

void SourceDataCache::RunDeleteTableQuery(const QString& table) {

	QSqlQuery dropTableQuery(m_db);
	dropTableQuery.prepare("DROP TABLE \"" + table + "\"");
	bool querySucceeded = dropTableQuery.exec();

	if (!querySucceeded) {

		throw std::runtime_error((QObject::tr("Delete table in CSV cache failed: ") + m_db.lastError().text()).toStdString().c_str());
	}

	dropTableQuery.finish();
}

QStringList SourceDataCache::GetListOfTablesWithoutAutogeneratedTables() {

	QStringList tableList = m_db.tables(QSql::Tables);
	tableList.append(m_db.tables(QSql::Views));

	if (m_db.driverName() == "QSQLITE") {

		tableList.removeAll("sqlite_sequence");
	}

	return tableList;
}

bool SourceDataCache::IsCacheOutOfDate(const SynGlyphX::DataTransformMapping::DatasourceMap& datasources) const {

	if (!IsValid()) {

		throw std::runtime_error("Source data cache is not setup.");
	}

	for (auto datasource : datasources) {

		if (datasource.second->GetSourceType() == SynGlyphX::Datasource::SourceType::File) {

			if (DoesFileDatasourceNeedUpdate(datasource.first, *std::dynamic_pointer_cast<SynGlyphX::FileDatasource>(datasource.second))) {

				return true;
			}
		}
		else if (datasource.second->GetSourceType() == SynGlyphX::Datasource::SourceType::DatabaseServer) {

			return true; //Not sure how to check if RDBMS tables need update so just return true automatically for now
		}
	}

	return false;
}

bool SourceDataCache::DoesFileDatasourceNeedUpdate(const boost::uuids::uuid& id, const SynGlyphX::FileDatasource& datasource) const {

	if (!datasource.RequiresConversionToDB()) {

		return DoesFileDatabaseNeedUpdate(id, datasource);
	}
	else if (datasource.GetFileType() == SynGlyphX::FileDatasource::CSV) {

		QString tableName = QString::fromStdString(boost::uuids::to_string(id));
		QString csvFilename = QString::fromStdWString(datasource.GetFilename());
		QFileInfo datasourceFileInfo(QString::fromStdWString(datasource.GetFilename()));

		if (!IsTableInCache(id, "")) {

			return true;
		}

		QDateTime tableLastModified = GetTimestampForTable(tableName);
		if (datasourceFileInfo.lastModified().toUTC() > tableLastModified) {

			return true;
		}
	}
	return false;
}

bool SourceDataCache::DoesFileDatabaseNeedUpdate(const boost::uuids::uuid& id, const SynGlyphX::FileDatasource& datasource) const {

	QString datasourceId = QString::fromStdString(boost::uuids::to_string(id));
	SynGlyphX::Datasource::TableNames tablesInCache;
	for (auto cacheTable : m_tableNameMap) {

		if (cacheTable.first.startsWith(datasourceId)) {

			const QString& tableName = cacheTable.first;
			tablesInCache.push_back(tableName.mid(tableName.lastIndexOf(':') + 1).toStdWString());
		}
	}

	if (tablesInCache.empty()) {

		return true;
	}
	else {

		QDateTime lastModifiedInCache = GetTimestampForTable(CreateTablename(datasourceId, QString::fromStdWString(*tablesInCache.begin())));
		QFileInfo datasourceFileInfo(QString::fromStdWString(datasource.GetFilename()));

		if ((tablesInCache != datasource.GetTableNames()) || (datasourceFileInfo.lastModified().toUTC() > lastModifiedInCache)) {

			return true;
		}
	}

	return false;
}

QDateTime SourceDataCache::GetTimestampForTable(const QString& table) const {

	QSqlQuery timestampQuery(m_db);
	timestampQuery.prepare("SELECT \"Timestamp\" FROM \"" + s_tableIndexName + "\" WHERE TableName=\"" + table + "\";");
	bool querySucceeded = timestampQuery.exec();

	if (!querySucceeded) {

		throw std::runtime_error((QObject::tr("Get timestamp in CSV cache failed: ") + m_db.lastError().text()).toStdString().c_str());
	}

	timestampQuery.first();
	qint64 timestamp = timestampQuery.value(0).toULongLong();
	timestampQuery.finish();
	return QDateTime::fromMSecsSinceEpoch(timestamp);
}

QString SourceDataCache::CreateEscapedString(const QString& string) {

	QString escapedString = string;
	escapedString.replace('\'', "\'\'");
	escapedString.prepend('\'');
	escapedString.append('\'');

	return escapedString;
}

bool SourceDataCache::ExportFilteredDataToCSV(const QString& filename, const QString& tableName, const FilteringParameters& filters) const {

	QStringList columnNames;
	QSqlRecord columns = m_db.record(tableName);
	for (int i = 0; i < columns.count(); ++i) {

		columnNames.push_back("\"" + columns.field(i).name() + "\"");
	}

	if (columnNames.isEmpty()) {

		throw std::invalid_argument("Source Data Cache was asked for column names for a table that does not exist");
	}

	QString queryString = "SELECT " + columnNames.join(", ") + " FROM \"" + tableName + "\" ";
	if (filters.HasFilters()) {

		queryString += CreateFilterString(filters);
	}

	QSqlQuery query(m_db);
	query.prepare(queryString);
	if (!query.exec()) {

		throw std::runtime_error((QObject::tr("Failed to get filtered data from cache: ") + m_db.lastError().text()).toStdString().c_str());
	}

	SynGlyphX::CSVFileWriter csvFile(filename.toStdString().c_str());

	SynGlyphX::CSVFileHandler::CSVValues headers;
	for (const auto& columnName : columnNames) {

		headers.push_back(columnName.toStdWString());
	}
	csvFile.WriteLine(headers);

	bool wereAnyValuesWrittenToFile = false;
	while (query.next()) {

		SynGlyphX::CSVFileHandler::CSVValues lineOfValues;
		for (int i = 0; i < columnNames.size(); ++i) {

			QVariant var = query.value(i);
			if (!var.isValid()) {

				lineOfValues.push_back(L"\" \"");
			}
			else if ((var.type() == QVariant::Char) || (var.type() == QVariant::String)) {

				lineOfValues.push_back(L"\"" + var.toString().toStdWString() + L"\"");
			}
			else {

				lineOfValues.push_back(var.toString().toStdWString());
			}
		}

		csvFile.WriteLine(lineOfValues);
		wereAnyValuesWrittenToFile = true;
	}

	csvFile.Close();
	return wereAnyValuesWrittenToFile;
}

std::vector<float> SourceDataCache::GetNumericValuesForField(const SynGlyphX::InputField& inputField) const {

	if (!inputField.IsNumeric()) {

		throw std::invalid_argument("SourceDataCache can not get numeric values for a field that is not numeric.");
	}

	QString cacheTable = CreateTablename(inputField);

	std::vector<float> values;

	QSqlQuery query(m_db);
	query.prepare("SELECT \"" + QString::fromStdWString(inputField.GetField()) + "\" FROM \"" + cacheTable + "\" ORDER BY \"" + IndexColumnName + "\" ASC");
	if (query.exec()) {

		while (query.next()) {

			values.push_back(query.value(0).toFloat());
		}

		return values;
	}
	else {

		throw std::runtime_error("SourceDataCache failed to get numeric values in GetNumericValuesForField.");
	}
}

QList<QVariant> SourceDataCache::GetValuesForRow(const SynGlyphX::InputTable& table, const QStringList& fields, unsigned long row) const {

	QString cacheTable = CreateTablename(table);

	QList<QVariant> values;

	QString queryString = "SELECT ";
	if (fields.size() == 0) {
		queryString += "*";
	}
	else {
		for (const auto& field : fields) {

			if (!field.isEmpty()) {

				queryString += "\"" + field + "\"";
				if (field != fields.last()) {

					queryString += ", ";
				}
			}
		}
	}

	SynGlyphX::IndexSet rows;
	rows.insert(row);
	queryString += " FROM \"" + cacheTable + "\" " + CreateWhereString(rows);

	QSqlQuery query(m_db);
	query.prepare(queryString);
	if (query.exec()) {

		query.first();
		for (int i = 0; i < fields.size(); ++i) {

			values.push_back(query.value(i));
		}

		return values;
	}
	else {

		throw std::runtime_error("SourceDataCache failed to get fields at given row.");
	}
}