#ifndef DATAENGINESTATEMENT_H
#define DATAENGINESTATEMENT_H

#include <iostream>
#include <jni.h>
#include "DataEngine_Exports.h"

namespace DataEngine
{
	class DATAENGINE DataEngineStatement {

	private:
		JNIEnv *jniEnv;
		jclass jcls;
		int hello;

	public:
		DataEngineStatement(){};
		void prepare(JNIEnv *jniEnv, jclass jcls);
		std::string getTableName(int i);
		std::string getColumnName(int i);
		std::string getType(int i);
		std::string getMin(int i);
		std::string getMax(int i);
		std::string getAverage(int i);
		std::string getCount(int i);
		std::string getDistinct(int i);
		std::string checkMethodID(jmethodID methodId, int i);
		int size();
		int tableCount();
		~DataEngineStatement(){};

	};
}
#endif // DATAENGINESTATEMENT_H
