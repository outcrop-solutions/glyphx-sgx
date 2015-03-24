#include "sourcedatacache.h"
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlField>
#include <QtSql/QSqlError>
#include <QtCore/QVariant>
#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include <boost/uuid/uuid_io.hpp>
#include "uuid.h"
#include "databaseinfo.h"

namespace SynGlyphX {

	const QString SourceDataCache::IndexColumnName = "SGXIndex";

	SourceDataCache::SourceDataCache() :
		CSVCache() {

	}

	SourceDataCache::SourceDataCache(const QString& filename) :
		CSVCache()
	{
		Setup(filename);
	}

	SourceDataCache::~SourceDataCache()
	{
		
	}

	bool SourceDataCache::IsValid() const {

		return CSVCache::IsValid();
	}

	void SourceDataCache::Close() {

		CSVCache::Close();
	}

	void SourceDataCache::Setup(const QString& filename) {

		CSVCache::Setup(filename);
		RebuildTableMap();
	}

	void SourceDataCache::RebuildTableMap() {

		m_tableIndexMap.clear();
		m_tableNameMap.clear();
		QStringList unorderedTables = DatabaseInfo::GetListOfTablesWithoutAutogeneratedTables(m_db);
		unorderedTables.removeAll(s_tableIndexName);
		Q_FOREACH(QString table, unorderedTables)  {

			AddTableToMap(table, GetFormattedNameFromCache(table));
		}
	}

	void SourceDataCache::AddDatasourcesToCache(const DatasourceMaps& datasources) {

		if (!IsValid()) {

			throw std::exception("Source data cache is not setup.");
		}

		for (auto fileDatasource : datasources.GetFileDatasources()) {

			AddFileDatasourceToCache(fileDatasource.first, fileDatasource.second);
		}
	}

	void SourceDataCache::AddFileDatasourceToCache(const boost::uuids::uuid& id, const FileDatasource& datasource) {
		
		try {

			if (datasource.IsOriginalDatasourceADatabase()) {

				QString datasourceId = QString::fromStdString(boost::uuids::to_string(id));
				Datasource::TableSet tablesInCache;
				for (auto cacheTable : m_tableIndexMap) {

					if (cacheTable.second.startsWith(datasourceId)) {

						tablesInCache.insert(cacheTable.second.mid(cacheTable.second.lastIndexOf(':') + 1).toStdWString());
					}
				}
				
				if (!tablesInCache.empty()) {

					QDateTime lastModifiedInCache = GetTimestampForTable(CreateTablename(datasourceId, QString::fromStdWString(*tablesInCache.begin())));
					QFileInfo datasourceFileInfo(QString::fromStdWString(datasource.GetFilename()));
					
					if ((tablesInCache != datasource.GetTables()) || (datasourceFileInfo.lastModified() > lastModifiedInCache)) {

						QStringList tablesToDelete;
						for (auto tableToDelete : tablesInCache) {

							tablesToDelete.push_back(CreateTablename(datasourceId, QString::fromStdWString(tableToDelete)));
						}
						DeleteTables(tablesToDelete);
					}
					else {

						//No need to update
						return;
					}
				}

				AddDBTablesToCache(id, datasource, "QSQLITE");
			}
			else if (datasource.GetType() == FileDatasource::CSV) {

				QString newTableName = QString::fromStdString(boost::uuids::to_string(id));
				UpdateCSVFile(newTableName, QString::fromStdWString(datasource.GetFilename()));
				try {

					AddTableToMap(newTableName, QString::fromStdWString(datasource.GetFormattedName()));
				}
				catch (const std::exception& e) {

					throw;
				}
			}
			else {

				throw std::exception("File datasource not supported");
			}
		}
		catch (const std::exception& e) {

			throw;
		}
	}

	void SourceDataCache::AddDBTablesToCache(const boost::uuids::uuid& id, const Datasource& datasource, const QString& dbType) {

		try {

			bool closeDatasource = false;
			QString dbConnectionID = QString::fromStdWString(boost::uuids::to_wstring(id));
			QSqlDatabase db;

			if (QSqlDatabase::contains(dbConnectionID)) {

				db = QSqlDatabase::database(dbConnectionID);
			}
			else {

				db = QSqlDatabase::addDatabase(dbType, dbConnectionID);
				db.setDatabaseName(QString::fromStdWString(datasource.GetDBName()));
				if (!db.open()) {

					throw std::exception("File source data source db failed to open.");
				}
				closeDatasource = true;
			}

			QString formattedName = QString::fromStdWString(datasource.GetFormattedName());
			for (const std::wstring& table : datasource.GetTables()) {

				QString qTable = QString::fromStdWString(table);
				AddDBTableToCache(db, qTable, formattedName, CreateTablename(dbConnectionID, qTable));
			}

			if (closeDatasource) {

				db.close();
				QSqlDatabase::removeDatabase(dbConnectionID);
			}
		}
		catch (const std::exception& e) {

			throw;
		}
	}

