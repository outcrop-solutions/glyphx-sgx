#pragma warning(disable: 4996)

#include "dataengineconnection.h"
#include "dataenginestatement.h"
#include <fstream>
#include <Windows.h>
//#include <QtCore/QDebug>

namespace DataEngine
{
	typedef long(JNICALL *CreateJVMFunc)(JavaVM **pvm, void **penv, void *args);

	void DataEngineConnection::createJVM(){

		std::ifstream jre(".\\jre\\bin\\client\\jvm.dll");
		HMODULE jvmDll;
		if (jre){
			jvmDll = LoadLibrary(L"jre\\bin\\client\\jvm.dll");
		}
		else{
			jvmDll = LoadLibrary(L"..\\..\\DataEngine\\jdk1.7.0_79\\jre\\bin\\client\\jvm.dll");
		}jre.close();
		CreateJVMFunc CreateJVM = (CreateJVMFunc)GetProcAddress(jvmDll, "JNI_CreateJavaVM");

		JavaVMInitArgs vmArgs;
		JavaVMOption options[1];
		std::ifstream ifile(".\\dataengine.jar");
		if (ifile){
			options[0].optionString =
				"-Djava.class.path=.\\dataengine.jar;.\\sqlite4java.jar;.\\database_drivers\\mysql-connector-java-5.1.38-bin.jar";
		}else{
			options[0].optionString =
				"-Djava.class.path=..\\..\\DataEngine\\dataengine.jar;..\\..\\DataEngine\\sqlite4java.jar;..\\..\\DataEngine\\database_drivers\\mysql-connector-java-5.1.38-bin.jar";
		}ifile.close();
		vmArgs.version = JNI_VERSION_1_2;
		vmArgs.options = options;
		vmArgs.nOptions = 1;
		vmArgs.ignoreUnrecognized = JNI_FALSE;

		// Create the JVM
		long flag = CreateJVM(&javaVM, (void**)&jniEnv, &vmArgs);
		//flag = JNI_CreateJavaVM(&javaVM, (void**)&jniEnv, &vmArgs);

		if (flag == JNI_ERR) {
			//qDebug() << QString("Error creating VM. Exiting...");
		}

		jcls = jniEnv->FindClass("DataEngine");

		if (jcls == NULL) {
			std::cout << "Class not found." << std::endl;
			jniEnv->ExceptionDescribe();
			javaVM->DestroyJavaVM();
		}
		if (jcls != NULL) {
			std::cout << "Class found." << std::endl;
			classFound = true;
		}
	}

	bool DataEngineConnection::hasJVM(){
		return classFound;
	}

	void DataEngineConnection::loadCSV(std::string path){

		if (classFound){
			jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
				"loadFromCSV", "(Ljava/lang/String;)V");
			if (methodId != NULL) {
				std::cout << "Loading csv..." << std::endl;

				jstring str = jniEnv->NewStringUTF(path.c_str());
				jniEnv->CallStaticVoidMethod(jcls, methodId, str);
				if (jniEnv->ExceptionCheck()) {
					jniEnv->ExceptionDescribe();
					jniEnv->ExceptionClear();
				}
			}
		}
	}

	void DataEngineConnection::loadSQLite(std::string path){

		if (classFound){
			jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
				"loadFromSQLite", "(Ljava/lang/String;)V");
			if (methodId != NULL) {
				std::cout << "Loading sqlite..." << std::endl;

				jstring str = jniEnv->NewStringUTF(path.c_str());
				jniEnv->CallStaticVoidMethod(jcls, methodId, str);
				if (jniEnv->ExceptionCheck()) {
					jniEnv->ExceptionDescribe();
					jniEnv->ExceptionClear();
				}
			}
		}
	}

	std::vector<std::string> DataEngineConnection::getTableNames(){

		std::vector<std::string> str;
		if (classFound){
			DataEngine::DataEngineStatement s1;
			s1.prepare(jniEnv, jcls);
			int size = s1.tableCount();
			for (int i = 0; i < size; i++){
				str.push_back(s1.getTableName(i));
			}
		}

		return str;
	}

	JNIEnv* DataEngineConnection::getEnv(){
		return jniEnv;
	}

	jclass DataEngineConnection::getJcls(){
		return jcls;
	}

	void DataEngineConnection::destroyJVM(){

		javaVM->DestroyJavaVM();
	}

	void DataEngineConnection::addTableNumericFields(boost::uuids::uuid id, std::vector<std::wstring> tableNames){
		tableNumericFields[id] = tableNames;
	}

	std::vector<std::wstring> DataEngineConnection::getTableNumericFields(boost::uuids::uuid id){
		return tableNumericFields[id];
	}

	std::map<boost::uuids::uuid, std::vector<std::wstring>> DataEngineConnection::getNumericFieldsTable(){
		return tableNumericFields;
	}

	void DataEngineConnection::addTableName(boost::uuids::uuid id, std::wstring name){
		tableNames[id] = name;
	}

	std::wstring DataEngineConnection::getTableName(boost::uuids::uuid id){
		return tableNames[id];
	}

//JDBC ACCESSOR FUNCTIONS
	QStringList DataEngineConnection::connectToServer(QString db_url, QString user, QString pass, QString db_type){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"connectToServer", "(Ljava/lang/String;java/lang/String;java/lang/String;java/lang/String;)[Ljava/lang/String;");
		jobjectArray itr;
		QStringList databases;
		if (methodId != NULL) {

			itr = (jobjectArray)jniEnv->CallStaticObjectMethod(jcls, methodId, db_url, user, pass, db_type);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}

			int length = jniEnv->GetArrayLength(itr);

			for (int i = 0; i < length; i++){
				jstring element = (jstring)jniEnv->GetObjectArrayElement(itr, i);
				const char *str = jniEnv->GetStringUTFChars(element, 0);
				QString db_name(str);
				databases << db_name;
			}
		}
		return databases;
	}

	QStringList DataEngineConnection::chooseDatabase(QString db_name){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"chooseDatabase", "(Ljava/lang/String;)[Ljava/lang/String;");
		jobjectArray itr;
		QStringList tables;
		if (methodId != NULL) {

			itr = (jobjectArray)jniEnv->CallStaticObjectMethod(jcls, methodId, db_name);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}

			int length = jniEnv->GetArrayLength(itr);

			for (int i = 0; i < length; i++){
				jstring element = (jstring)jniEnv->GetObjectArrayElement(itr, i);
				const char *str = jniEnv->GetStringUTFChars(element, 0);
				QString tbl_name(str);
				tables << tbl_name;
			}
		}
		return tables;
	}

	void DataEngineConnection::setChosenTables(QStringList chosen){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"connectToServer", "([Ljava/lang/String;)V");

		jobjectArray selected = (jobjectArray)jniEnv->NewObjectArray(chosen.size(), jniEnv->FindClass("java/lang/String"), jniEnv->NewStringUTF(""));

		if (methodId != NULL) {

			for (int i = 0; i < chosen.size(); i++){
				jniEnv->SetObjectArrayElement(selected, i, jniEnv->NewStringUTF(chosen.at(i).toStdString().c_str()));
			}

			jniEnv->CallStaticObjectMethod(jcls, methodId, selected);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
	}
//JDBC END

}