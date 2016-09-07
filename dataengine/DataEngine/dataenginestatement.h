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
#ifndef DATAENGINESTATEMENT_H
#define DATAENGINESTATEMENT_H

#include <iostream>
#include <jni.h>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include "filedatasource.h"
#include "DataEngine_Exports.h"

namespace DataEngine
{
	class DATAENGINE DataEngineStatement {

	private:
		void constructDataStats(int i);
		QStringList getStatsForField(int i, QString field);
		void setSourceType(SynGlyphX::FileDatasource::FileType type);
		JNIEnv *jniEnv;
		jclass jcls;
		QStringList fields;
		QMap<QString, QStringList> dataStats;
		int current;
		QString sourceType;

	public:
		DataEngineStatement(){};
		void prepare(JNIEnv *jniEnv, jclass jcls);
		void prepare(JNIEnv *jniEnv, jclass jcls, const QString& sourceTypeString);
		std::string getTableName(int i);
		std::string checkMethodID(jmethodID methodId, int i);
		int size();
		int tableCount();
		//JDBC FUNCTIONS
		void getFieldsForTable(int i);
		bool hasNext();
		QString getField();
		QString getType();
		QString getMin();
		QString getMax();
		QString getAverage();
		QString getCount();
		QString getDistinct();
		//END JDBC
		~DataEngineStatement(){};

	};
}
#endif // DATAENGINESTATEMENT_H
