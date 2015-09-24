#include "csvcache.h"
#include <boost/uuid/uuid_io.hpp>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include "csvtfilereaderwriter.h"
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>
#include <QtCore/QVariant>
#include <QtCore/QDir>
#include <QtSql/QSqlDriver>

namespace SynGlyphX {

	const QString CSVCache::s_tableIndexName = "TableIndex";

	CSVCache::CSVCache() {

	}

	CSVCache::CSVCache(const QString& cacheFilename) {

		Setup(cacheFilename);
	}

	CSVCache::~CSVCache() {

		Close();
	}

	bool CSVCache::IsValid() const {

		return m_db.isOpen();
	}

	const boost::uuids::uuid& CSVCache::GetConnectionID() const {

		return m_connectionID;
	}

	void CSVCache::Setup(const QString& cacheFilename) {

		if (IsValid()) {

			Close();
		}

		m_connectionID = UUIDGenerator::GetNewRandomUUID();
		m_db = QSqlDatabase::addDatabase("QSQLITE", QString::fromStdWString(boost::uuids::to_wstring(m_connectionID)));
		m_db.setDatabaseName(cacheFilename);

		if (!m_db.open()) {

			Close();
			throw std::exception("CSV cache db failed to open.");
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
			catch (const std::exception& e) {

				throw;
			}
		}
	}

	void CSVCache::SetPragmaValue(const QString& pragmaString) {

		QSqlQuery pragmaQuery(m_db);
		pragmaQuery.prepare("PRAGMA " + pragmaString);
		if (!pragmaQuery.exec()) {

			throw std::exception(("Pragma Error: " + m_db.lastError().text()).toStdString().c_str());
		}
		pragmaQuery.finish();
	}

	void CSVCache::Close() {

		if (!m_connectionID.is_nil()) {

			m_db.close();
			QSqlDatabase::removeDatabase(QString::fromStdWString(boost::uuids::to_wstring(m_connectionID)));
			m_connectionID = boost::uuids::uuid();
		}
	}

	void CSVCache::UpdateCSVFile(const QString& tableName, const QString& csvFilename, const QString& formattedName) {

		if (!IsValid()) {

			throw std::exception("Could not add CSV file to cache.  CSV cache is not set up.");
		}

		QString csvtFilename = GetCSVTFilename(csvFilename);
		if (!QFileInfo::exists(csvtFilename)) {

			throw std::exception("CSVT file does not exist");
		}

		if (!QFileInfo::exists(csvFilename)) {

			throw std::exception("CSV file does not exist");
		}

		if (DoesCSVFileNeedUpdate(tableName, csvFilename, csvtFilename)) {

			if (IsTableInCache(tableName)) {

				DeleteTable(tableName);
			}

			try {

				CSVTFileReaderWriter csvtFileReader(QDir::toNativeSeparators(QDir::cleanPath(csvtFilename)).toStdString());
				CSVFileReader csvFileReader(QDir::toNativeSeparators(QDir::cleanPath(csvFilename)).toStdString());

				const CSVFileReader::CSVValues& headers = csvFileReader.GetHeaders();
				const CSVFileReader::CSVValues& types = csvtFileReader.GetTypes();

				CreateTableFromCSVHeaders(tableName, headers, types);

				std::vector<bool> typeIsText;
				typeIsText.reserve(types.size());
				for (int j = 0; j < types.size(); ++j) {

					typeIsText.push_back(types[j] == L"TEXT");
				}

				QString headerList = "\"" + QString::fromStdWString(headers[0]) + "\"";
				for (int k = 1; k < headers.size(); ++k) {

					headerList += ", \"" + QString::fromStdWString(headers[k]) + "\"";
				}

				std::vector<QVariantList> dataToInsert;
				dataToInsert.resize(headers.size());

				while (!csvFileReader.IsAtEndOfFile()) {

					CSVFileReader::CSVValues values = csvFileReader.GetValuesFromLine();

					if (values.empty()) {

						continue;
					}

					for (int i = 0; i < values.size(); ++i) {

						dataToInsert[i].push_back(QString::fromStdWString(values[i]));
					}
				}

				BatchInsertIntoTable(tableName, headerList, dataToInsert);

				UpdateTimestampForTable(tableName, formattedName, GetTimestamp(csvFilename, csvtFilename));
			}
			catch (const std::exception& e) {

				throw;
			}
		}
	}

	QSqlRecord CSVCache::GetColumnNames(const QString& tableName) const {

		if (!IsValid()) {

			throw std::exception("Could not get column names.  CSV cache is not set up.");
		}

		QSqlRecord columnRecord = m_db.record(tableName);
		if (columnRecord.isEmpty()) {

			throw std::invalid_argument("Could not get column names since table is not in CSV cache");
		}

		return columnRecord;
	}
	
	void CSVCache::CreateNewCacheTable(const QString& name, const QString& fieldNamesAndTypes) {

		CreateNewTable(name, fieldNamesAndTypes);
	}

