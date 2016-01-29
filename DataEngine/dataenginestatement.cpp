#include "dataenginestatement.h"
//#include <QtCore/QDebug>

namespace DataEngine
{
	void DataEngineStatement::prepare(JNIEnv *env, jclass jc){
		jniEnv = env;
		jcls = jc;
	}

	void DataEngineStatement::prepare(JNIEnv *env, jclass jc, SynGlyphX::FileDatasource::SourceType type){

		std::cout << "Preparing statement" << std::endl;
		jniEnv = env;
		jcls = jc;
		setSourceType(type);
	}

	std::string DataEngineStatement::getTableName(int i){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getTableName", "(I)Ljava/lang/String;");

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

	void DataEngineStatement::getFieldsForTable(int i){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getFieldsForTable", "(ILjava/lang/String;)[Ljava/lang/String;");
		jobjectArray itr;
		if (methodId != NULL) {
			jstring jstr = jniEnv->NewStringUTF(sourceType.toStdString().c_str());
			itr = (jobjectArray)jniEnv->CallStaticObjectMethod(jcls, methodId, i, jstr);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}

			int length = jniEnv->GetArrayLength(itr);

			for (int j = 0; j < length; j++){
				jstring element = (jstring)jniEnv->GetObjectArrayElement(itr, j);
				const char *str = jniEnv->GetStringUTFChars(element, 0);
				QString field_name(str);
				fields << field_name;
			}
		}
		constructDataStats(i);
	}

	void DataEngineStatement::constructDataStats(int i){

		for (int j = 0; j < fields.size(); j++){
			QStringList fieldStats = getStatsForField(i, fields.at(j));
			dataStats.insert(fields.at(j), fieldStats);
		}
		current = -1;
	}

	QStringList DataEngineStatement::getStatsForField(int i, QString field){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getStatsForField", "(ILjava/lang/String;)[Ljava/lang/String;");
		jobjectArray itr;
		QStringList stats;
		if (methodId != NULL) {

			jstring jstr = jniEnv->NewStringUTF(field.toStdString().c_str());
			itr = (jobjectArray)jniEnv->CallStaticObjectMethod(jcls, methodId, i, jstr);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}

			int length = jniEnv->GetArrayLength(itr);

			for (int j = 0; j < length; j++){
				jstring element = (jstring)jniEnv->GetObjectArrayElement(itr, j);
				const char *str = jniEnv->GetStringUTFChars(element, 0);
				QString stat(str);
				stats << stat;
			}
		}
		return stats;
	}

	bool DataEngineStatement::hasNext(){

		current++;
		if (current < fields.size()){
			return true;
		}
		return false;
	}

	QString DataEngineStatement::getField(){
		return fields.at(current);
	}

	QString DataEngineStatement::getType(){
		return dataStats.value(fields.at(current)).at(0);
	}

	QString DataEngineStatement::getMin(){
		return dataStats.value(fields.at(current)).at(1);
	}

	QString DataEngineStatement::getMax(){
		return dataStats.value(fields.at(current)).at(2);
	}

	QString DataEngineStatement::getAverage(){
		return dataStats.value(fields.at(current)).at(3);
	}

	QString DataEngineStatement::getCount(){
		return dataStats.value(fields.at(current)).at(4);
	}

	QString DataEngineStatement::getDistinct(){
		return dataStats.value(fields.at(current)).at(5);
	}

	void DataEngineStatement::setSourceType(SynGlyphX::FileDatasource::SourceType type){

		if (type == SynGlyphX::FileDatasource::CSV){
			sourceType = "csv";
		}
		else if (type == SynGlyphX::FileDatasource::SQLITE3){
			sourceType = "sqlite3";
		}
		else{
			sourceType = "mysql";
		}
	}

}