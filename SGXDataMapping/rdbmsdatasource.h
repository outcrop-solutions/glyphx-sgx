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

#ifndef SYNGLYPHX_RDBMSDATASOURCE_H
#define SYNGLYPHX_RDBMSDATASOURCE_H

#include "datasource.h"

namespace SynGlyphX {

	class RDBMSDatasource : public Datasource
	{
	public:
		enum DBType {
			MySQL,
			Vertica
		};

		typedef boost::bimap<DBType, std::wstring> DBTypeBimap;
		typedef boost::property_tree::wptree PropertyTree;

		typedef std::shared_ptr<RDBMSDatasource> SharedPtr;
		typedef std::shared_ptr<const RDBMSDatasource> ConstSharedPtr;

		RDBMSDatasource(DBType type, const std::wstring& connection, const std::wstring& schema, const std::wstring& username = L"", const std::wstring& password = L"");
		RDBMSDatasource(const PropertyTree& propertyTree);
		RDBMSDatasource(const RDBMSDatasource& datasource);
		virtual ~RDBMSDatasource();

		RDBMSDatasource& operator=(const RDBMSDatasource& datasource);
		bool operator==(const RDBMSDatasource& datasource) const;
		bool operator!=(const RDBMSDatasource& datasource) const;

		virtual SourceType GetSourceType() const;
		DBType GetDBType() const;
		const std::wstring& GetSchema() const;

		virtual bool IsOriginalDatasourceADatabase() const;
		virtual bool CanDatasourceHaveMultipleTables() const;
		virtual bool IsFile() const;
		virtual bool CanDatasourceBeFound() const;
		virtual std::wstring GetFormattedName() const;
		virtual std::wstring GetDBName() const;

		virtual PropertyTree& ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree);

		static const DBTypeBimap s_dbTypeStrings;

	protected:
		DBType m_dbType;
		std::wstring m_schema;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_RDBMSDATASOURCE_H