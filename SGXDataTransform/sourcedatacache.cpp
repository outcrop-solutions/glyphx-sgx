#include "sourcedatacache.h"
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlField>
#include <QtSql/QSqlError>
#include <QtCore/QVariant>
#include <QtCore/QDateTime>
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
		QStringList unorderedTables = DatabaseInfo::GetListOfTablesWithoutAutogeneratedTables(m_db);
		Q_FOREACH(QString table, unorderedTables)  {

			AddTableToMap(table);
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

				AddDBTablesToCache(id, datasource, "QSQLITE");
				CommitChanges();
			}
			else if (datasource.GetType() == FileDatasource::CSV) {

				QString newTableName = QString::fromStdString(boost::uuids::to_string(id));
				UpdateCSVFile(newTableName, QString::fromStdWString(datasource.GetFilename()));
				try {

					AddTableToMap(newTableName);
				}
				catch (const std::exception& e) {

					m_db.rollback();
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

			for (const std::wstring& table : datasource.GetTables()) {

				QString qTable = QString::fromStdWString(table);
				AddDBTableToCache(db, qTable, dbConnectionID + ":" + qTable);
			}

			if (closeDatasource) {

				db.close();
				QSqlDatabase::removeDatabase(dbConnectionID);
			}
		}
		catch (const std::exception& e) {

			m_db.rollback();
			throw;
		}
	}

	void SourceDataCache::AddDBTableToCache(QSqlDatabase& db, const QString& sourceTable, const QString& formattedSourceName, const QString& cacheTable) {

		QString fieldNamesAndTypes;
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
			fieldNamesAndTypes += ",\n" + field.name() + " " + typeString;
			++fieldNameCount;
		}
		fieldNameList.chop(2);

		CreateNewTableInCache(cacheTable, fieldNamesAndTypes);

		QString bindString;
		for (int j = 0; j < fieldNameCount; ++j) {

			bindString += "?, ";
		}
		bindString.chop(2);
		QSqlQuery insertIntoCacheQuery(m_db);
		insertIntoCacheQuery.prepare("INSERT INTO " + cacheTable + " (" + IndexColumnName + ", " + fieldNameList + ") VALUES (" + bindString + ");");

		unsigned long sgxIndex = 0;
		if (!m_tables.empty()) {

			sgxIndex = m_tables.rbegin()->first + 1;
		}

		QSqlQuery getDataQuery(db);
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

		UpdateTimestampForTable(cacheTable, formattedSourceName + ":" + sourceTable, QDateTime::currentDateTime());
		AddTableToMap(cacheTable);
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

	int SourceDataCache::GetLastIndexOfTable(const QString& tableName) {

		QSqlQuery query(m_db);
		query.prepare("SELECT " + IndexColumnName + " FROM " + tableName + " ORDER BY " + IndexColumnName + " DESC LIMIT 1");
		if (query.exec()) {

			query.first();
			return query.value(0).toInt();
		}
		else {

			throw std::exception((QObject::tr("Table indexing failure: ") + m_db.lastError().text()).toStdString().c_str());
		}
	}

	void SourceDataCache::CreateNewTableInCache(const QString& name, const QString& fieldNamesAndTypes) {

		CSVCache::CreateNewTableInCache(name, IndexColumnName + " INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,\n" + fieldNamesAndTypes);
	}

	void SourceDataCache::AddTableToMap(const QString& tableName) {

		int startingIndex = -1;
		if (!m_tables.empty()) {

			startingIndex = m_tables.rbegin()->first;
		}

		m_tables[startingIndex + GetLastIndexOfTable(tableName)] = tableName;
	}

	QSqlQuery SourceDataCache::CreateSelectFieldQueryAscending(const InputField& inputfield) const {

		if (!IsInputfieldInCache(inputfield)) {

			throw std::invalid_argument("Can not create SQL query for input field that isn't in the source data cache");
		}

		QSqlQuery query(m_db);
		query.prepare(QString("SELECT %1 FROM ").arg("\"" + QString::fromStdWString(inputfield.GetField()) + "\"") + QString::fromStdWString(inputfield.GetTable()));
		return query;
	}

	QSqlQuery SourceDataCache::CreateMinMaxQuery(const InputField& inputfield) const {

		if (!IsInputfieldInCache(inputfield)) {

			throw std::invalid_argument("Can not create SQL query for input field that isn't in the source data cache");
		}

		QSqlQuery query(m_db);
		QString queryString = QString("SELECT MIN(%1), MAX(%1) FROM ").arg("\"" + QString::fromStdWString(inputfield.GetField()) + "\"") + QString::fromStdWString(inputfield.GetTable());
		query.prepare(queryString);

		return query;
	}

	bool SourceDataCache::IsInputfieldInCache(const InputField& inputfield) const {

		return (m_db.connectionName() == QString::fromStdWString(boost::uuids::to_wstring(inputfield.GetDatasourceID())));
	}

} //namespace SynGlyphX
