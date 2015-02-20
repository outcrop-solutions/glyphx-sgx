#include "sourcedatacache.h"
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlField>
#include <QtSql/QSqlError>
#include <QtCore/QVariant>
#include <boost/uuid/uuid_io.hpp>
#include "uuid.h"

namespace SynGlyphX {

	const QString SourceDataCache::IndexColumnName = "SGXIndex";

	SourceDataCache::SourceDataCache() {

	}

	SourceDataCache::SourceDataCache(const QString& filename) :
		m_connectionID(QString::fromStdWString(boost::uuids::to_wstring(UUIDGenerator::GetNewRandomUUID())))
	{
		m_db = QSqlDatabase::addDatabase("QSQLITE", m_connectionID);
		m_db.setDatabaseName(filename);
		
		if (!m_db.open()) {

			throw std::exception("Source data cache db failed to open.");
		}

		CreateTableMap();
	}

	SourceDataCache::~SourceDataCache()
	{
		if (IsValid()) {

			m_db.close();
			QSqlDatabase::removeDatabase(m_connectionID);
		}
	}

	bool SourceDataCache::IsValid() const {

		return !m_connectionID.isEmpty();
	}

	void SourceDataCache::CreateCache(const DatasourceMaps& datasources, const QString& filename) {

		m_connectionID = QString::fromStdWString(boost::uuids::to_wstring(UUIDGenerator::GetNewRandomUUID()));
		m_db = QSqlDatabase::addDatabase("QSQLITE", m_connectionID);
		m_db.setDatabaseName(filename);

		if (!m_db.open()) {

			throw std::exception("Source data cache db failed to open.");
		}

		for (auto fileDatasource : datasources.GetFileDatasources()) {

			AddFileDatasourceToCache(fileDatasource.first, fileDatasource.second);
		}

		if (!m_db.commit()) {

			throw std::exception("Source data cache db failed to commit source data.");
		}
	}

	void SourceDataCache::AddFileDatasourceToCache(const boost::uuids::uuid& id, const FileDatasource& datasource) {

		bool closeDatasource = false;
		QString dbConnectionID = QString::fromStdWString(boost::uuids::to_wstring(id));
		QSqlDatabase db;
		
		if (QSqlDatabase::contains(dbConnectionID)) {

			db = QSqlDatabase::database(dbConnectionID);
		}
		else {

			db = QSqlDatabase::addDatabase("QSQLITE", dbConnectionID);
			db.setDatabaseName(QString::fromStdWString(datasource.GetFilename()));
			if (!db.open()) {

				throw std::exception("File source data source db failed to open.");
			}
			closeDatasource = true;
		}
		
		if (datasource.CanDatasourceHaveMultipleTables()) {

			for (const std::wstring& table : datasource.GetTables()) {

				QString qTable = QString::fromStdWString(table);
				AddTableToCache(db, qTable, QString::fromStdWString(datasource.GetFormattedName()) + ":" + qTable);
			}
		}
		else {

			AddTableToCache(db, QString::fromStdWString(*datasource.GetTables().begin()), QString::fromStdWString(datasource.GetFormattedName()));
		}

		if (closeDatasource) {

			db.close();
			QSqlDatabase::removeDatabase(dbConnectionID);
		}
	}

	void SourceDataCache::AddTableToCache(QSqlDatabase& db, const QString& sourceTable, const QString& cacheTable) {

		QString createTableQueryString = "CREATE TABLE " + cacheTable + " (\n";
		createTableQueryString += IndexColumnName + " INTEGER NOT NULL PRIMARY KEY";

		QString fieldNameList;
		unsigned int fieldNameCount = 0;
		QSqlRecord sourceTableFieldNames = db.record(sourceTable);
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

			fieldNameList += field.name() + ", ";
			createTableQueryString += ",\n" + field.name() + " " + typeString;
			++fieldNameCount;
		}
		createTableQueryString += "\n);";
		fieldNameList.chop(2);

		QSqlQuery createTableQuery;
		createTableQuery.prepare(createTableQueryString);
		bool querySucceeded = createTableQuery.exec();

		if (!querySucceeded) {

			throw std::exception(db.lastError().text().toStdString().c_str());
		}

		createTableQuery.finish();

		QString bindString;
		for (int j = 0; j < fieldNameCount; ++j) {

			bindString += "?, ";
		}
		bindString.chop(2);
		QSqlQuery insertIntoCacheQuery;
		insertIntoCacheQuery.prepare("INSERT INTO " + cacheTable + " (" + IndexColumnName + ", " + fieldNameList + ") VALUES (" + bindString + ");");

		unsigned long sgxIndex = 0;
		if (!m_tables.empty()) {

			sgxIndex = m_tables.rbegin()->first + 1;
		}

		QSqlQuery getDataQuery;
		getDataQuery.prepare("SELECT " + fieldNameList + " FROM " + sourceTable);
		getDataQuery.exec();
		while (getDataQuery.next()) {

			insertIntoCacheQuery.bindValue(0, static_cast<qulonglong>(sgxIndex));
			for (int k = 0; k < fieldNameCount; ++k) {

				insertIntoCacheQuery.bindValue(k + 1, getDataQuery.value(k));
			}

			insertIntoCacheQuery.exec();
			insertIntoCacheQuery.finish();
			++sgxIndex;
		}
		getDataQuery.finish();
		db.commit();
	}

	SourceDataCache::TableMap SourceDataCache::GetTables() const {

		return m_tables;
	}

	QStringList SourceDataCache::GetColumnsForTable(const QString& table) const {

		QStringList columnNames;
		QSqlRecord columns = m_db.record(table);
		for (int i = 0; i < columns.count(); ++i) {

			QString fieldName = columns.fieldName(i);
			if (fieldName != IndexColumnName) {

				columnNames.push_back(fieldName);
			}
		}

		if (columnNames.isEmpty()) {

			throw std::invalid_argument("Source Data Cache was asked for column names for a table that does not exist");
		}

		return columnNames;
	}

	/*QVariantList SourceDataCache::GetDataAtIndex(unsigned long index) const {


	}*/

	void SourceDataCache::CreateTableMap() {

		QStringList unorderedTables = SourceDataManager::GetListOfTablesWithoutAutogeneratedTables(m_db);

		QSqlQuery query(m_db);
		QString queryTemplate = "SELECT " + IndexColumnName + " FROM %1 ORDER BY " + IndexColumnName + " DESC LIMIT 1";
		Q_FOREACH(QString table, unorderedTables) {

			query.prepare(queryTemplate.arg(table));
			query.exec();
			query.first();

			m_tables[query.value(0).toUInt()] = table;
		}
	}

} //namespace SynGlyphX
