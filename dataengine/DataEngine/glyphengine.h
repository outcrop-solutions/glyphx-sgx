///
/// SynGlyphX Holdings Incorporated ("COMPANY") CONFIDENTIAL
/// Copyright (c) 2013-2015 SynGlyphX Holdings Incorporated, All Rights Reserved.
///
/// NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
/// herein are proprietary to COMPANY and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
/// Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
/// from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed 
/// Confidentiality and Non-disclosure agreements explicitly covering such access.
///
/// The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes  
/// information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
/// OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE 
/// LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  
/// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
///

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
		bool jniSet;
		std::string sdtFile;
		std::string baseOutputDir;
		std::string baseImageDir;
		std::string baseFilename;
		std::string application;
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
		GlyphEngine():jniSet(false){};
		void initiate(JNIEnv *env, std::string sdtPath, std::string outDir, std::string bid, std::string bfn, std::string appName, bool run = true);
		void AddVisualization(JNIEnv *env, std::string sdtPath);
		bool getDownloadedBaseImage(std::vector<SynGlyphX::BaseImage> baseImages);
		std::vector<std::string> getBaseImages();
		std::vector<std::string> getCompassValues();
		void generateGlyphs(QWidget *mainWindow);
		bool IsUpdateNeeded() const;
		QString JavaErrors();
		void ClearJavaErrors();
		QStringList DistinctValuesForField(QString id, QString table, QString field);
		void SetQueryForDatasource(QString id, QString table, QString query);
		int SizeOfQuery(QString id, QString table, QString query);
		bool DownloadFiles(QString bucket_name, QString file_name, QString location);
		~GlyphEngine(){};

	};
}
#endif // DATAENGINE_H
