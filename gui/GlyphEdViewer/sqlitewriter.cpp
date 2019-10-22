#include "sqlitewriter.h"
#include <QtWidgets/QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtXml/QXmlSimpleReader>
#include <QtWidgets/QMessageBox>

SqliteWriter::SqliteWriter()
{
	
}

SqliteWriter::~SqliteWriter()
{

}

QString SqliteWriter::WriteDatabase(QString location, QJsonArray data, QString sdt, QString table_name) {

	sdt_name = sdt;
	QString tableName = table_name;
	QString queryStr = GetCreateTableStatement(location, data, tableName);

	const QString DRIVER("QSQLITE");

	if (QSqlDatabase::isDriverAvailable(DRIVER)){

		QSqlDatabase db = QSqlDatabase::addDatabase(DRIVER);
		db.setDatabaseName(location + "data.db");   
		if (!db.open())
			qWarning() << "ERROR: " << db.lastError();
		QSqlQuery drop("DROP TABLE IF EXISTS " + tableName);
		QSqlQuery create(queryStr);
		if (!create.isActive())
			qWarning() << "ERROR: " << create.lastError().text();

		QSqlQuery query;
		QString insertQuery = "INSERT INTO " + tableName + " VALUES";
		QStringList rows;

		foreach(const QJsonValue & value, data) {
			QJsonObject obj = value.toObject();

			QString row = "(";
			foreach(const QString& key, obj.keys()) {
				QJsonValue value = obj.value(key);
				if (value.isDouble()){
					row += QString::number(value.toDouble()) + ",";
				}
				else {
					row += "\"" + value.toString() + "\",";
				}
			}
			rows.append(row.left(row.length() - 1) + ")");
		}

		foreach(const QString & r, rows){
			insertQuery += r + ",";
		}

		if (!query.exec(insertQuery.left(insertQuery.length() - 1)))
			qWarning() << "ERROR: " << query.lastError().text();
	}

	return queryStr;
}

QString SqliteWriter::GetCreateTableStatement(QString location, QJsonArray data, QString tableName) {

	QString query = "CREATE TABLE " + tableName + "(";

	QMap<QString, QMap<QString, QString>> fieldTypes = ReadInSdt(location);
	QMap<QString, QString> fields = fieldTypes[tableName];

	QStringList qsl;
	foreach(const QJsonValue & value, data) {
		QJsonObject obj = value.toObject();
		qsl = obj.keys();
		break;
	}

	foreach(const QString & field, qsl) {
		if (fields.contains(field)){
			query += field + " " + fields[field].toUpper() + ",";
		}
		else
			query += field + " TEXT ,";
	}

	return query.left(query.length()-1) + ")";
}

QMap<QString, QMap<QString, QString>> SqliteWriter::ReadInSdt(QString location) {

	QFile sdt(location + sdt_name);
	if (!sdt.open(QFile::ReadOnly | QFile::Text)){
		
	}

	QXmlStreamReader reader(&sdt);

	QMap<QString, QMap<QString, QString>> fieldTypes;

	if (reader.readNextStartElement()) {
		if (reader.name() == "Transform"){
			while (reader.readNextStartElement()){
				if (reader.name() == "Datasources"){
					while (reader.readNextStartElement()){
						if (reader.name() == "Datasource"){
							id = reader.attributes().value("id").toString();
							while (reader.readNextStartElement()){
								if (reader.name() == "Tables"){
									while (reader.readNextStartElement()){
										if (reader.name() == "Table"){
											QString s = reader.attributes().value("name").toString();
											tableNames.append(s);
											reader.skipCurrentElement();
										}
										else
											reader.skipCurrentElement();
									}
								}
								else
									reader.skipCurrentElement();
							}
						}
						else
							reader.skipCurrentElement();
					}
				}
				else if (reader.name() == "InputFields"){
					while (reader.readNextStartElement()){
						if (reader.name() == "InputField"){
							QString table = reader.attributes().value("table").toString();
							QString field = reader.attributes().value("field").toString();
							QString type = reader.attributes().value("type").toString();
							fieldTypes[table][field] = type;
							reader.skipCurrentElement();
						}
						else
							reader.skipCurrentElement();
					}
				}
				else
					reader.skipCurrentElement();
			}
		}
		else
			reader.raiseError(QObject::tr("Incorrect file"));
	}

	return fieldTypes;
}

