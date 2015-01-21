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

#include "sgxdatatransform_global.h"
#include "datasource.h"

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT FileDatasource : public Datasource
	{
	public:
		enum SourceType {
			SQLITE3,
			CSV,
			KML,
		};

		typedef boost::bimap<SourceType, std::wstring> SourceTypeBimap;
		typedef boost::property_tree::wptree PropertyTree;

		FileDatasource(SourceType type, const std::wstring& filename, const std::wstring& host = L"localhost", unsigned int port = 0, const std::wstring& username = L"", const std::wstring& password = L"");
		FileDatasource(const PropertyTree& propertyTree);
		FileDatasource(const FileDatasource& datasource);
		virtual ~FileDatasource();

		FileDatasource& operator=(const FileDatasource& datasource);
		bool operator==(const FileDatasource& datasource) const;
		bool operator!=(const FileDatasource& datasource) const;

		SourceType GetType() const;

		const std::wstring& GetFilename() const;
		void ChangeFilename(const std::wstring& filename);

		bool RequiresConversionToDB() const;

		virtual bool IsOriginalDatasourceADatabase() const;
		virtual bool CanDatasourceHaveMultipleTables() const;
		virtual bool IsFile() const;
		virtual bool CanDatasourceBeFound() const;
		virtual const std::wstring& GetFormattedName() const;

		virtual PropertyTree& ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree);

		static const SourceTypeBimap s_sourceTypeStrings;

	private:
		SourceType m_type;
		std::wstring m_formattedName;
	};

} //namespace SynGlyphX

#endif //FILEDATASOURCE_H