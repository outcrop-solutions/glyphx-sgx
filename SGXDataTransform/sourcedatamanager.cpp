#include "sourcedatamanager.h"
#include <QtCore/QUuid>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QVariant>
#include <QtCore/QDateTime>
#include <exception>
#include <boost/uuid/uuid_io.hpp>
#include "datatransformmapping.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include "csvfilereader.h"
#include "csvtfilereaderwriter.h"

namespace SynGlyphX {

	SourceDataManager::SourceDataManager()
	{
	}


	SourceDataManager::~SourceDataManager()
	{
		ClearDatabaseConnections();
	}

	bool SourceDataManager::IsSQLiteDB(const QString& filename) {

		const char* first16BytesOfSQLiteDB = "SQLite format 3";

		QFile file(filename);
		if (file.exists()) {

			if (file.open(QIODevice::ReadOnly)) {

				uchar* first16bytes = file.map(0, 16);
				file.close();

				return (memcmp(first16bytes, first16BytesOfSQLiteDB, 16) == 0);
			}
		}

		return false;
	}

	QStringList SourceDataManager::GetListOfTablesWithoutAutogeneratedTables(const QSqlDatabase& db) {

		QStringList tableList = db.tables(QSql::Tables);
		tableList.append(db.tables(QSql::Views));

		if (db.driverName() == "QSQLITE") {

			tableList.removeAll("sqlite_sequence");
		}

		return tableList;
	}

	QStringList SourceDataManager::GetListOfTablesWithoutAutogeneratedTablesInDatabaseFile(const QString& dbName) {

		QStringList tables;
		if (IsSQLiteDB(dbName)) {

			QSqlDatabase newDataSourceDB = QSqlDatabase::addDatabase("QSQLITE", "TEMPDB");
			newDataSourceDB.setDatabaseName(dbName);

			newDataSourceDB.open();
			tables = GetListOfTablesWithoutAutogeneratedTables(newDataSourceDB);
			
			newDataSourceDB.close();
			QSqlDatabase::removeDatabase("TEMPDB");
		}

		return tables;
	}

	QString SourceDataManager::GetQtDBType(const SynGlyphX::Datasource& datasource) {

		if (datasource.IsFile()) {

			return "QSQLITE";
		}

		return "";
	}

	QStringList SourceDataManager::GetSQLiteDataTypesForFormat(const QString& format) {

		QStringList fieldTypes;
		if (format.toUpper() == "CSV") {

			fieldTypes.push_back("Text");
			fieldTypes.push_back("Integer");
			fieldTypes.push_back("Real");
		}
		else {

			throw std::invalid_argument("Format " + format.toStdString() + " was not recognized");
		}

		return fieldTypes;
	}

	void SourceDataManager::AddDatabaseConnection(const Datasource& datasource, const boost::uuids::uuid& datasourceID) {

		QString datasourceName = QString::fromStdWString(datasource.GetDBName());
		QString connectionName = QString::fromStdString(boost::uuids::to_string(datasourceID));

		if (datasource.IsFile()) {

			const FileDatasource& fileDatasource = static_cast<const FileDatasource&>(datasource);

			if (fileDatasource.RequiresConversionToDB()) {
				datasourceName = GetIntermediateSQLiteDB(fileDatasource, connectionName);
			}
		}

		QSqlDatabase newDataSourceDB = QSqlDatabase::addDatabase(GetQtDBType(datasource), connectionName);
		newDataSourceDB.setDatabaseName(datasourceName);
	}

	void SourceDataManager::AddDatabaseConnections(const DatasourceMaps& datasources) {

		for (auto fileDatasource : datasources.GetFileDatasources()) {

			AddDatabaseConnection(fileDatasource.second, fileDatasource.first);
		}
	}

	void SourceDataManager::ClearDatabaseConnection(const boost::uuids::uuid& id) {

		ClearDatabaseConnection(QString::fromStdString(boost::uuids::to_string(id)));
		m_databaseIDs.erase(m_databaseIDs.find(id));
	}

	void SourceDataManager::ClearDatabaseConnection(const QString& id) {

		QSqlDatabase::removeDatabase(id);
	}

	void SourceDataManager::ClearDatabaseConnections() {

		for (boost::uuids::uuid id : m_databaseIDs) {

			ClearDatabaseConnection(QString::fromStdString(boost::uuids::to_string(id)));
		}

		m_databaseIDs.clear();
	}

	QVariantList SourceDataManager::RunSelectSqlQuery(const InputField& inputfield) const {

		QVariantList results;
		QSqlDatabase db = QSqlDatabase::database(QString::fromStdWString(boost::uuids::to_wstring(inputfield.GetDatasourceID())));

		QSqlQuery query(db);
		query.prepare(QString("SELECT %1 FROM ").arg("\"" + QString::fromStdWString(inputfield.GetField()) + "\"") + QString::fromStdWString(inputfield.GetTable()));
		query.exec();
		while (query.next()) {
			results.push_back(query.value(0));
		}

		return results;
	}

