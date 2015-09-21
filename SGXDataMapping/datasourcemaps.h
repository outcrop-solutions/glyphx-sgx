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

#ifndef SYNGLYPHX_DATASOURCEMAPS_H
#define SYNGLYPHX_DATASOURCEMAPS_H

#include "sgxdatamapping.h"
#include <string>
#include <unordered_map>
#include <boost/property_tree/ptree.hpp>
#include "UUID.h"
#include "filedatasource.h"
#include "inputtable.h"

namespace SynGlyphX {

	class SGXDATAMAPPING_API DatasourceMaps
	{
	public:
		typedef boost::property_tree::wptree PropertyTree;
		typedef std::unordered_map<boost::uuids::uuid, FileDatasource, SynGlyphX::UUIDHash> FileDatasourceMap;

		DatasourceMaps();
		DatasourceMaps(const PropertyTree& propertyTree);
		~DatasourceMaps();

		bool operator==(const DatasourceMaps& maps) const;
		bool operator!=(const DatasourceMaps& maps) const;

		const Datasource& GetDatasourceByID(const boost::uuids::uuid& id) const;
		bool HasDatasources() const;
		unsigned int Count() const;
		void Clear();
		bool EnableTables(const boost::uuids::uuid& id, const Datasource::TableNames& tables, bool enable = true);

		void ChangeDatasourceName(const boost::uuids::uuid& id, const std::wstring& name);

		PropertyTree& ExportToPropertyTree(PropertyTree& propertyTreeParent) const;

		void RemoveDatasource(const boost::uuids::uuid& id);

		const FileDatasourceMap& GetFileDatasources() const;
		boost::uuids::uuid AddFileDatasource(FileDatasource::SourceType type,
			const std::wstring& name,
			const std::wstring& host = L"localhost",
			unsigned int port = 0,
			const std::wstring& username = L"",
			const std::wstring& password = L"");
		void AddFileDatasource(const boost::uuids::uuid& id, const FileDatasource& fileDatasource);

	private:
		FileDatasourceMap m_fileDatasources;
	};

} //namespace SynGlyphX

#endif //DATASOURCEMAPS_H