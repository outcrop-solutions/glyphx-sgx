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
#ifndef S3FILEMANAGER_H
#define S3FILEMANAGER_H

#include "DataEngine_Exports.h"
#include <string>
#include <vector>
#include <QtCore/qdatetime.h>

namespace Aws
{
	namespace S3
	{
		class S3Client;

		namespace Model
		{
			class Object;
		}
	}
}

namespace DataEngine
{

	class DATAENGINE S3File
	{
	public:
		S3File(std::string bucket, Aws::S3::Model::Object obj);
		~S3File();

		std::string GetName(){ return name; }
		std::string GetTag(){ return tag; }
		long long GetSize(){ return size; }
		double GetTimestamp(){ return timestamp; }
		std::string GetUrl(){ return url; }

		friend bool operator<(S3File& a, S3File& b) { return a.GetTimestamp() > b.GetTimestamp(); }

	private:
		std::string name;
		std::string tag;
		long long size;
		double timestamp;
		std::string url;
	};

	class DATAENGINE S3FileManager
	{
	public:
		S3FileManager();
		~S3FileManager();

		std::vector<S3File*> GetFilesFromDirectory(std::string bucket, const char* directory);

	private:
		Aws::S3::S3Client* s3Client;

	};

}
#endif // S3FILEMANAGER_H