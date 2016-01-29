#pragma warning(disable: 4996)

#include "dataengineconnection.h"
#include "dataenginestatement.h"
#include <fstream>
#include <Windows.h>
//#include <QtCore/QDebug>

namespace DataEngine
{
	typedef long(JNICALL *CreateJVMFunc)(JavaVM **pvm, void **penv, void *args);

	DataEngineConnection::DataEngineConnection() :
		classFound(false) {


	}

	DataEngineConnection::~DataEngineConnection() {


	}

	void DataEngineConnection::createJVM(){

		std::ifstream jre(".\\jre\\bin\\client\\jvm.dll");
		HMODULE jvmDll;
		if (jre){
			jvmDll = LoadLibrary(L".\\jre\\bin\\client\\jvm.dll");
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
				"-Djava.class.path=.\\dataengine.jar;"
				".\\ojdbc6.jar;"
				".\\database-drivers\\sqlite4java.jar;"
				".\\database-drivers\\mysql-connector-java-5.1.38-bin.jar;"
				".\\database-drivers\\sqlite-jdbc-3.8.11.2.jar;";
		}else{
			options[0].optionString =
				"-Djava.class.path=..\\..\\DataEngine\\Java DataEngine\\dataengine.jar;"
				"..\\..\\DataEngine\\Java DataEngine\\ojdbc6.jar;"
				"..\\..\\DataEngine\\Java DataEngine\\database-drivers\\sqlite4java.jar;"
				"..\\..\\DataEngine\\Java DataEngine\\database-drivers\\mysql-connector-java-5.1.38-bin.jar;"
				"..\\..\\DataEngine\\Java DataEngine\\database-drivers\\sqlite-jdbc-3.8.11.2.jar;";
		}ifile.close();
		vmArgs.version = JNI_VERSION_1_2;
		vmArgs.options = options;
		vmArgs.nOptions = 1;
		vmArgs.ignoreUnrecognized = JNI_FALSE;

		// Create the JVM
		long flag = CreateJVM(&javaVM, (void**)&jniEnv, &vmArgs);
		//flag = JNI_CreateJavaVM(&javaVM, (void**)&jniEnv, &vmArgs);
		
		if (flag == JNI_ERR) {
			
			throw std::exception("JVM Error: Unknown Error");
		}
		else if (flag == JNI_EDETACHED) {

			throw std::exception("JVM Error: Thread detached from VM");
		}
		else if (flag == JNI_EVERSION) {

			throw std::exception("JVM Error: Version Error");
		}
		else if (flag == JNI_ENOMEM) {

			throw std::exception("JVM Error: Not Enough Memory");
		}
		else if (flag == JNI_EEXIST) {

			throw std::exception("JVM Error: JVM already created");
		}
		else if (flag == JNI_EINVAL) {

			throw std::exception("JVM Error: Invalid Arguments");
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
			"connectToServer", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)[Ljava/lang/String;");
		jobjectArray itr;
		QStringList databases;
		if (methodId != NULL) {
			jstring db = jniEnv->NewStringUTF(db_url.toStdString().c_str());
			jstring usr = jniEnv->NewStringUTF(user.toStdString().c_str());
			jstring pwd = jniEnv->NewStringUTF(pass.toStdString().c_str());
			jstring type = jniEnv->NewStringUTF(db_type.toStdString().c_str());
			itr = (jobjectArray)jniEnv->CallStaticObjectMethod(jcls, methodId, db, usr, pwd, type);
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

		foreignKeysByTable.clear();
		sampleDataByTable.clear();
		columnNames.clear();
		tables.clear();
		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"chooseDatabase", "(Ljava/lang/String;)[Ljava/lang/String;");
		jobjectArray itr;
		if (methodId != NULL) {
			jstring name = jniEnv->NewStringUTF(db_name.toStdString().c_str());
			itr = (jobjectArray)jniEnv->CallStaticObjectMethod(jcls, methodId, name);
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
			"setChosenTables", "([Ljava/lang/String;)V");

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

	QStringList DataEngineConnection::getColumnNames(QString tablename){

		if (columnNames.find(tablename) == columnNames.end()) {
			jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
				"getFieldsForTable", "(I)[Ljava/lang/String;");
			jobjectArray itr;
			QStringList temp;
			if (methodId != NULL) {
				itr = (jobjectArray)jniEnv->CallStaticObjectMethod(jcls, methodId, tables.indexOf(tablename));
				if (jniEnv->ExceptionCheck()) {
					jniEnv->ExceptionDescribe();
					jniEnv->ExceptionClear();
				}

				int length = jniEnv->GetArrayLength(itr);

				for (int i = 0; i < length; i++){
					jstring element = (jstring)jniEnv->GetObjectArrayElement(itr, i);
					const char *str = jniEnv->GetStringUTFChars(element, 0);
					QString colname(str);
					temp << colname;
				}
				columnNames[tablename] = temp;
			}
		}
		return columnNames[tablename];
	}

	std::vector<DataEngineConnection::ForeignKey> DataEngineConnection::getForeignKeys(QString tablename){

		if (foreignKeysByTable.find(tablename) == foreignKeysByTable.end()) {
			std::vector<ForeignKey> fkeys;
			QStringList fkey_string = getForeignKeyString(tablename);
			for (int i = 0; i < fkey_string.size(); i += 3){
				fkeys.push_back(ForeignKey(fkey_string.at(i), fkey_string.at(i + 1), fkey_string.at(i + 2)));
			}
			foreignKeysByTable[tablename] = fkeys;
		}
		return foreignKeysByTable[tablename];
	}

	std::vector<QStringList> DataEngineConnection::getSampleData(QString tablename){

		if (sampleDataByTable.find(tablename) == sampleDataByTable.end()) {
			std::vector<QStringList> rows;
			for (int i = 0; i < 15; i++){
				rows.push_back(getRowOfSampleData(tables.indexOf(tablename), i));
			}
			sampleDataByTable[tablename] = rows;
		}
		return sampleDataByTable[tablename];
	}

	QStringList DataEngineConnection::getForeignKeyString(QString tablename){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getForeignKeys", "(Ljava/lang/String;)[Ljava/lang/String;");
		jobjectArray itr;
		QStringList temp;
		if (methodId != NULL) {
			jstring name = jniEnv->NewStringUTF(tablename.toStdString().c_str());
			itr = (jobjectArray)jniEnv->CallStaticObjectMethod(jcls, methodId, name);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}

			int length = jniEnv->GetArrayLength(itr);

			for (int i = 0; i < length; i++){
				jstring element = (jstring)jniEnv->GetObjectArrayElement(itr, i);
				const char *str = jniEnv->GetStringUTFChars(element, 0);
				QString fkey_str(str);
				temp << fkey_str;
			}
		}
		return temp;
	}

	QStringList DataEngineConnection::getRowOfSampleData(int index, int row){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getSampleData", "(II)[Ljava/lang/String;");
		jobjectArray itr;
		QStringList samplerow;
		if (methodId != NULL) {
			itr = (jobjectArray)jniEnv->CallStaticObjectMethod(jcls, methodId, index, row);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}

			int length = jniEnv->GetArrayLength(itr);

			for (int i = 0; i < length; i++){
				jstring element = (jstring)jniEnv->GetObjectArrayElement(itr, i);
				const char *str = jniEnv->GetStringUTFChars(element, 0);
				QString col_value(str);
				samplerow << col_value;
			}
		}
		return samplerow;
	}

	void DataEngineConnection::closeConnection(){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"closeConnection", "()V");

		if (methodId != NULL) {
			jniEnv->CallStaticVoidMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
	}

	//JDBC END

}