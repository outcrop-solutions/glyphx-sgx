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

#ifndef SYNGLYPHX_DATASOURCE
#define SYNGLYPHX_DATASOURCE

#include "sgxdatamapping.h"
#include <boost/property_tree/ptree.hpp>
#include <string>
#include <unordered_set>
#include <boost/bimap.hpp>
#include "datasourcetable.h"

namespace SynGlyphX {

	class SGXDATAMAPPING_API Datasource
	{
	public:
		typedef boost::property_tree::wptree PropertyTree;
		typedef std::unordered_map<std::wstring, DatasourceTable> Tables;
		typedef std::unordered_set<std::wstring> TableNames;

		static const std::wstring SingleTableName;

		Datasource(const std::wstring& dbName, const std::wstring& host = L"localhost", unsigned int port = 0, const std::wstring& username = L"", const std::wstring& password = L"");
		Datasource(const PropertyTree& propertyTree);
        Datasource(const Datasource& datasource);
        virtual ~Datasource();

        Datasource& operator=(const Datasource& datasource);
		bool operator==(const Datasource& datasource) const;
		bool operator!=(const Datasource& datasource) const;

        const std::wstring& GetDBName() const;
        const std::wstring& GetHost() const;
        unsigned int GetPort() const;
        const std::wstring& GetUsername() const;
        const std::wstring& GetPassword() const;
		const Tables& GetTables() const;

		void AddTables(const Tables& tables);
		void AddTables(const TableNames& tableNames);
		void RemoveTable(const std::wstring& tableName);
		void ClearTables();
		TableNames GetTableNames() const;

		const DatasourceTable::FieldGroupMap& GetFieldGroupMap(const std::wstring& tableName) const;
		void UpdateFieldGroup(const std::wstring& tableName, const DatasourceTable::FieldGroupName& groupName, const DatasourceTable::FieldGroup& fieldGroup);
		void RemoveFieldGroup(const std::wstring& tableName, const DatasourceTable::FieldGroupName& groupName);

		virtual bool IsOriginalDatasourceADatabase() const = 0;
		virtual bool CanDatasourceHaveMultipleTables() const = 0;
		virtual bool IsFile() const = 0;
		virtual bool CanDatasourceBeFound() const = 0;
		virtual const std::wstring& GetFormattedName() const = 0;

		virtual PropertyTree& ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree, const std::wstring& parentName);

    protected:
        std::wstring m_dbName;
        std::wstring m_host;
        unsigned int m_port;
        std::wstring m_username;
        std::wstring m_password;
		Tables m_tables;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATASOURCE
