#ifndef DATAENGINE_H
#define DATAENGINE_H

#include <jni.h>
#include <iostream>
#include <boost/filesystem.hpp>
#include "georeference.h"
#include <QtCore/QString>
#include "datatransformmapping.h"
#include "DataEngine_Exports.h"

namespace DataEngine
{
	class DATAENGINE GlyphEngine {

	private:
		JNIEnv *jniEnv;
		jclass jcls;
		std::string sdtFile;
		std::string baseOutputDir;
		std::string antzTemplateDir;
		std::string baseImageDir;
		std::string baseFilename;
		std::string application;
		const unsigned int NumberOfDefaultBaseImages = 1;
		bool downloadComplete = true;
	    QString m_error;
		std::vector<std::string> images;
		void prepare();
		void copyFiles();
		void createCacheDirectory();
		void copyBaseImages();
		void copyDirectoryOverwrite(const std::string& sourceDir, const std::string& destinationDir, bool recursive);
		void copyDirectoryOverwrite(const boost::filesystem::path& sourcePath, const boost::filesystem::path& destinationPath, bool recursive);
		bool downloadBaseImage(const SynGlyphX::BaseImage& baseImage, QString baseImageFilename);
		void setGeoBoundingBox(std::vector<double> nw, std::vector<double> se, std::vector<double> size);
		bool hasImageBeenUpdated();
		std::vector<double> getNWandSE();
	public:
		GlyphEngine(){};
		void initiate(JNIEnv *env, std::string sdtPath, std::string outDir, std::string antzExDir, std::string bid, std::string bfn,std::string appName);
		bool getDownloadedBaseImage(std::vector<SynGlyphX::BaseImage> baseImages);
		std::vector<std::string> getBaseImages();
		void generateGlyphs();
		QString getError();
		~GlyphEngine(){};

	};
}
#endif // DATAENGINE_H