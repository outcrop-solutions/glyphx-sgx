#include "useraccesscontrols.h"
#include <QtCore/QSettings>

namespace DataEngine
{
	UserAccessControls::UserAccessControls(JNIEnv *env) :
		jniEnv(env),
		validConnection(false),
        synced(false),
		appVersion("")
	{
		jcls = jniEnv->FindClass("UserAccessControls");
	}

	void UserAccessControls::InitializeConnection(){

		bool connected = false;
		if (jcls != NULL) {
			jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
				"initConnection", "()Z");
			if (methodId != NULL) {
				connected = jniEnv->CallStaticBooleanMethod(jcls, methodId);

				if (jniEnv->ExceptionCheck()) {
					jniEnv->ExceptionDescribe();
					jniEnv->ExceptionClear();
				}
			}
		}
		validConnection = connected;
	}

	bool UserAccessControls::IsValidConnection(){
		return validConnection;
	}

	void UserAccessControls::ResetConnection(){
		synced = false;
		validConnection = false;
		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"logOutCurrentUser", "()V");
		if (methodId != NULL) {
			jniEnv->CallStaticVoidMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
	}

	void UserAccessControls::FlushOutFilterSetup(){
		synced = false;
		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"flushOutFilterSetup", "()V");
		if (methodId != NULL) {
			jniEnv->CallStaticVoidMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
	}

	int UserAccessControls::ValidateCredentials(QString username, QString password){

		valid = 0;
		if (jcls != NULL) {
			jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
				"validateCredentials", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I");
			if (methodId != NULL) {
				jstring un = jniEnv->NewStringUTF(username.toStdString().c_str());
				jstring pw = jniEnv->NewStringUTF(password.toStdString().c_str());
				jstring vn = jniEnv->NewStringUTF(appVersion.toStdString().c_str());
				valid = jniEnv->CallStaticIntMethod(jcls, methodId, un, pw, vn);

				if (jniEnv->ExceptionOccurred()) {
					jniEnv->ExceptionDescribe();
					jniEnv->ExceptionClear();
				}
			}
		}
		if (valid == 0){
			QSettings settings;
			settings.beginGroup("LoggedInUser");
			settings.setValue("StayLogged", false);
			settings.endGroup();
		}
		else if (valid == 1){
			validConnection = true;
		}
		return valid;
	}

	bool UserAccessControls::GenerateLicenseKey(QString path){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"generateLicenseKey", "(Ljava/lang/String;)Z");
		bool success = false;
		if (methodId != NULL) {
			jstring fp = jniEnv->NewStringUTF(path.toStdString().c_str());
			success = jniEnv->CallStaticBooleanMethod(jcls, methodId, fp);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
		return success;
	}

	int UserAccessControls::GetUserID(){

		if (valid == 2){
			return presetId.toInt();
		}
		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getUserID", "()I");
		int id = 0;
		if (methodId != NULL) {
			id = (jint)jniEnv->CallStaticIntMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
		return id;
	}

	int UserAccessControls::GetLicenseType(){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getLicenseType", "()I");
		int type = 0;
		if (methodId != NULL) {
			type = (jint)jniEnv->CallStaticIntMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
		return type;
	}

	QString UserAccessControls::NameOfUser(){

		if (valid == 2){
			return presetName;
		}
		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"nameOfUser", "()Ljava/lang/String;");
		jstring itr = NULL;
		if (methodId != NULL) {
			itr = (jstring)jniEnv->CallStaticObjectMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
		return QString(jniEnv->GetStringUTFChars(itr, JNI_FALSE));
	}

	QString UserAccessControls::NameOfDirectory(){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getS3Directory", "()Ljava/lang/String;");
		jstring itr = NULL;
		if (methodId != NULL) {
			itr = (jstring)jniEnv->CallStaticObjectMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
		return QString(jniEnv->GetStringUTFChars(itr, JNI_FALSE));
	}

	QString UserAccessControls::NameOfInstitution(){

		if (valid == 2){
			return presetInstitution;
		}
		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"nameOfInstitution", "()Ljava/lang/String;");
		jstring itr = NULL;
		if (methodId != NULL) {
			itr = (jstring)jniEnv->CallStaticObjectMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
		return QString(jniEnv->GetStringUTFChars(itr, JNI_FALSE));
	}

	QString UserAccessControls::LastModified(){
		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"lastModified", "()Ljava/lang/String;");
		jstring itr = NULL;
		if (methodId != NULL) {
			itr = (jstring)jniEnv->CallStaticObjectMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
		return QString(jniEnv->GetStringUTFChars(itr, JNI_FALSE));
	}

	QStringList UserAccessControls::VizualizationNames(){

		if (valid == 2){
			return vizNames;
		}

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"visualizationNames", "()[Ljava/lang/String;");
		jobjectArray itr;
		QStringList names;
		if (methodId != NULL) {
			itr = (jobjectArray)jniEnv->CallStaticObjectMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}

			int length = jniEnv->GetArrayLength(itr);

			for (int j = 0; j < length; j++){
				jstring element = (jstring)jniEnv->GetObjectArrayElement(itr, j);
				const char *str = jniEnv->GetStringUTFChars(element, 0);
				QString name(str);
				names << name;
			}
		}
		return names;
	}

	QStringList UserAccessControls::GetFormattedGroupNames(){

		if (valid == 2){
			QSettings settings;
			QStringList childGroups = settings.childGroups();
			for (auto group : groupNames){
				if (!childGroups.contains(group)){
					groupNames.removeAll(group);
				}
			}
			return groupNames;
		}

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getListOfFormattedGroupNames", "()[Ljava/lang/String;");
		jobjectArray itr;
		QStringList names;
		if (methodId != NULL) {
			itr = (jobjectArray)jniEnv->CallStaticObjectMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}

			int length = jniEnv->GetArrayLength(itr);

			for (int j = 0; j < length; j++){
				jstring element = (jstring)jniEnv->GetObjectArrayElement(itr, j);
				const char *str = jniEnv->GetStringUTFChars(element, 0);
				QString name(str);
				names << name;
			}
		}
		return names;
	}

	void UserAccessControls::SetChosenGroup(QString name){

		if (valid == 2){
			QSettings groupSettings;
			groupSettings.beginGroup(name);
			PresetLogoPath(groupSettings.value("DirectoryPath", "").toString());
			SetVisualizationNames(groupSettings.value("VisualizationNames", "").toStringList());
			groupSettings.endGroup();
			return;
		}

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"setChosenGroup", "(Ljava/lang/String;)V");
		if (methodId != NULL) {
			jstring fp = jniEnv->NewStringUTF(name.toStdString().c_str());
			jniEnv->CallStaticVoidMethod(jcls, methodId, fp);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}

	}

	bool UserAccessControls::FileSyncSetup(QString path){

		if (valid == 2){
			return 0;
		}

		synced = true;
		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"fileSyncSetup", "(Ljava/lang/String;)Z");
		bool needed = false;
		if (methodId != NULL) {
			jstring fp = jniEnv->NewStringUTF(path.toStdString().c_str());
			needed = jniEnv->CallStaticBooleanMethod(jcls, methodId, fp);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
		return needed;
	}

	int UserAccessControls::VisualizationsToSync(){
		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"visualizationsToSync", "()I");
		int count = 0;
		if (methodId != NULL) {
			count = (jint)jniEnv->CallStaticIntMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
		return count;
	}

	void UserAccessControls::StartSyncingFiles(){
		
		if (valid != 2){
			jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
				"startSyncingFiles", "()V");
			if (methodId != NULL) {
				jniEnv->CallStaticVoidMethod(jcls, methodId);
				if (jniEnv->ExceptionCheck()) {
					jniEnv->ExceptionDescribe();
					jniEnv->ExceptionClear();
				}
			}
		}
	}

	int UserAccessControls::FilesSynced(){
		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"filesSynced", "()I");
		int count = 0;
		if (methodId != NULL) {
			count = (jint)jniEnv->CallStaticIntMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
		return count;
	}

	QString UserAccessControls::GlyphEdPath(){

		if (!synced){
			return presetLogoPath;
		}

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getGlyphEdPath", "()Ljava/lang/String;");
		jstring itr = NULL;
		if (methodId != NULL) {
			itr = (jstring)jniEnv->CallStaticObjectMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
		return QString(jniEnv->GetStringUTFChars(itr, JNI_FALSE));
	}

	bool UserAccessControls::HasSynced(){

		return synced;
	}

	void UserAccessControls::PresetLogoPath(QString path){
		presetLogoPath = path;
	}

	void UserAccessControls::SetVisualizationGroupNames(QStringList groups){
		groupNames = groups;
	}

	void UserAccessControls::SetVisualizationNames(QStringList vizs){
		
		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"setVisualizationNames", "(Ljava/lang/String;[Ljava/lang/String;)V");

		jstring fp = jniEnv->NewStringUTF(presetLogoPath.toStdString().c_str());
		jobjectArray selected = (jobjectArray)jniEnv->NewObjectArray(vizs.size(), jniEnv->FindClass("java/lang/String"), jniEnv->NewStringUTF(""));

		if (methodId != NULL) {

			for (int i = 0; i < vizs.size(); i++){
				jniEnv->SetObjectArrayElement(selected, i, jniEnv->NewStringUTF(vizs.at(i).toStdString().c_str()));
			}

			jniEnv->CallStaticObjectMethod(jcls, methodId, fp, selected);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
			vizNames = vizs;
		}
		
	}

	void UserAccessControls::SetUsersInformation(QString id, QString name, QString inst){
		presetId = id;
		presetName = name;
		presetInstitution = inst;
	}

	int UserAccessControls::CheckAvailableGroups(){

		if (valid == 2){
			return groupNames.size();
		}

		int group_count = 0;
		if (jcls != NULL) {
			jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
				"checkAvailableGroups", "()I");
			if (methodId != NULL) {
				group_count = jniEnv->CallStaticIntMethod(jcls, methodId);

				if (jniEnv->ExceptionCheck()) {
					jniEnv->ExceptionDescribe();
					jniEnv->ExceptionClear();
				}
			}
		}
		return group_count;
	}

	int UserAccessControls::GetSyncProgress(){

		long progress = 0;
		if (jcls != NULL) {
			jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
				"getSyncProgress", "()J");
			if (methodId != NULL) {
				progress = jniEnv->CallStaticLongMethod(jcls, methodId);

				if (jniEnv->ExceptionCheck()) {
					jniEnv->ExceptionDescribe();
					jniEnv->ExceptionClear();
				}
			}
		}
		return static_cast<int>(progress);
	}

	bool UserAccessControls::IsDoneSyncing(){

		if (valid == 2){
			return true;
		}

		bool done = false;
		if (jcls != NULL) {
			jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
				"isDoneSyncing", "()Z");
			if (methodId != NULL) {
				done = jniEnv->CallStaticBooleanMethod(jcls, methodId);

				if (jniEnv->ExceptionCheck()) {
					jniEnv->ExceptionDescribe();
					jniEnv->ExceptionClear();
				}
			}
		}
		return done;
	}
}