	void SourceDataCache::AddDBTableToCache(QSqlDatabase& db, const QString& sourceTable, const QString& formattedSourceName, const QString& cacheTable) {

		QString fieldNamesAndTypes;
		QString fieldNameList;
		//unsigned int fieldNameCount = 0;
		QSqlRecord sourceTableFieldNames = db.record(sourceTable);
		std::vector<bool> isTextField;
		isTextField.reserve(sourceTableFieldNames.count());
		for (int i = 0; i < sourceTableFieldNames.count(); ++i) {

			QString typeString;
			QSqlField field = sourceTableFieldNames.field(i);
			QVariant::Type fieldType = field.type();
			if ((fieldType == QVariant::Type::Char) || (fieldType == QVariant::Type::String) || (fieldType == QVariant::Type::Url)) {
				
				typeString = "TEXT";
			}
			else if (fieldType == QVariant::Type::Double) {
				
				typeString = "REAL";
			}
			else if ((fieldType == QVariant::Type::Int) || (fieldType == QVariant::Type::UInt) || (fieldType == QVariant::Type::LongLong) || (fieldType == QVariant::Type::ULongLong)) {
				
				typeString = "INTEGER";
			}
			else if ((fieldType == QVariant::Type::Date) || (fieldType == QVariant::Type::DateTime) || (fieldType == QVariant::Type::Time)) {
				
				typeString = "TEXT"; //Need to deal with DateTime types properly later
			}
			else {

				continue;
			}

			isTextField.push_back(typeString == "TEXT");
			fieldNameList += "\"" + field.name() + "\", ";
			fieldNamesAndTypes += "\"" + field.name() + "\" " + typeString + ",\n";
			//++fieldNameCount;
		}
		fieldNameList.chop(2);
		fieldNamesAndTypes.chop(2);

		if (isTextField.empty()) {

			return;
		}

		CreateNewIndexedTableInCache(cacheTable, fieldNamesAndTypes);

		/*QString bindString;
		for (int j = 0; j < fieldNameCount + 1; ++j) {

			bindString += "?, ";
		}
		bindString.chop(2);*/
		QSqlQuery insertIntoCacheQuery(m_db);
		QString insertIntoCacheQueryString = "INSERT INTO \"" + cacheTable + "\" (\"" + IndexColumnName + "\", " + fieldNameList + ") VALUES (%1);";

		unsigned long sgxIndex = 0;

		QSqlQuery getDataQuery(db);
		getDataQuery.prepare("SELECT " + fieldNameList + " FROM " + sourceTable);
		if (!getDataQuery.exec()) {

			throw std::exception((QObject::tr("Failed to extract source data: ") + m_db.lastError().text()).toStdString().c_str());
		}

		while (getDataQuery.next()) {

			QString dataToAdd = QString::number(sgxIndex);
			//insertIntoCacheQuery.bindValue(0, static_cast<qulonglong>(sgxIndex));
			for (int k = 0; k < isTextField.size(); ++k) {

				if (isTextField[k]) {

					dataToAdd += ", \"" + getDataQuery.value(k).toString() + "\"";
				}
				else {

					dataToAdd += ", " + getDataQuery.value(k).toString();
				}
				//insertIntoCacheQuery.bindValue(k + 1, getDataQuery.value(k));
			}
			insertIntoCacheQuery.prepare(insertIntoCacheQueryString.arg(dataToAdd));
			
			if (!insertIntoCacheQuery.exec()) {

				throw std::exception((QObject::tr("Failed to insert data into table: ") + m_db.lastError().text()).toStdString().c_str());
			}
			insertIntoCacheQuery.finish();
			++sgxIndex;
		}
		getDataQuery.finish();

		QString formattedTableName = CreateTablename(formattedSourceName, sourceTable);
		UpdateTimestampForTable(cacheTable, formattedTableName, QDateTime::currentDateTime());
		AddTableToMap(cacheTable, formattedTableName);
	}

	const SourceDataCache::TableIndexMap& SourceDataCache::GetTablesIndexMap() const {

		return m_tableIndexMap;
	}

	SourceDataCache::TableColumnSet SourceDataCache::GetColumnsForTable(const QString& table) const {

		TableColumnSet columnNames;
		QSqlRecord columns = m_db.record(table);
		for (int i = 0; i < columns.count(); ++i) {

			QString fieldName = columns.fieldName(i);
			if (fieldName != IndexColumnName) {

				columnNames.insert(fieldName);
			}
		}

		if (columnNames.empty()) {

			throw std::invalid_argument("Source Data Cache was asked for column names for a table that does not exist");
		}

		return columnNames;
	}

	int SourceDataCache::GetLastIndexOfTable(const QString& tableName) {

		QSqlQuery query(m_db);
		query.prepare("SELECT \"" + IndexColumnName + "\" FROM \"" + tableName + "\" ORDER BY \"" + IndexColumnName + "\" DESC LIMIT 1");
		if (query.exec()) {

			query.first();
			return query.value(0).toInt();
		}
		else {

			throw std::exception((QObject::tr("Table indexing failure: ") + m_db.lastError().text()).toStdString().c_str());
		}
	}

