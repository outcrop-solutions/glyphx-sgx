#ifndef DATABASESERVICES_H
#define DATABASESERVICES_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtSql/QSqlDatabase>

class DatabaseServices
{
public:
	DatabaseServices();
	~DatabaseServices();

	static bool IsSQLiteDB(const QString& filename);

	static QString GetFormattedDBName(const QSqlDatabase& db);

	static const QString& GetProjectDBConnectionName();
	static const QStringList& GetSourceDataDBConnectionNames();
	static const QString& AddNewSourceDataDBConnectionName();

	static void RemoveAutoGeneratedTableNamesFromList(QStringList& list);

private:
	static QStringList s_sourceDataDBConnectionNames;
	static const QString s_projectDatabaseConnectionID;
};

#endif //DATABASESERVICES_H