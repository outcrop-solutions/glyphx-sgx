#include "glyphengine.h"
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtWidgets/QMessageBox>
#include <fstream>
#include "networkdownloader.h"
#include "downloadexception.h"
#include "downloadedmapproperties.h"
#include "baseimage.h"
#include "application.h"

namespace DataEngine
{
	void GlyphEngine::initiate(JNIEnv *env, std::string sdtPath, std::string outDir, std::string bid, std::string bfn, std::string appName){

		jniEnv = env;
		sdtFile = sdtPath;
		baseOutputDir = outDir;
		baseImageDir = bid;
		baseFilename = bfn;
		application = appName;
		/*
		std::ofstream myfile;
		myfile.open("dmlog.txt");
		myfile << outDir;
		myfile << appName;
		*/

		jcls = jniEnv->FindClass("GlyphEngine");
		if (jcls == NULL){
			//myfile << "GlyphEngine class not found";
		}
		if (jcls != NULL) {
			//qDebug() << "GlyphEngine class found";
			jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
				"initiate", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I");
			int err_code = 0;
			if (methodId != NULL) {
				jstring str = jniEnv->NewStringUTF(sdtPath.c_str());
				jstring out = jniEnv->NewStringUTF(outDir.c_str());
				jstring exp = jniEnv->NewStringUTF(appName.c_str());
				err_code = (jint)jniEnv->CallStaticIntMethod(jcls, methodId, str, out, exp);

				if (jniEnv->ExceptionCheck()) {
					jniEnv->ExceptionDescribe();
					jniEnv->ExceptionClear();
				}
			}
			else{
				//myfile << "Method ID not found";
			}
		}
		//myfile.close();

	}

	void GlyphEngine::prepare(){

		if (application == "GlyphViewer") {

			QString bod(baseOutputDir.c_str());

			QDir dir(bod);
			if (!dir.exists()) {

				if (!dir.mkpath(bod)) {

					throw std::runtime_error("Instance directory was not created");
				}
			}

			createCacheDirectory();
		}

	}

	void GlyphEngine::createCacheDirectory() {

		if (!boost::filesystem::exists(baseOutputDir + "/antz")){
			if (!boost::filesystem::create_directories(baseOutputDir + "/antz")) {

				throw std::invalid_argument("CopyDirectory: Unable to create destination directory");
			}
		}

		boost::filesystem::copy_file(sdtFile, baseOutputDir+"/mapping.sdt", boost::filesystem::copy_option::overwrite_if_exists);

	}

