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
#ifndef USERACCESSCONTROLS_H
#define USERACCESSCONTROLS_H

//#include <jni.h>
#include "DataEngine_Exports.h"
#include <QtCore/QString>
#include <QtCore/QStringList>

namespace DataEngine
{
	class DATAENGINE UserAccessControls
	{

	private:
		JNIEnv *jniEnv;
		jclass jcls;

		bool validConnection;
		bool synced;
		int valid;
		QString presetLogoPath;
		QString presetId;
		QString presetName;
		QString presetInstitution;
		QStringList groupNames;
		QStringList vizNames;
		QString appVersion;

	public:
		UserAccessControls(JNIEnv *env);
		~UserAccessControls(){};

		bool IsValidConnection();
		void ResetConnection();
		void FlushOutFilterSetup();
		int GetUserID();
		int GetLicenseType();
		QString NameOfUser();
		QString NameOfInstitution();
		QString NameOfDirectory();
		QString LastModified();
		QStringList VizualizationNames();
		QStringList GetFormattedGroupNames();
		void SetChosenGroup(QString name);
		void SetAppVersionNumber(QString version){ appVersion = version; }

		void InitializeConnection();
		int ValidateCredentials(QString username, QString password);
		bool GenerateLicenseKey(QString path);
		int CheckAvailableGroups();
		bool FileSyncSetup(QString path);
		int VisualizationsToSync();
		void StartSyncingFiles();
		int GetSyncProgress();
		bool IsDoneSyncing();

		int FilesSynced();
		void PresetLogoPath(QString path);
		void SetVisualizationGroupNames(QStringList groups);
		void SetVisualizationNames(QStringList vizs);
		void SetUsersInformation(QString id, QString name, QString inst);
		QString GlyphEdPath();
		bool HasSynced();


	};
}
#endif // USERACCESSCONTROLS_H
