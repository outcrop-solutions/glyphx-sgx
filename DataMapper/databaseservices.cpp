#include "databaseservices.h"
#include <QtCore/QUuid>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <exception>

const QString DatabaseServices::s_projectDatabaseConnectionID("ProjectDatabase");
QStringList DatabaseServices::s_sourceDataDBConnectionNames;

DatabaseServices::DatabaseServices()
{
}


DatabaseServices::~DatabaseServices()
{
}

bool DatabaseServices::IsSQLiteDB(const QString& filename) {

	const char* first16BytesOfSQLiteDB = "SQLITE3 format 3";

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

QString DatabaseServices::GetFormattedDBName(const QSqlDatabase& db) {

	QString databaseName = db.databaseName();

	if (db.driverName() == "QSQLITE") {
		QFileInfo fi(databaseName);
		return fi.baseName();
	}
	else {
		return databaseName;
	}
}

const QString& DatabaseServices::GetProjectDBConnectionName() {

	return s_projectDatabaseConnectionID;
}

const QStringList& DatabaseServices::GetSourceDataDBConnectionNames() {

	return s_sourceDataDBConnectionNames;
}

const QString& DatabaseServices::AddNewSourceDataDBConnectionName() {

	s_sourceDataDBConnectionNames.push_back(QUuid::createUuid().toString());
	return s_sourceDataDBConnectionNames.last();
}