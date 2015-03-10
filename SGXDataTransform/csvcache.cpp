#include "csvcache.h"
#include "uuid.h"
#include <boost/uuid/uuid_io.hpp>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include "csvtfilereaderwriter.h"
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>
#include <QtCore/QVariant>

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

	void CSVCache::Setup(const QString& cacheFilename) {

		if (IsValid()) {

			Close();
		}

		m_connectionID = QString::fromStdWString(boost::uuids::to_wstring(UUIDGenerator::GetNewRandomUUID()));
		m_db = QSqlDatabase::addDatabase("QSQLITE", m_connectionID);
		m_db.setDatabaseName(cacheFilename);

		if (!m_db.open()) {

			Close();
			throw std::exception("CSV cache db failed to open.");
		}

		if (!m_db.tables().contains(s_tableIndexName)) {

			try {

				CreateNewTableInCache(s_tableIndexName, "TableName TEXT NOT NULL PRIMARY KEY UNIQUE,\nFormattedName TEXT NOT NULL,\nTimestamp INTEGER NOT NULL");
			}
			catch (const std::exception& e) {

				throw;
			}
		}
	}

	void CSVCache::Close() {

		if (!m_connectionID.isEmpty()) {

			m_db.close();
			QSqlDatabase::removeDatabase(m_connectionID);
		}
	}

	void CSVCache::UpdateCSVFile(const QString& tableName, const QString& csvFilename) {

		if (!IsValid()) {

			throw std::exception("Could not add CSV file to cache.  CSV cache is not set up.");
		}

		QString csvtFilename = csvFilename + L't';
		QFileInfo csvtFile(csvtFilename);
		if (!csvtFile.exists()) {

			throw std::exception("CSVT file does not exist");
		}

		QFileInfo csvFile(csvFilename);
		if (!csvFile.exists()) {

			throw std::exception("CSV file does not exist");
		}

		QDateTime csvTimestamp;
		if (csvFile.lastModified() > csvtFile.lastModified()) {

			csvTimestamp = csvFile.lastModified();
		}
		else {

			csvTimestamp = csvtFile.lastModified();
		}

		bool csvFileIsInCache = m_db.tables().contains(tableName);
		if (csvFileIsInCache) {

			QDateTime tableLastModified = GetTimestampForTable(tableName);
			if (csvTimestamp < tableLastModified) {

				//CSV Cache is up to date so do nothing
				return;
			}
			else {

				DeleteTable(tableName);
			}
		}

		try {

			CSVTFileReaderWriter csvtFileReader(csvtFile.fileName().toStdString());
			CSVFileReader csvFileReader(csvFile.fileName().toStdString());

			const CSVFileReader::CSVValues& headers = csvFileReader.GetHeaders();
			const CSVFileReader::CSVValues& types = csvtFileReader.GetTypes();

			CreateTableFromCSVHeaders(tableName, headers, types);

			std::vector<bool> typeIsText;
			typeIsText.reserve(types.size());
			for (int j = 0; j < types.size(); ++j) {

				typeIsText.push_back(types[j] == L"TEXT");
			}

			QString headerList = QString::fromStdWString(headers[0]);
			for (int k = 1; k < headers.size(); ++k) {

				headerList += ", " + QString::fromStdWString(headers[k]);
			}

			while (!csvFileReader.IsAtEndOfFile()) {

				CSVFileReader::CSVValues values = csvFileReader.GetValuesFromLine();

				if (values.empty()) {

					continue;
				}

				QSqlQuery insertTableQuery(m_db);
				QString sqlInsert = "INSERT INTO " + tableName + " (" + headerList + ") VALUES (";
				for (int i = 0; i < values.size(); ++i) {

					if (typeIsText[i]) {

						sqlInsert += "\"";
					}

					sqlInsert += QString::fromStdWString(values[i]);

					if (typeIsText[i]) {

						sqlInsert += "\"";
					}

					if (i < values.size() - 1) {

						sqlInsert += ", ";
					}
				}
				sqlInsert += ");";
				insertTableQuery.prepare(sqlInsert);
				bool querySucceeded = insertTableQuery.exec();
				if (!querySucceeded) {

					throw std::exception((QObject::tr("Failed to add file to CSV cache:") + m_db.lastError().text()).toStdString().c_str());
				}
				insertTableQuery.finish();
			}

			UpdateTimestampForTable(tableName, csvFile.fileName(), csvTimestamp);
		}
		catch (const std::exception& e) {

			throw;
		}
	}

	void CSVCache::CreateNewTableInCache(const QString& name, const QString& fieldNamesAndTypes) {

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

		QString fieldNamesAndTypes = QString::fromStdWString(headers[0]) + ' ' + QString::fromStdWString(types[0]);
		for (int i = 1; i < headers.size(); ++i) {

			fieldNamesAndTypes += ",\n\"" + QString::fromStdWString(headers[i]) + "\" " + QString::fromStdWString(types[i]);
		}
		CreateNewTableInCache(name, fieldNamesAndTypes);
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

	QDateTime CSVCache::GetTimestampForTable(const QString& table) {

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

} //namespace SynGlyphX