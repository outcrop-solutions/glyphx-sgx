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
#ifndef DATASOURCETABLE_H
#define DATASOURCETABLE_H

#include "GlyphEngine_Exports.h"
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <boost/property_tree/ptree.hpp>

namespace GlyphEngine {

	class GLYPHENGINE DataSourceTable
	{
	public:
		DataSourceTable(const std::wstring& name, const boost::property_tree::wptree& query = boost::property_tree::wptree());
		DataSourceTable(const boost::property_tree::wptree& propertyTree);
		DataSourceTable(const DataSourceTable& table);
		DataSourceTable()
		{

		}
		~DataSourceTable();

		DataSourceTable& operator=(const DataSourceTable& table);
		bool operator==(const DataSourceTable& table) const;
		bool operator!=(const DataSourceTable& table) const;

		const std::wstring& GetName() const;
		const boost::property_tree::wptree& GetQuery() const;
		void  SetQuery(const boost::property_tree::wptree& query);

		boost::property_tree::wptree& ExportToPropertyTree(boost::property_tree::wptree& parentPropertyTree) const;

	private:
		std::wstring m_name;
		boost::property_tree::wptree m_query;
	};

} 

#endif //DataSourceTABLE_H