	void SourceDataCache::CreateNewIndexedTableInCache(const QString& name, const QString& fieldNamesAndTypes) {

		CreateNewTableInCache(name, "\"" + IndexColumnName + "\" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,\n" + fieldNamesAndTypes);
	}

	void SourceDataCache::AddTableToMap(const QString& tableName, const QString& formattedName) {

		unsigned int startingIndex = 0;
		if (!m_tableIndexMap.empty()) {

			startingIndex = m_tableIndexMap.rbegin()->first + 1;
		}

		m_tableIndexMap[startingIndex + GetLastIndexOfTable(tableName)] = tableName;
		m_tableNameMap[tableName] = formattedName;
	}

	QSqlQuery SourceDataCache::CreateSelectFieldQueryAscending(const InputField& inputfield) const {

		if (!IsInputfieldInCache(inputfield)) {

			throw std::invalid_argument("Can not create SQL query for input field that isn't in the source data cache");
		}

		QSqlQuery query(m_db);
		query.prepare("SELECT \"" + QString::fromStdWString(inputfield.GetField()) + "\" FROM \"" + CreateTablename(inputfield) + "\" ORDER BY \"" + IndexColumnName + "\" ASC");
		return query;
	}

	QString SourceDataCache::CreateTablename(const InputField& inputfield) const {

		return CreateTablename(QString::fromStdString(boost::uuids::to_string(inputfield.GetDatasourceID())), QString::fromStdWString(inputfield.GetTable()));
	}
		
	QString SourceDataCache::CreateTablename(const QString& datasourceID, const QString& originalTablename) const {

		return datasourceID + ":" + originalTablename;
	}

	QSqlQuery SourceDataCache::CreateMinMaxQuery(const InputField& inputfield) const {

		if (!IsInputfieldInCache(inputfield)) {

			throw std::invalid_argument("Can not create SQL query for input field that isn't in the source data cache");
		}

		QSqlQuery query(m_db);
		QString queryString = QString("SELECT MIN(%1), MAX(%1) FROM ").arg("\"" + QString::fromStdWString(inputfield.GetField()) + "\"") + "\"" + CreateTablename(inputfield) + "\"";
		query.prepare(queryString);

		return query;
	}

	bool SourceDataCache::IsInputfieldInCache(const InputField& inputfield) const {

		QString inputFieldTable = QString::fromStdWString(boost::uuids::to_wstring(inputfield.GetDatasourceID()));
		if (!inputfield.GetTable().empty()) {

			inputFieldTable += ":" + QString::fromStdWString(inputfield.GetTable());
		}

		for (auto table : m_tableIndexMap) {

			if (table.second == inputFieldTable) {

				return true;
			}
		}

		return false;
	}

	void SourceDataCache::DeleteTable(const QString& table) {

		CSVCache::DeleteTable(table);
		RebuildTableMap();
	}

	void SourceDataCache::DeleteTables(const QStringList& tables) {

		Q_FOREACH(const QString& table, tables) {

			CSVCache::DeleteTable(table);
		}
		RebuildTableMap();
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

			throw std::exception((QObject::tr("Failure to get formatted name from cache: ") + m_db.lastError().text()).toStdString().c_str());
		}
	}

	SourceDataCache::IndexSetMap SourceDataCache::SplitIndexSet(const IndexSet& indexSet) const {

		IndexSet::const_iterator iT = indexSet.begin();
		unsigned int sizeOfPreviousTables = 0;

		IndexSetMap indexSets;
		for (auto table : m_tableIndexMap) {

			std::set<unsigned long> indexSetForTable;
			while ((iT != indexSet.end()) && (*iT <= table.first)) {

				indexSetForTable.insert(*iT - sizeOfPreviousTables);
				++iT;
			}

			if (!indexSetForTable.empty()) {

				indexSets[table.second] = indexSetForTable;
			}

			sizeOfPreviousTables += table.first + 1;
		}

		return indexSets;
	}

	QSqlQuery SourceDataCache::CreateSelectQueryForIndexSet(const QString& tableName, const TableColumnSet& columns, const IndexSet& indexSet) const {

		TableColumnSet::const_iterator column = columns.begin();
		QString columnNameString = "\"" + *column;
		++column;
		for (; column != columns.end(); ++column) {

			columnNameString += "\", \"" + *column;
		}
		columnNameString += "\"";

		IndexSet::const_iterator iT = indexSet.begin();
		QString whereString = "WHERE \"" + IndexColumnName + "\" IN (" + QString::number(*iT);
		++iT;
		while (iT != indexSet.end()) {

			whereString += ", " + QString::number(*iT);
			++iT;
		}
		whereString += ")";

		QString queryString = "SELECT " + columnNameString + " FROM \"" + tableName + "\" " + whereString;

		QSqlQuery query(m_db);
		query.prepare(queryString);

		return query;
	}

} //namespace SynGlyphX
