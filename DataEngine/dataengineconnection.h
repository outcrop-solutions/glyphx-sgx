#pragma once
#ifndef DATAENGINECONNECTION_H
#define DATAENGINECONNECTION_H

#include <jni.h>
#include <vector>
#include <boost/uuid/uuid.hpp>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <map>
#include <memory>
#include "DataEngine_Exports.h"
#include "runtimeloadedlibrary.h"

namespace DataEngine
{
	class DATAENGINE DataEngineConnection {

	public:
		struct ForeignKey {
			QString key;
			QString origin;
			QString value;
			ForeignKey(QString k, QString o, QString v) : key(k), origin(o), value(v) {}
		};

		typedef std::vector<DataEngineConnection::ForeignKey> ForiegnKeyVector;
		typedef std::shared_ptr<DataEngineConnection> SharedPtr;

		DataEngineConnection();
		~DataEngineConnection();

		void createJVM();
		bool hasJVM() const;
		QString VersionNumber();
		void loadCSV(std::string path);
		void loadSQLite(std::string path);
		QString encryptPassword(QString password);
		QString decryptPassword(QString encrypted);
		//void setTable(int i);
		//void addTableNumericFields(boost::uuids::uuid id, std::vector<std::wstring> tableNames);
		//void addTableName(boost::uuids::uuid id, std::wstring name);
		//const std::wstring& getTableName(boost::uuids::uuid id) const;
		//const std::vector<std::wstring>& getTableNumericFields(boost::uuids::uuid id) const;
		//const std::map<boost::uuids::uuid, std::vector<std::wstring>>& getNumericFieldsTable() const;
		//std::vector<std::string> getTableNames();
		//std::vector<std::string> getColumnNames();
		//JDBC ACCESSOR FUNCTIONS
		QStringList connectToServer(QString db_url, QString user, QString pass, QString db_type);
		QStringList getColumnNames(QString tablename);
		const std::vector<DataEngineConnection::ForeignKey>& getForeignKeys(QString tablename);
		const std::vector<QStringList>& getSampleData(QString tablename);
		void setChosenTables(QStringList chosen);
		void setQueryTables(QString query);
		void queueATable(QString name, QString query);
		void removeQueuedTable(QString name);
		void executeQueuedTables();
		QStringList getSchemaTableNames(QString schema);
		QStringList getTables();
		int sizeOfQuery(QString query);
		QString JavaErrors();
		void closeConnection();
		//JDBC END
		void destroyJVM();
		JNIEnv* getEnv() const;
		jclass getJcls() const;
		//void testFunction();
		bool IsConnectionOpen() const;
		bool IsConnectionOpen(const QString& connection) const;

		static QString CreateInnerJoinQueryFromForiegnKeys(const QString& mainTable, const ForiegnKeyVector& foriegnKeyTables);

	private:
		QString m_openConnection;
		JavaVM *javaVM;
		JNIEnv *jniEnv;
		jclass jcls;
		bool classFound = false;
		QStringList tables;
		std::map<boost::uuids::uuid, std::vector<std::wstring>> tableNumericFields;
		std::map<boost::uuids::uuid, std::wstring> tableNames;
		std::map<QString, QStringList> columnNames;
		std::map<QString, std::vector<ForeignKey>> foreignKeysByTable;
		std::map<QString, std::vector<QStringList>> sampleDataByTable;

		SynGlyphX::RuntimeLoadedLibrary jvmDll;

		QStringList getForeignKeyString(QString tablename);
		QStringList getRowOfSampleData(int index, int row);
		QStringList getSchemas();
		void setTables();
	};
}
#endif // DATAENGINECONNECTION_H