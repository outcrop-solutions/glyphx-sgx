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
#ifndef DATASOURCE_H
#define DATASOURCE_H

#include "GlyphEngine_Exports.h"
#include <boost/property_tree/ptree_fwd.hpp>
#include <string>
#include <unordered_set>
#include <boost/bimap.hpp>
#include "DataSourceTable.h"

namespace GlyphEngine {

	class GLYPHENGINE DataSource
	{
	public:
		typedef std::shared_ptr<DataSource> SharedPtr;
		typedef std::shared_ptr<const DataSource> ConstSharedPtr;

		typedef boost::property_tree::wptree PropertyTree;
		typedef std::unordered_map<std::wstring, DataSourceTable> Tables;
		typedef std::vector<std::wstring> TableNames;

		static const std::wstring SingleTableName;

		enum SourceType {
			File,
			DatabaseServer
		};

		typedef boost::bimap<SourceType, std::wstring> SourceTypeBimap;

		DataSource(const std::wstring& host = L"localhost", const std::wstring& username = L"", const std::wstring& password = L"");
		DataSource(const PropertyTree& propertyTree);
        DataSource(const DataSource& datasource);
        virtual ~DataSource();

        DataSource& operator=(const DataSource& datasource);
		bool operator==(const DataSource& datasource) const;
		bool operator!=(const DataSource& datasource) const;

		virtual SourceType GetSourceType() const = 0;

        const std::wstring& GetHost() const;
        const std::wstring& GetUsername() const;
        const std::wstring& GetPassword() const;
		const Tables& GetTables() const;

		const boost::property_tree::wptree& GetQuery(const std::wstring& table) const;
		void SetQuery(const std::wstring& table, const boost::property_tree::wptree& query);

		void AddTables(const Tables& tables);
		void AddTables(const TableNames& tableNames);
		void RemoveTable(const std::wstring& tableName);
		void ClearTables();
		TableNames GetTableNames() const;

		virtual bool IsOriginalDatasourceADatabase() { return false; };
		virtual bool CanDataSourceHaveMultipleTables() const = 0;
		virtual bool IsFile() const = 0;
		virtual bool CanDataSourceBeFound() const = 0;
		virtual std::wstring GetFormattedName() const = 0;
		virtual std::wstring GetDBName() const = 0;

		bool DoAnyTablesHaveQueries() const;

		virtual PropertyTree& ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree);

		static const SourceTypeBimap s_sourceTypeStrings;

    protected:
        std::wstring m_host;
        std::wstring m_username;
        std::wstring m_password;
		Tables m_tables;
	};

} 

#endif //DATASOURCE_H
