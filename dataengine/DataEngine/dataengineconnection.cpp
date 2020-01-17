#pragma warning(disable: 4996)

#include "dataengineconnection.h"
#include "dataenginestatement.h"
#include <fstream>
#include "utilitytypes.h"
#include <boost/filesystem.hpp>
//#include <QtCore/QDebug>

namespace DataEngine
{
	typedef jint(JNICALL *CreateJVMFunc)(JavaVM **pvm, void **penv, void *args);

	DataEngineConnection::DataEngineConnection() :
		classFound(false) {


	}

	DataEngineConnection::~DataEngineConnection() {


	}

	void DataEngineConnection::createJVM(){
        //"../Frameworks/jdk-13.0.1.jdk/Contents/Home/lib/server/libjvm.dylib";
        std::string jre_name =
#ifdef WIN32
        ".\\jre\\bin\\server\\jvm.dll";
#elif __APPLE__
        "../Frameworks/jdk-13.0.1.jdk/Contents/MacOS/libjli.dylib";
#endif
        
        std::ifstream jre(jre_name);
        
        if (jre){
		
			jvmDll.Load(jre_name.c_str());
		}
#ifdef WIN32
        else{
			
			jvmDll.Load("..\\..\\DataEngine\\jdk\\bin\\server\\jvm.dll");
		}
#endif
		jre.close();
		
		CreateJVMFunc CreateJVM = (CreateJVMFunc)jvmDll.GetAddress("JNI_CreateJavaVM");

        const auto option_count = 3u;
		JavaVMInitArgs vmArgs;
        memset(&vmArgs, 0, sizeof(vmArgs));
		JavaVMOption options[option_count];
        memset(&options, 0, sizeof(options));
		std::ifstream ifile
#ifdef __APPLE__
        ("../Resources/DataEngine/dataengine.jar");
#else
        (".\\dataengine.jar");
#endif
        
#ifdef WIN32
		char jarFileSeparator = ';';
#else
		char jarFileSeparator = ':';
#endif

        // Note: please use forward slashes! Paths with \\ are NOT cross-platform.
		
		std::string jarFilePrefix;
		if (ifile) {
#ifdef __APPLE__
			jarFilePrefix = "../Resources/DataEngine/";
#else
			jarFilePrefix = ".\\";
#endif
		} else {
			
			jarFilePrefix = "..\\..\\classes\\";
		}

		std::string jarFilesOptionString = "-Djava.class.path=";
		jarFilesOptionString += jarFilePrefix + "dataengine.jar" + jarFileSeparator;
		boost::filesystem::path dir(jarFilePrefix+"database-drivers/");
		boost::filesystem::directory_iterator end_iter;
		for (boost::filesystem::directory_iterator dir_iter(dir); dir_iter != end_iter; ++dir_iter){
			jarFilesOptionString += dir_iter->path().string() + jarFileSeparator;
		}

		ifile.close();
		options[0].optionString = const_cast<char*>(jarFilesOptionString.c_str());
        options[1].optionString = const_cast<char*>("-Xmx4G"); //Max of 4G
		options[2].optionString = const_cast<char*>("-XX:+UseConcMarkSweepGC"); //Make GC in JVM run concurrently

        vmArgs.version = JNI_VERSION_1_8;
		vmArgs.options = options;
		vmArgs.nOptions = option_count;
		vmArgs.ignoreUnrecognized = JNI_FALSE;

		// Create the JVM
		long flag = CreateJVM(&javaVM, (void**)&jniEnv, &vmArgs);
		//flag = JNI_CreateJavaVM(&javaVM, (void**)&jniEnv, &vmArgs);
		
		if (flag == JNI_ERR) {
			
			throw std::runtime_error("JVM Error: Unknown Error");
		}
		else if (flag == JNI_EDETACHED) {

			throw std::runtime_error("JVM Error: Thread detached from VM");
		}
		else if (flag == JNI_EVERSION) {

			throw std::runtime_error("JVM Error: Version Error");
		}
		else if (flag == JNI_ENOMEM) {

			throw std::runtime_error("JVM Error: Not Enough Memory");
		}
		else if (flag == JNI_EEXIST) {

			throw std::runtime_error("JVM Error: JVM already created");
		}
		else if (flag == JNI_EINVAL) {

			throw std::runtime_error("JVM Error: Invalid Arguments");
		}

		jcls = jniEnv->FindClass("DataEngine");

		if (jcls == NULL) {
			jniEnv->ExceptionDescribe();
			javaVM->DestroyJavaVM();
			throw std::runtime_error("JVM Error: Java class could not be found");
		}
		if (jcls != NULL) {
			classFound = true;
		}

		uac = new DataEngine::UserAccessControls(jniEnv);
	}

