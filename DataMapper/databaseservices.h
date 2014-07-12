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

	static void RemoveAutoGeneratedTableNamesFromList(QStringList& list);

private:
	
};

#endif //DATABASESERVICES_H