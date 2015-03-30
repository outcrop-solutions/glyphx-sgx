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

	const boost::uuids::uuid& SourceDataManager::GetCSVCacheConnectionID() const {

		return m_csvCache.GetConnectionID();
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

} //namespace SynGlyphX