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
#ifndef RDBMSDATASOURCE_H
#define RDBMSDATASOURCE_H

#include "GlyphEngine_Exports.h"
#include "DataSource.h"

namespace GlyphEngine {

	class GLYPHENGINE DatabaseServerDataSource : public DataSource
	{
	public:
		enum DBType {
			MySQL,
			Vertica
		};

		typedef boost::bimap<DBType, std::wstring> DBTypeBimap;
		typedef boost::property_tree::wptree PropertyTree;

		typedef std::shared_ptr<DatabaseServerDataSource> SharedPtr;
		typedef std::shared_ptr<const DatabaseServerDataSource> ConstSharedPtr;

		DatabaseServerDataSource(DBType type, const std::wstring& connection, const std::wstring& schema, const std::wstring& username = L"", const std::wstring& password = L"");
		DatabaseServerDataSource(const PropertyTree& propertyTree);
		DatabaseServerDataSource(const DatabaseServerDataSource& datasource);
		virtual ~DatabaseServerDataSource();

		DatabaseServerDataSource& operator=(const DatabaseServerDataSource& datasource);
		bool operator==(const DatabaseServerDataSource& datasource) const;
		bool operator!=(const DatabaseServerDataSource& datasource) const;

		virtual SourceType GetSourceType() const;
		DBType GetDBType() const;
		const std::wstring& GetSchema() const;

		virtual bool IsOriginalDataSourceADatabase() const;
		virtual bool CanDataSourceHaveMultipleTables() const;
		virtual bool IsFile() const;
		virtual bool CanDataSourceBeFound() const;
		virtual std::wstring GetFormattedName() const;
		virtual std::wstring GetDBName() const;

		std::wstring GetFullJDBCConnectionString() const;

		virtual PropertyTree& ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree);

		static const DBTypeBimap s_dbTypeStrings;
		static const DBTypeBimap s_dbTypePrefixes;
		static const std::wstring s_prefixSeparator;

	protected:
		DBType m_dbType;
		std::wstring m_schema;
	};

} 

#endif //RDBMSDATASOURCE_H
