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

namespace SynGlyphX {

	//QString SourceDataManager::s_intermediateDirectory;

	SourceDataManager::SourceDataManager() :
		m_csvCache()
	{
	}


	SourceDataManager::~SourceDataManager()
	{
		ClearDatabaseConnections();
	}

	QString SourceDataManager::GetQtDBType(const SynGlyphX::Datasource& datasource) {

		if (datasource.IsFile()) {

			return "QSQLITE";
		}

		return "";
	}

	void SourceDataManager::AddDatabaseConnection(const Datasource& datasource, const boost::uuids::uuid& datasourceID) {

		QString datasourceName = QString::fromStdWString(datasource.GetDBName());
		QString connectionName = QString::fromStdString(boost::uuids::to_string(datasourceID));

		if (datasource.IsFile()) {

			const FileDatasource& fileDatasource = static_cast<const FileDatasource&>(datasource);

			if (fileDatasource.RequiresConversionToDB()) {

				if (m_csvCache.IsValid()) {

					m_csvCache.UpdateCSVFile(connectionName, datasourceName);
				}
				else {

					throw std::invalid_argument("Could not add CSV file.  CSV cache is not setup.");
				}
				return;
			}
		}

		if (!QSqlDatabase::contains(connectionName)) {

			QSqlDatabase newDataSourceDB = QSqlDatabase::addDatabase(GetQtDBType(datasource), connectionName);
			newDataSourceDB.setDatabaseName(datasourceName);
			m_databaseIDs.insert(datasourceID);
		}
	}

	void SourceDataManager::AddDatabaseConnections(const DatasourceMaps& datasources) {

		for (auto fileDatasource : datasources.GetFileDatasources()) {

			AddDatabaseConnection(fileDatasource.second, fileDatasource.first);
		}
	}

	void SourceDataManager::ClearDatabaseConnection(const boost::uuids::uuid& id) {

		DatabaseIDSet::iterator iT = m_databaseIDs.find(id);
		if (iT != m_databaseIDs.end()) {

			ClearDatabaseConnection(iT);
		}
	}

	void SourceDataManager::ClearDatabaseConnection(const DatabaseIDSet::const_iterator& id) {

		QSqlDatabase::removeDatabase(QString::fromStdString(boost::uuids::to_string(*id)));
		m_databaseIDs.erase(id);
	}

	void SourceDataManager::ClearDatabaseConnections() {

		while (!m_databaseIDs.empty()) {

			ClearDatabaseConnection(m_databaseIDs.begin());
		}
	}

	void SourceDataManager::Clear() {

		ClearDatabaseConnections();
		m_csvCache.Close();
	}

	void SourceDataManager::SetCacheLocation(const QString& location) {

		m_csvCache.Setup(location);
	}
	/*
	void SourceDataManager::SetIntermediateDirectory(const QString& directory) {

		QDir newDirectory(directory);
		if (!newDirectory.exists()) {

			if (!newDirectory.mkpath(directory)) {

				throw std::invalid_argument("Unable to create " + directory.toStdString());
			}
		}

		s_intermediateDirectory = directory;
	}

	const QString& SourceDataManager::GetIntermeidateDirectory()  {

		return s_intermediateDirectory;
	} 

	QString SourceDataManager::GetIntermediateSQLiteDB(const FileDatasource& datasource, const QString& connectionName) {

		QString datasourceName = s_intermediateDirectory + QDir::separator() + "int_" + connectionName + ".db";

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
					QString sqlCreateTable = "CREATE TABLE " + QString::fromStdWString(*datasource.GetTables().begin()) + " (\n" + QString::fromStdWString(headers[0]) + ' ' + QString::fromStdWString(types[0]);
					for (int i = 1; i < headers.size(); ++i) {

						sqlCreateTable += ",\n\"" + QString::fromStdWString(headers[i]) + "\" " + QString::fromStdWString(types[i]);
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

						typeIsText.push_back(types[j] == L"TEXT");
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
	}*/

} //namespace SynGlyphX