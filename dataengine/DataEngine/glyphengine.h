#pragma once
#ifndef DATAENGINE_H
#define DATAENGINE_H

#include <jni.h>
#include <iostream>
#include <boost/filesystem.hpp>
#include "georeference.h"
#include <QtCore/QString>
#include <QtWidgets/QWidget>
#include "baseimage.h"
#include "DataEngine_Exports.h"

namespace DataEngine
{
	class DATAENGINE GlyphEngine {

	private:
		JNIEnv *jniEnv;
		jclass jcls;
		std::string sdtFile;
		std::string baseOutputDir;
		std::string baseImageDir;
		std::string baseFilename;
		std::string application;
		const unsigned int NumberOfDefaultBaseImages = 1;
		bool downloadComplete = true;
		std::vector<std::string> images;
		void prepare();
		void createCacheDirectory();
		void copyBaseImages();
		bool downloadBaseImage(const SynGlyphX::BaseImage& baseImage, QString baseImageFilename);
		void setGeoBoundingBox(std::vector<double> nw, std::vector<double> se, std::vector<double> size);
		bool hasImageBeenUpdated();
		std::vector<double> getNWandSE();

	public:
		GlyphEngine(){};
		void initiate(JNIEnv *env, std::string sdtPath, std::string outDir, std::string bid, std::string bfn,std::string appName);
		bool getDownloadedBaseImage(std::vector<SynGlyphX::BaseImage> baseImages);
		std::vector<std::string> getBaseImages();
		void generateGlyphs(QWidget *mainWindow);
		bool IsUpdateNeeded() const;
		QString JavaErrors();
		void ClearJavaErrors();
		QStringList DistinctValuesForField(QString id, QString table, QString field);
		void SetQueryForDatasource(QString id, QString table, QString query);
		int SizeOfQuery(QString id, QString table, QString query);
		~GlyphEngine(){};

	};
}
#endif // DATAENGINE_H