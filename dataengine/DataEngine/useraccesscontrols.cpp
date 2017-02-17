#include "useraccesscontrols.h"

namespace DataEngine
{
	UserAccessControls::UserAccessControls(JNIEnv *env) :
		jniEnv(env),
		validConnection(false),
        synced(false)
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

	void UserAccessControls::SetVisualizationNames(QStringList vizs){
		
		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"", "(Ljava/lang/String;[Ljava/lang/String;)V");

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

	void UserAccessControls::SetUsersNameAndInstitution(QString name, QString inst){
		presetName = name;
		presetInstitution = inst;
	}

	int UserAccessControls::CheckAvailableGroups(){

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