	QVariantList SourceDataManager::GetMinMaxSqlQuery(const InputField& inputfield) const {

		QSqlDatabase db = QSqlDatabase::database(QString::fromStdWString(boost::uuids::to_wstring(inputfield.GetDatasourceID())));
		QSqlQuery query(db);
		QString queryString = QString("SELECT MIN(%1), MAX(%1) FROM ").arg("\"" + QString::fromStdWString(inputfield.GetField()) + "\"") + QString::fromStdWString(inputfield.GetTable());
		query.prepare(queryString);

		query.exec();
		query.first();

		QVariantList results;
		results.push_back(query.value(0));
		results.push_back(query.value(1));

		return results;
	}

	void SourceDataManager::SetIntermediateDirectory(QString directory) {

		QDir newDirectory(directory);
		if (!newDirectory.exists()) {

			if (!newDirectory.mkpath(directory)) {

				throw std::invalid_argument("Unable to create " + directory.toStdString());
			}
		}

		m_intermediateDirectory = directory;
	}

	const QString& SourceDataManager::GetIntermeidateDirectory() const {

		return m_intermediateDirectory;
	}

	QString SourceDataManager::GetIntermediateSQLiteDB(const FileDatasource& datasource, const QString& connectionName) {

		QString datasourceName = m_intermediateDirectory + QDir::separator() + "int_" + connectionName + ".db";

		QFileInfo originalDataSourceFile(QString::fromStdWString(datasource.GetDBName()));
		QFileInfo intermediateFile(datasourceName);
		if ((!intermediateFile.exists()) || (originalDataSourceFile.lastModified() > intermediateFile.lastModified())) {

			if (intermediateFile.exists()) {

				QFile::remove(datasourceName);
			}

			try {

				QSqlDatabase intermediateDB = QSqlDatabase::addDatabase("QSQLITE", "Intermediate");
				intermediateDB.setDatabaseName(datasourceName);
				intermediateDB.open();

				if (datasource.GetType() == FileDatasource::CSV) {

					std::wstring csvtFilename = datasource.GetDBName() + L't';
					QFile csvtFile(QString::fromStdWString(csvtFilename));
					if (!csvtFile.exists()) {

						throw std::exception("CSVT file does not exist");
					}

					QFile csvFile(QString::fromStdWString(datasource.GetDBName()));
					if (!csvFile.exists()) {

						throw std::exception("CSV file does not exist");
					}

					CSVTFileReaderWriter csvtFileReader(csvtFile.fileName().toStdString());
					CSVFileReader csvFileReader(csvFile.fileName().toStdString());

					const CSVFileReader::CSVValues& headers = csvFileReader.GetHeaders();
					const CSVFileReader::CSVValues& types = csvtFileReader.GetTypes();

					QSqlQuery createTableQuery(intermediateDB);
					QString sqlCreateTable = "CREATE TABLE " + QString::fromStdWString(*datasource.GetTables().begin()) + " (\n" + QString::fromStdString(headers[0]) + ' ' + QString::fromStdString(types[0]);
					for (int i = 1; i < headers.size(); ++i) {

						sqlCreateTable += ",\n\"" + QString::fromStdString(headers[i]) + "\" " + QString::fromStdString(types[i]);
					}
					sqlCreateTable += "\n);";

					createTableQuery.prepare(sqlCreateTable);
					bool querySucceeded = createTableQuery.exec();
					
					if (!querySucceeded) {

						throw std::exception(intermediateDB.lastError().text().toStdString().c_str());
					}

					createTableQuery.finish();

					std::vector<bool> typeIsText;
					typeIsText.reserve(types.size());
					for (int j = 0; j < types.size(); ++j) {

						typeIsText.push_back(types[j] == "TEXT");
					}
					
					while (!csvFileReader.IsAtEndOfFile()) {

						CSVFileReader::CSVValues values = csvFileReader.GetValuesFromLine();

						if (values.empty()) {

							continue;
						}

						QSqlQuery insertTableQuery(intermediateDB);
						QString sqlInsert = "INSERT INTO " + QString::fromStdWString(*(datasource.GetTables().begin())) + "\nVALUES (";
						for (int i = 0; i < values.size(); ++i) {

							if (typeIsText[i]) {

								sqlInsert += "\"";
							}

							sqlInsert += QString::fromStdString(values[i]);

							if (typeIsText[i]) {

								sqlInsert += "\"";
							}

							if (i < values.size() - 1) {

								sqlInsert += ", ";
							}
						}
						sqlInsert += ");";
						insertTableQuery.prepare(sqlInsert);
						querySucceeded = insertTableQuery.exec();
						if (!querySucceeded) {

							throw std::exception(intermediateDB.lastError().text().toStdString().c_str());
						}
						insertTableQuery.finish();
					}
				}

				intermediateDB.commit();

				QSqlDatabase::removeDatabase("Intermediate");
			}
			catch (const std::exception& e) {

				QSqlDatabase::removeDatabase("Intermediate");
				throw;
			}
		}

		return datasourceName;
	}

} //namespace SynGlyphX