#ifndef DATAENGINESTATEMENT_H
#define DATAENGINESTATEMENT_H

#include <iostream>
#include <jni.h>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include "filedatasource.h"
#include "DataEngine_Exports.h"

namespace DataEngine
{
	class DATAENGINE DataEngineStatement {

	private:
		void constructDataStats(int i);
		QStringList getStatsForField(int i, QString field);
		void setSourceType(SynGlyphX::FileDatasource::FileType type);
		JNIEnv *jniEnv;
		jclass jcls;
		QStringList fields;
		QMap<QString, QStringList> dataStats;
		int current;
		QString sourceType;

	public:
		DataEngineStatement(){};
		void prepare(JNIEnv *jniEnv, jclass jcls);
		void prepare(JNIEnv *jniEnv, jclass jcls, const QString& sourceTypeString);
		std::string getTableName(int i);
		std::string checkMethodID(jmethodID methodId, int i);
		int size();
		int tableCount();
		//JDBC FUNCTIONS
		void getFieldsForTable(int i);
		bool hasNext();
		QString getField();
		QString getType();
		QString getMin();
		QString getMax();
		QString getAverage();
		QString getCount();
		QString getDistinct();
		//END JDBC
		~DataEngineStatement(){};

	};
}
#endif // DATAENGINESTATEMENT_H
