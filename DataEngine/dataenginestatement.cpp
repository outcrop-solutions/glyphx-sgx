#include "dataenginestatement.h"
//#include <QtCore/QDebug>

namespace DataEngine
{
	void DataEngineStatement::prepare(JNIEnv *env, jclass jc){

		std::cout << "Preparing statement" << std::endl;
		jniEnv = env;
		jcls = jc;
	}

	std::string DataEngineStatement::getTableName(int i){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getTableName", "(I)Ljava/lang/String;");

		return checkMethodID(methodId, i);
	}

	std::string DataEngineStatement::getColumnName(int i){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getColumnName", "(I)Ljava/lang/String;");

		return checkMethodID(methodId, i);
	}

	std::string DataEngineStatement::getType(int i){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getType", "(I)Ljava/lang/String;");

		return checkMethodID(methodId, i);
	}

	std::string DataEngineStatement::getMin(int i){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getMin", "(I)Ljava/lang/String;");

		return checkMethodID(methodId, i);
	}

	std::string DataEngineStatement::getMax(int i){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getMax", "(I)Ljava/lang/String;");

		return checkMethodID(methodId, i);
	}

	std::string DataEngineStatement::getAverage(int i){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getAverage", "(I)Ljava/lang/String;");

		return checkMethodID(methodId, i);
	}

	std::string DataEngineStatement::getCount(int i){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getCount", "(I)Ljava/lang/String;");

		return checkMethodID(methodId, i);
	}

	std::string DataEngineStatement::getDistinct(int i){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getDistinct", "(I)Ljava/lang/String;");

		return checkMethodID(methodId, i);
	}

	std::string DataEngineStatement::checkMethodID(jmethodID methodId, int i){
		
		jstring itr = NULL;
		if (methodId != NULL) {
			itr = (jstring)jniEnv->CallStaticObjectMethod(jcls, methodId, i);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}

		std::string str = jniEnv->GetStringUTFChars(itr, JNI_FALSE);
		return str;
	}


	int DataEngineStatement::size(){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"size", "()I");
		jint itr;
		if (methodId != NULL) {
			std::cout << "Returning size..." << std::endl;
			itr = jniEnv->CallStaticIntMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}

		return itr;
	}

	int DataEngineStatement::tableCount(){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getTableCount", "()I");
		jint itr;
		if (methodId != NULL) {
			//qDebug() << "Returning count...";
			itr = jniEnv->CallStaticIntMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
			//qDebug() << itr;
		}

		return itr;
	}

}