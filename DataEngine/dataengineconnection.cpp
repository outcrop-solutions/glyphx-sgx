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
				"-Djava.class.path=.\\dataengine.jar;.\\sqlite4java.jar;";
		}else{
			options[0].optionString =
				"-Djava.class.path=..\\..\\DataEngine\\dataengine.jar;..\\..\\DataEngine\\sqlite4java.jar;";
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

	void DataEngineConnection::setTable(int i){

		if (classFound){
			jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
				"setTable", "(I)V");
			if (methodId != NULL) {

				jniEnv->CallStaticVoidMethod(jcls, methodId, i);
				if (jniEnv->ExceptionCheck()) {
					jniEnv->ExceptionDescribe();
					jniEnv->ExceptionClear();
				}
			}
		}
	}

	std::vector<std::string> DataEngineConnection::getTableNames(){

		//qDebug() << "Retrieving table names...";

		std::vector<std::string> str;
		if (classFound){
			DataEngine::DataEngineStatement s1;
			s1.prepare(jniEnv, jcls);
			int size = s1.tableCount();
			//qDebug() << size;
			for (int i = 0; i < size; i++){
				str.push_back(s1.getTableName(i));
			}
		}

		return str;
	}

	std::vector<std::string> DataEngineConnection::getColumnNames(){

		std::cout << "Retrieving column names..." << std::endl;

		std::vector<std::string> str;
		if (classFound){
			DataEngine::DataEngineStatement s1;
			s1.prepare(jniEnv, jcls);
			int size = s1.size();
			for (int i = 0; i < size; i++){
				str.push_back(s1.getColumnName(i));
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

}