///
/// SynGlyphX Holdings Incorporated ("COMPANY") CONFIDENTIAL
/// Copyright (c) 2013-2015 SynGlyphX Holdings Incorporated, All Rights Reserved.
///
/// NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
/// herein are proprietary to COMPANY and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
/// Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
/// from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed 
/// Confidentiality and Non-disclosure agreements explicitly covering such access.
///
/// The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes  
/// information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
/// OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE 
/// LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  
/// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
///


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
#include "useraccesscontrols.h"

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
		void SetGlyphEdPath(QString path);
		QString GetGlyphEdPath();
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
		void ClearJavaErrors();
		void closeConnection();
		//JDBC END
		void destroyJVM();
		JNIEnv* getEnv() const;
		jclass getJcls() const;
		//void testFunction();
		bool IsConnectionOpen() const;
		bool IsConnectionOpen(const QString& connection) const;
		DataEngine::UserAccessControls* UserAccessControls();

		static QString CreateInnerJoinQueryFromForiegnKeys(const QString& mainTable, const ForiegnKeyVector& foriegnKeyTables);

		bool UnZipFile(QString file);

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
		DataEngine::UserAccessControls* uac;

		SynGlyphX::RuntimeLoadedLibrary jvmDll;

		QStringList getForeignKeyString(QString tablename);
		QStringList getRowOfSampleData(int index, int row);
		QStringList getSchemas();
		void setTables();

		QString glyphEdPath;
	};
}
#endif // DATAENGINECONNECTION_H