	std::vector<std::string> GlyphEngine::getBaseImages(){

		if (images.size() >= 1){
			return images;
		}

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"getBaseImages", "()[Ljava/lang/String;");
		jobjectArray itr;
		if (methodId != NULL) {
			//qDebug() << "Returning count...";
			itr = (jobjectArray)jniEnv->CallStaticObjectMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {

				jthrowable exc;
				exc = jniEnv->ExceptionOccurred();
				jboolean isCopy = false;
				jmethodID toString = jniEnv->GetMethodID( jniEnv->FindClass( "java/lang/Object" ), "toString", "()Ljava/lang/String;" );
				jstring s = (jstring)( jniEnv )->CallObjectMethod( exc, toString );
				const char* utf = ( jniEnv )->GetStringUTFChars( s, &isCopy );

				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}


			//qDebug() << itr;

			int length = jniEnv->GetArrayLength(itr);

			for (int i = 0; i < length; i++){
				jstring element = (jstring)jniEnv->GetObjectArrayElement(itr, i);
				const char *str = jniEnv->GetStringUTFChars(element, 0);
				if (application == "GlyphViewer"){
					std::string name(str);
					std::string dl = "downloadedMap.jpg";
					if (name == dl && downloadComplete){
						images.push_back(baseOutputDir + "antz/base_image_2.png");
					}
					else if (boost::filesystem::exists(boost::filesystem::path(baseImageDir + name))){
						images.push_back(baseImageDir + name);
					}
					else if (boost::filesystem::exists(boost::filesystem::path(name))){
						images.push_back(name);
					}
				}
				else{
					images.push_back(str);
				}
			}
		}
		return images;
	}

	void GlyphEngine::copyBaseImages(){

		QString bod(baseOutputDir.c_str());
		QString baseUsrImageDir = QDir::toNativeSeparators(bod + "usr" + QDir::separator() + "images" + QDir::separator());

		images = getBaseImages();
		QString num;
		QString file;
		int offset = 2;
		for (int i = 0; i < images.size(); i++){
			file = (images.at(i)).c_str();
			if (i == 0 && images.at(i) == "World.png"){ offset = 1; }
			if (application == "DataMapper"){
				num = "map0000" + QString::number(i + offset) + ".jpg";
				if (images.at(i) == "downloadedMap.jpg" && downloadComplete){
					//QFile::copy((baseImageDir).c_str() + file, baseUsrImageDir + num);
					//boost::filesystem::remove(baseImageDir + "downloadedMap.jpg");
					boost::filesystem::rename((baseUsrImageDir + "downloadedMap.jpg").toStdString(), (baseUsrImageDir + num).toStdString());

				}
				else if (boost::filesystem::exists(boost::filesystem::path(baseImageDir + images.at(i)))){
					QFile::copy((baseImageDir).c_str() + file, baseUsrImageDir + num);
				}
				else if (boost::filesystem::exists(boost::filesystem::path(images.at(i)))){
					QFile::copy((images.at(i)).c_str(), baseUsrImageDir + num);
				}
			}
			else if (application == "GlyphViewer"){
				if (images.size() > 1 && i > 0){
					num = "base_image_" + QString::number(i + offset) + ".png";
					if (boost::filesystem::exists(boost::filesystem::path(baseImageDir + images.at(i)))){
						QFile::copy((baseImageDir).c_str() + file, bod + "antz/" + num);
					}
					else if (boost::filesystem::exists(boost::filesystem::path(images.at(i)))){
						QFile::copy((images.at(i)).c_str(), bod + "antz/" + num);
					}
				}
			}
		}

		if (application == "DataMapper"){
			QFile::copy(QString((baseImageDir + "World.png").c_str()), baseUsrImageDir + "map00001.jpg");
			num = "map0000" + QString::number(images.size() + offset) + ".jpg";
			QFile::copy(QString("logo.png"), baseUsrImageDir + num);
		}
	}

	void GlyphEngine::setGeoBoundingBox(std::vector<double> nw, std::vector<double> se, std::vector<double> size){
		
		if (jcls != NULL) {
			jdoubleArray nw_p = jniEnv->NewDoubleArray(2);
			jdoubleArray se_p = jniEnv->NewDoubleArray(2);
			jdoubleArray img = jniEnv->NewDoubleArray(2);
			jniEnv->SetDoubleArrayRegion(nw_p, 0, 2, &nw[0]);
			jniEnv->SetDoubleArrayRegion(se_p, 0, 2, &se[0]);
			jniEnv->SetDoubleArrayRegion(img, 0, 2, &size[0]);
			jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
				"setBoundingBox", "([D[D[D)V");
			if (methodId != NULL) {
				jniEnv->CallStaticVoidMethod(jcls, methodId, nw_p, se_p, img);
				if (jniEnv->ExceptionCheck()) {
					jniEnv->ExceptionDescribe();
					jniEnv->ExceptionClear();
				}
			}
		}
	}

	bool GlyphEngine::getDownloadedBaseImage(std::vector<SynGlyphX::BaseImage> baseImages){
		
		prepare();
		unsigned int nextTextureID = NumberOfDefaultBaseImages + 1;
		for (const SynGlyphX::BaseImage& baseImage : baseImages) {

			if (baseImage.GetType() == SynGlyphX::BaseImage::Type::DownloadedMap) {

				QString downloadedImageFilename;
				if (application == "DataMapper"){
					downloadedImageFilename = QString((baseOutputDir + "usr/images/downloadedMap.jpg").c_str());
				}
				else if (application == "GlyphViewer"){
					downloadedImageFilename = QString((baseOutputDir + "antz/base_image_2.png").c_str());
				}
				return downloadBaseImage(baseImage, downloadedImageFilename);
			}
		}
		return true;
	}

	bool GlyphEngine::downloadBaseImage(const SynGlyphX::BaseImage& baseImage, QString baseImageFilename) {

		std::vector<GeographicPoint> points; //Set to ne and sw points
		std::vector<double> pts = getNWandSE();
		GeographicPoint ne(pts.at(2),pts.at(1));
		GeographicPoint sw(pts.at(0),pts.at(3));
		points.push_back(sw);
		points.push_back(ne);

		NetworkDownloader& downloader = NetworkDownloader::Instance();
		GeographicBoundingBox m_overrideRootXYBoundingBox;
		GeographicPoint sw_p;
		GeographicPoint ne_p;
		std::vector<double> nw;
		std::vector<double> se;
		if (hasImageBeenUpdated()){
			if (QFile::exists(baseImageFilename)) {

				if (!QFile::remove(baseImageFilename)) {

					throw std::runtime_error("Failed to remove old base image");
				}
			}
			try {
				m_overrideRootXYBoundingBox = downloader.DownloadMap(points, baseImageFilename.toStdString(), std::dynamic_pointer_cast<const SynGlyphX::DownloadedMapProperties>(baseImage.GetProperties()));
				sw_p = m_overrideRootXYBoundingBox.GetSWCorner();
				ne_p = m_overrideRootXYBoundingBox.GetNECorner();
				nw = { sw_p.get<0>(), ne_p.get<1>() };
				se = { ne_p.get<0>(), sw_p.get<1>() };
				setGeoBoundingBox(nw, se, downloader.getImageSize());
				return true;
			}
			catch (const DownloadException& e) {

				downloadComplete = false;
				GeographicBoundingBox m_overrideRootXYBoundingBox = GeographicBoundingBox(GeographicPoint(0.0, 0.0), 90.0, 180.0);
				throw;
			}
		}
		else{
			m_overrideRootXYBoundingBox = downloader.DownloadMap(points, std::dynamic_pointer_cast<const SynGlyphX::DownloadedMapProperties>(baseImage.GetProperties()));
			sw_p = m_overrideRootXYBoundingBox.GetSWCorner();
			ne_p = m_overrideRootXYBoundingBox.GetNECorner();
			nw = { sw_p.get<0>(), ne_p.get<1>() };
			se = { ne_p.get<0>(), sw_p.get<1>() };
			setGeoBoundingBox(nw, se, downloader.getImageSize());
			return true;
		}
	}

	bool GlyphEngine::hasImageBeenUpdated(){

		bool updated = true;
		if (jcls != NULL) {

			jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
				"hasImageBeenUpdated", "()Z");
			if (methodId != NULL) {
				updated = jniEnv->CallStaticBooleanMethod(jcls, methodId);
				if (jniEnv->ExceptionCheck()) {
					jniEnv->ExceptionDescribe();
					jniEnv->ExceptionClear();
				}
			}
		}
		return updated;
	}

	bool GlyphEngine::IsUpdateNeeded() const {

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"isUpdateNeeded", "()Z");

		if (methodId != NULL) {
			bool update = (jboolean)jniEnv->CallStaticBooleanMethod(jcls, methodId);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
			return update;
		}
		return true;
	}

	std::vector<double> GlyphEngine::getNWandSE(){

		std::vector<double> points;
		if (jcls != NULL) {
			jdoubleArray nw_se = jniEnv->NewDoubleArray(4);
			jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
				"getNWandSE", "()[D");
			if (methodId != NULL) {
				nw_se = (jdoubleArray)jniEnv->CallStaticObjectMethod(jcls, methodId);
				if (jniEnv->ExceptionCheck()) {
					jniEnv->ExceptionDescribe();
					jniEnv->ExceptionClear();
				}
			}
			jdouble *each = jniEnv->GetDoubleArrayElements(nw_se, 0);
			for (jint i = 0; i < 4; i++){
				points.push_back(each[i]);
			}
			jniEnv->ReleaseDoubleArrayElements(nw_se, each, 0);
		}
		return points;
	}

	void GlyphEngine::generateGlyphs(QWidget *mainWindow){

		if (!downloadComplete){
			std::vector<double> nw = { -180.0, 90.0 };
			std::vector<double> se = { 180.0, -90.0 };
			std::vector<double> size = {2048.0, 1024.0};
			setGeoBoundingBox(nw, se, size);
		}

		int err_code = 0;
		if (jcls != NULL) {
			jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
				"beginGlyphGeneration", "()I");
			if (methodId != NULL) {
				err_code = (jint)jniEnv->CallStaticIntMethod(jcls, methodId);
				if (jniEnv->ExceptionCheck()) {
					jniEnv->ExceptionDescribe();
					jniEnv->ExceptionClear();
				}
			}

		}
		if (err_code == 1){
			throw std::runtime_error("Failed to generate glyphs");
		}
		else if (err_code == 2){
			SynGlyphX::Application::restoreOverrideCursor();
			QMessageBox warning_error(QMessageBox::Warning, mainWindow->tr("Failed To Open Project"), mainWindow->tr("The visualization has loaded, but errors were thrown. The visualization may appear different than intended"), QMessageBox::Ok, mainWindow);
			warning_error.setDetailedText(JavaErrors());
			warning_error.setStyleSheet("QLabel{margin-right:75px;},QTextEdit{min-width:500px;}");
			warning_error.setStandardButtons(QMessageBox::Ok);
			warning_error.setDefaultButton(QMessageBox::Ok);
			warning_error.setEscapeButton(QMessageBox::Ok);
			warning_error.exec();
			ClearJavaErrors();
			SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
		}
		copyBaseImages();
	}

	QStringList GlyphEngine::DistinctValuesForField(QString id, QString table, QString field){

		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"distinctValuesForField", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)[Ljava/lang/String;");
		jobjectArray itr;
		QStringList values;
		if (methodId != NULL) {
			jstring jid = jniEnv->NewStringUTF(id.toStdString().c_str());
			jstring jtbl = jniEnv->NewStringUTF(table.toStdString().c_str());
			jstring jfld = jniEnv->NewStringUTF(field.toStdString().c_str());
			itr = (jobjectArray)jniEnv->CallStaticObjectMethod(jcls, methodId, jid, jtbl, jfld);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}

			int length = jniEnv->GetArrayLength(itr);

			for (int i = 0; i < length; i++){
				jstring element = (jstring)jniEnv->GetObjectArrayElement(itr, i);
				if (length == 1 && element == NULL){ break; }
				const char *str = jniEnv->GetStringUTFChars(element, 0);
				QString value(str);
				values << value;
			}
		}

		return values;
	}

	void GlyphEngine::SetQueryForDatasource(QString id, QString table, QString query){
		
		jmethodID methodId = jniEnv->GetStaticMethodID(jcls,
			"setQueryForDatasource", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
		if (methodId != NULL) {
			jstring jid = jniEnv->NewStringUTF(id.toStdString().c_str());
			jstring jtbl = jniEnv->NewStringUTF(table.toStdString().c_str());
			jstring jqry = jniEnv->NewStringUTF(query.toStdString().c_str());
			jniEnv->CallStaticVoidMethod(jcls, methodId, jid, jtbl, jqry);
			if (jniEnv->ExceptionCheck()) {
				jniEnv->ExceptionDescribe();
				jniEnv->ExceptionClear();
			}
		}
	}

	QString GlyphEngine::JavaErrors(){

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
				errors += "\n" + error;
			}
		}
		return errors;
	}

	void GlyphEngine::ClearJavaErrors(){

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
}