	void CSVCache::CreateNewTable(const QString& name, const QString& fieldNamesAndTypes) {

		QString createTableQueryString = "CREATE TABLE \"" + name + "\" (\n" + fieldNamesAndTypes + "\n);";

		QSqlQuery createTableQuery(m_db);
		createTableQuery.prepare(createTableQueryString);
		bool querySucceeded = createTableQuery.exec();

		if (!querySucceeded) {

			throw std::exception((QObject::tr("Create table in CSV cache failed: ") + m_db.lastError().text()).toStdString().c_str());
		}

		createTableQuery.finish();
	}

	void CSVCache::CreateTableFromCSVHeaders(const QString& name, const CSVFileReader::CSVValues& headers, const CSVFileReader::CSVValues& types) {

		QString fieldNamesAndTypes = "\"" + QString::fromStdWString(headers[0]) + "\" " + QString::fromStdWString(types[0]);
		for (int i = 1; i < headers.size(); ++i) {

			fieldNamesAndTypes += ",\n\"" + QString::fromStdWString(headers[i]) + "\" " + QString::fromStdWString(types[i]);
		}
		CreateNewCacheTable(name, fieldNamesAndTypes);
	}

	void CSVCache::DeleteTable(const QString& table) {

		QSqlQuery dropTableQuery(m_db);
		dropTableQuery.prepare("DROP TABLE \"" + table + "\"");
		bool querySucceeded = dropTableQuery.exec();

		if (!querySucceeded) {

			throw std::exception((QObject::tr("Delete table in CSV cache failed: ") + m_db.lastError().text()).toStdString().c_str());
		}

		dropTableQuery.finish();
	}

	QDateTime CSVCache::GetTimestampForTable(const QString& table) const {

		QSqlQuery timestampQuery(m_db);
		timestampQuery.prepare("SELECT \"Timestamp\" FROM \"" + s_tableIndexName + "\" WHERE TableName=\"" + table + "\";");
		bool querySucceeded = timestampQuery.exec();

		if (!querySucceeded) {

			throw std::exception((QObject::tr("Get timestamp in CSV cache failed: ") + m_db.lastError().text()).toStdString().c_str());
		}

		timestampQuery.first();
		qint64 timestamp = timestampQuery.value(0).toULongLong();
		timestampQuery.finish();
		return QDateTime::fromMSecsSinceEpoch(timestamp);
	}

	void CSVCache::UpdateTimestampForTable(const QString& table, const QString& formattedName, const QDateTime& timestamp) {

		QSqlQuery timestampQuery(m_db);
		QString timeStampQueryString = "INSERT OR REPLACE INTO \"" + s_tableIndexName + "\" (\"TableName\", \"FormattedName\", \"Timestamp\") VALUES(\"" +
			table + "\", \"" + formattedName + "\", " + QString::number(timestamp.toMSecsSinceEpoch()) + ");";
		timestampQuery.prepare(timeStampQueryString);
		bool querySucceeded = timestampQuery.exec();

		if (!querySucceeded) {

			throw std::exception((QObject::tr("Update timestamp in CSV cache failed: ") + m_db.lastError().text()).toStdString().c_str());
		}

		timestampQuery.finish();
	}

	bool CSVCache::DoesCSVFileNeedUpdate(const QString& tableName, const QString& csvFilename, const QString& csvtFilename) const {

		QDateTime csvTimestamp = GetTimestamp(csvFilename, csvtFilename);

		if (!IsTableInCache(tableName)) {

			return true;
		}

		QDateTime tableLastModified = GetTimestampForTable(tableName);
		if (csvTimestamp > tableLastModified) {

			return true;
		}
		
		return false;
	}

	QDateTime CSVCache::GetTimestamp(const QString& csvFilename, const QString& csvtFilename) const {

		QFileInfo csvtFile(csvtFilename);
		QFileInfo csvFile(csvFilename);
		QDateTime csvTimestamp;
		if (csvFile.lastModified() > csvtFile.lastModified()) {

			csvTimestamp = csvFile.lastModified();
		}
		else {

			csvTimestamp = csvtFile.lastModified();
		}

		return csvTimestamp;
	}

	QString CSVCache::GetCSVTFilename(const QString& csvFilename) const {

		return csvFilename + L't';
	}

	bool CSVCache::IsTableInCache(const QString& tableName) const {

		return m_db.tables().contains(tableName);
	}

	void CSVCache::BatchInsertIntoTable(const QString& table, const QString& fieldNames, const std::vector<QVariantList>& data) {

		QStringList valuesBindList;
		for (int i = 0; i < data.size(); ++i) {

			valuesBindList << "?";
		}

		QSqlQuery insertIntoCacheQuery(m_db);
		insertIntoCacheQuery.prepare("INSERT INTO \"" + table + "\" (" + fieldNames + ") VALUES (" + valuesBindList.join(", ") + ");");
		for (int k = 0; k < data.size(); ++k) {

			insertIntoCacheQuery.addBindValue(data[k]);
		}

		m_db.transaction();

		if (!insertIntoCacheQuery.execBatch()) {

			throw std::exception((QObject::tr("Failed to insert data into table: ") + m_db.lastError().text()).toStdString().c_str());
		}
		insertIntoCacheQuery.finish();

		m_db.commit();
	}
} //namespace SynGlyphX