	bool DataEngineConnection::hasJVM() const {
		
		return classFound;
	}

	void DataEngineConnection::loadCSV(std::string path){

		if (classFound){

			if (IsConnectionOpen()) {

				closeConnection();
			}

			jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
				"loadFromCSV", "(Ljava/lang/String;)I");
			int err_code = 0;
			if (methodId != NULL) {
				jstring str = jniEnv->NewStringUTF(path.c_str());
				err_code = (jint)jniEnv->CallStaticIntMethod(jcls, methodId, str);
				if (jniEnv->ExceptionCheck()) {
					jniEnv->ExceptionDescribe();
					jniEnv->ExceptionClear();
				}
			}

			if (err_code == 1){
				throw std::runtime_error("Failed to load CSV.");
			}

			m_openConnection = QString::fromStdString(path);
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

	JNIEnv* DataEngineConnection::getEnv() const {
		
		return jniEnv;
	}

	jclass DataEngineConnection::getJcls() const {
		
		return jcls;
	}

	void DataEngineConnection::destroyJVM() {

		javaVM->DestroyJavaVM();
	}

	QString DataEngineConnection::VersionNumber(){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getVersion", "()Ljava/lang/String;");
		QString version;
		if (methodId != NULL) {
			jstring ver = (jstring)jniEnv->CallStaticObjectMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
			const char *str = jniEnv->GetStringUTFChars(ver, 0);
			version = str;
		}
		return "DataEngine v"+version;

	}

	//JDBC ACCESSOR FUNCTIONS
	QStringList DataEngineConnection::connectToServer(QString db_url, QString user, QString pass, QString db_type){

		if (IsConnectionOpen()) {

			closeConnection();
		}

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"connectToServer", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I");
		int err_code = 0;
		if (methodId != NULL) {
			jstring db = jniEnv->NewStringUTF(db_url.toStdString().c_str());
			jstring usr = jniEnv->NewStringUTF(user.toStdString().c_str());
			jstring pwd = jniEnv->NewStringUTF(pass.toStdString().c_str());
			jstring type = jniEnv->NewStringUTF(db_type.toStdString().c_str());
			err_code = (jint)jniEnv->CallStaticIntMethod(jcls, methodId, db, usr, pwd, type);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}

			m_openConnection = db_url;

			if (err_code != 0){
				throw std::runtime_error("Failed to connect to database.");
			}

		}
		setTables();
		return getSchemas();
	}

	QStringList DataEngineConnection::getSchemas(){
		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getSchemas", "()[Ljava/lang/String;");
		jobjectArray itr;
		QStringList schemas;
		if (methodId != NULL) {
			itr = (jobjectArray)jniEnv->CallStaticObjectMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}

			int length = jniEnv->GetArrayLength(itr);

			for (int i = 0; i < length; i++){
				jstring element = (jstring)jniEnv->GetObjectArrayElement(itr, i);
				if (length == 1 && element == NULL){ break; }
				const char *str = jniEnv->GetStringUTFChars(element, 0);
				QString db_name(str);
				schemas << db_name;
			}
		}
		return schemas;
	}

	void DataEngineConnection::setTables(){

		foreignKeysByTable.clear();
		sampleDataByTable.clear();
		columnNames.clear();
		tables.clear();
		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getTableNames", "()[Ljava/lang/String;");
		jobjectArray itr;
		if (methodId != NULL) {
			itr = (jobjectArray)jniEnv->CallStaticObjectMethod(jcls, methodId);
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
			tables = chosen;
		}
	}

	void DataEngineConnection::setQueryTables(QString query){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"setQueryTables", "(Ljava/lang/String;)V");

		if (methodId != NULL) {
			jstring q = jniEnv->NewStringUTF(query.toStdString().c_str());
			jniEnv->CallStaticObjectMethod(jcls, methodId, q);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
			tables.clear();
			tables << "Merged";
		}
	}

	QStringList DataEngineConnection::getTables(){
		return tables;
	}

