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

#ifndef FILEDATASOURCE_H
#define FILEDATASOURCE_H

#include "sgxdatamapping.h"
#include "datasource.h"

namespace SynGlyphX {

	class SGXDATAMAPPING_API FileDatasource : public Datasource
	{
	public:
		enum FileType {
			SQLITE3,
			CSV,
			KML,
			XML,
			JSON
		};

		typedef boost::bimap<FileType, std::wstring> FileTypeBimap;
		typedef boost::property_tree::wptree PropertyTree;

		typedef std::shared_ptr<FileDatasource> SharedPtr;
		typedef std::shared_ptr<const FileDatasource> ConstSharedPtr;

		FileDatasource(FileType type, const std::wstring& filename, const std::wstring& host = L"localhost", const std::wstring& username = L"", const std::wstring& password = L"");
		FileDatasource(const PropertyTree& propertyTree);
		FileDatasource(const FileDatasource& datasource);
		virtual ~FileDatasource();

		FileDatasource& operator=(const FileDatasource& datasource);
		bool operator==(const FileDatasource& datasource) const;
		bool operator!=(const FileDatasource& datasource) const;

		virtual SourceType GetSourceType() const;
		FileType GetFileType() const;

		const std::wstring& GetFilename() const;
		void ChangeFilename(const std::wstring& filename);

		bool RequiresConversionToDB() const;

		virtual bool IsOriginalDatasourceADatabase() const;
		virtual bool CanDatasourceHaveMultipleTables() const;
		virtual bool IsFile() const;
		virtual bool CanDatasourceBeFound() const;
		virtual std::wstring GetFormattedName() const;
		virtual std::wstring GetDBName() const;

		static bool CanFileTypeHaveMultipleTables(FileType fileType);

		virtual PropertyTree& ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree);

		static FileType GetFileTypeForFile(const std::wstring& filename);

		static const FileTypeBimap s_fileTypeStrings;
		static const FileTypeBimap s_fileTypePrefixes;

	private:
		static bool IsSQLite3DB(const std::wstring& filename);

		FileType m_fileType;
		std::wstring m_filename;
	};

} //namespace SynGlyphX

#endif //FILEDATASOURCE_H