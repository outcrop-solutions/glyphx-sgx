#include "useraccesscontrols.h"

namespace DataEngine
{
	UserAccessControls::UserAccessControls(JNIEnv *env) :
		jniEnv(env),
		synced(false),
		validConnection(false)
	{
		jcls = jniEnv->FindClass("UserAccessControls");
	}

	void UserAccessControls::InitializeConnection(){

		bool connected = false;
		if (jcls != NULL) {
			jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
				"initConnection", "()Z");
			if (methodId != NULL) {
				connected = jniEnv->CallStaticIntMethod(jcls, methodId);

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

	int UserAccessControls::ValidateCredentials(QString username, QString password){

		valid = 0;
		if (jcls != NULL) {
			jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
				"validateCredentials", "(Ljava/lang/String;Ljava/lang/String;)I");
			if (methodId != NULL) {
				jstring un = jniEnv->NewStringUTF(username.toStdString().c_str());
				jstring pw = jniEnv->NewStringUTF(password.toStdString().c_str());
				valid = jniEnv->CallStaticIntMethod(jcls, methodId, un, pw);

				if (jniEnv->ExceptionCheck()) {
					jniEnv->ExceptionDescribe();
					jniEnv->ExceptionClear();
				}
			}
		}

		return valid;

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

	int UserAccessControls::InstitutionID(){
		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"institutionID", "()I");
		int id;
		if (methodId != NULL) {
			id = (jint)jniEnv->CallStaticObjectMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
		return id;
	}

	QStringList UserAccessControls::VizualizationNames(){
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

	int UserAccessControls::FileSyncSetup(QString path){

		if (valid == 2){
			return 0;
		}

		synced = true;
		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"fileSyncSetup", "(Ljava/lang/String;)I");
		int count;
		if (methodId != NULL) {
			jstring fp = jniEnv->NewStringUTF(path.toStdString().c_str());
			count = (jint)jniEnv->CallStaticObjectMethod(jcls, methodId, fp);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
		return count;
	}

	int UserAccessControls::VisualizationsToSync(){
		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"visualizationsToSync", "()I");
		int count;
		if (methodId != NULL) {
			count = (jint)jniEnv->CallStaticObjectMethod(jcls, methodId);
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
		int count;
		if (methodId != NULL) {
			count = (jint)jniEnv->CallStaticObjectMethod(jcls, methodId);
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

	void UserAccessControls::SetUsersNameAndInstitution(QString name, QString inst){
		presetName = name;
		presetInstitution = inst;
	}
}