	QStringList DataEngineConnection::getSchemaTableNames(QString schema){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getSchemaTableNames", "(Ljava/lang/String;)[Ljava/lang/String;");
		jobjectArray itr;
		QStringList tbls;
		if (methodId != NULL) {
			jstring sch = jniEnv->NewStringUTF(schema.toStdString().c_str());
			itr = (jobjectArray)jniEnv->CallStaticObjectMethod(jcls, methodId, sch);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}

			int length = jniEnv->GetArrayLength(itr);

			for (int i = 0; i < length; i++){
				jstring element = (jstring)jniEnv->GetObjectArrayElement(itr, i);
				if (length == 1 && element == NULL){ break; }
				const char *str = jniEnv->GetStringUTFChars(element, 0);
				QString tbl_name(str);
				tbls << tbl_name;
			}
		}
		return tbls;
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

	const std::vector<DataEngineConnection::ForeignKey>& DataEngineConnection::getForeignKeys(QString tablename) {

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

	const std::vector<QStringList>& DataEngineConnection::getSampleData(QString tablename) {

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
				if ((length == 1) && (element == nullptr)) {

					break;
				}
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

	int DataEngineConnection::sizeOfQuery(QString query){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"sizeOfQuery", "(Ljava/lang/String;)I");

		jint count = 0;
		if (methodId != NULL) {
			jstring name = jniEnv->NewStringUTF(query.toStdString().c_str());
			count = (jint)jniEnv->CallStaticIntMethod(jcls, methodId, name);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
		return count;
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

		m_openConnection.clear();
	}

	bool DataEngineConnection::IsConnectionOpen() const {

		return (!m_openConnection.isEmpty());
	}

	bool DataEngineConnection::IsConnectionOpen(const QString& connection) const {

		return (m_openConnection == connection);
	}

	QString DataEngineConnection::CreateInnerJoinQueryFromForiegnKeys(const QString& mainTable, const ForiegnKeyVector& foriegnKeyTables) {

		QStringList origins;

		QString query = "SELECT * FROM (";
		query += mainTable;

		for (const auto& foreignKey : foriegnKeyTables) {

			if (!origins.contains(foreignKey.origin)){
				query += " INNER JOIN " + foreignKey.origin + " ON ";
				query += "(" + mainTable + "." + foreignKey.key + "=" + foreignKey.origin + "." + foreignKey.value + ")";
				origins << foreignKey.origin;
			}
		}
		
		query += ")";

		return query;
	}

	void DataEngineConnection::queueATable(QString name, QString query){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"queueATable", "(Ljava/lang/String;Ljava/lang/String;)V");

		if (methodId != NULL) {
			jstring n = jniEnv->NewStringUTF(name.toStdString().c_str());
			jstring q = jniEnv->NewStringUTF(query.toStdString().c_str());
			jniEnv->CallStaticObjectMethod(jcls, methodId, n, q);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
	}

	void DataEngineConnection::removeQueuedTable(QString name){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"removeQueuedTable", "(Ljava/lang/String;)V");

		if (methodId != NULL) {
			jstring n = jniEnv->NewStringUTF(name.toStdString().c_str());
			jniEnv->CallStaticObjectMethod(jcls, methodId, n);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
	}

	void DataEngineConnection::executeQueuedTables(){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"executeQueuedTable", "()V");

		if (methodId != NULL) {
			jniEnv->CallStaticObjectMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
	}

	//JDBC END
	
	QString DataEngineConnection::encryptPassword(QString password){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"encryptPassword", "(Ljava/lang/String;)Ljava/lang/String;");
		QString encryp;
		if (methodId != NULL) {
			jstring q = jniEnv->NewStringUTF(password.toStdString().c_str());
			jstring encrypted = (jstring)jniEnv->CallStaticObjectMethod(jcls, methodId, q);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
			const char *str = jniEnv->GetStringUTFChars(encrypted, 0);
			encryp = str;
		}
		return encryp;

	}
	
	QString DataEngineConnection::decryptPassword(QString encrypted){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"decryptPassword", "(Ljava/lang/String;)Ljava/lang/String;");
		QString decryp;
		if (methodId != NULL) {
			jstring q = jniEnv->NewStringUTF(encrypted.toStdString().c_str());
			jstring decrypted = (jstring)jniEnv->CallStaticObjectMethod(jcls, methodId, q);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
			const char *str = jniEnv->GetStringUTFChars(decrypted, 0);
			decryp = str;
		}
		return decryp;

	}

	QString DataEngineConnection::JavaErrors(){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getErrors", "()[Ljava/lang/String;");
		jobjectArray itr;
		QString errors;
		if (methodId != NULL) {
			itr = (jobjectArray)jniEnv->CallStaticObjectMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}

			int length = jniEnv->GetArrayLength(itr);

			for (int i = 0; i < length; i++){
				jstring element = (jstring)jniEnv->GetObjectArrayElement(itr, i);
				if (length == 1 && element == NULL){ break; }
				const char *str = jniEnv->GetStringUTFChars(element, 0);
				QString error(str);
				errors += "\n"+error;
			}
		}
		return errors;
	}

	void DataEngineConnection::ClearJavaErrors(){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"clearErrors", "()V");

		if (methodId != NULL) {
			jniEnv->CallStaticVoidMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
	}

	DataEngine::UserAccessControls* DataEngineConnection::UserAccessControls(){

		return uac;
	}

	void DataEngineConnection::SetGlyphEdPath(QString path){
		glyphEdPath = path;
	}

	QString DataEngineConnection::GetGlyphEdPath(){
		return glyphEdPath;
	}

}
