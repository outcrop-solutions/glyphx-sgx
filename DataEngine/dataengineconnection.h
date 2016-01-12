#ifndef DATAENGINECONNECTION_H
#define DATAENGINECONNECTION_H

#include <jni.h>
#include <vector>
#include <boost/uuid/uuid.hpp>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <map>
#include "DataEngine_Exports.h"

namespace DataEngine
{
	class DATAENGINE DataEngineConnection {

	private:
		JavaVM *javaVM;
		JNIEnv *jniEnv;
		jclass jcls;
		bool classFound = false;
		std::map<boost::uuids::uuid, std::vector<std::wstring>> tableNumericFields;
		std::map<boost::uuids::uuid, std::wstring> tableNames;

	public:
		DataEngineConnection(){};
		void createJVM();
		bool hasJVM();
		void loadCSV(std::string path);
		void loadSQLite(std::string path);
		void setTable(int i);
		void addTableNumericFields(boost::uuids::uuid id, std::vector<std::wstring> tableNames);
		void addTableName(boost::uuids::uuid id, std::wstring name);
		std::wstring getTableName(boost::uuids::uuid id);
		std::vector<std::wstring> getTableNumericFields(boost::uuids::uuid id);
		std::map<boost::uuids::uuid, std::vector<std::wstring>> getNumericFieldsTable();
		std::vector<std::string> getTableNames();
		std::vector<std::string> getColumnNames();
		//JDBC ACCESSOR FUNCTIONS
		QStringList connectToServer(QString db_url, QString user, QString pass, QString db_type);
		QStringList chooseDatabase(QString db_name);
		void setChosenTables(QStringList chosen);
		//JDBC END
		void destroyJVM();
		JNIEnv* getEnv();
		jclass getJcls();
		~DataEngineConnection(){};

	};
}
#endif // DATAENGINECONNECTION_H