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

#ifndef SYNGLYPHX_DATATRANSFORMMAPPING_H
#define SYNGLYPHX_DATATRANSFORMMAPPING_H

#include "sgxdatatransform_global.h"
#include "xmlpropertytreefile.h"
#include <string>
#include "datasourcemaps.h"
#include <unordered_map>
#include <boost/property_tree/ptree.hpp>
#include "UUID.h"
#include "datamappingglyphgraph.h"
#include "baseimage.h"
#include "transformer.h"
#include "datamappingdefaults.h"

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT DataTransformMapping : public XMLPropertyTreeFile, boost::noncopyable
    {
    public:
		typedef std::unordered_map<boost::uuids::uuid, DataMappingGlyphGraph::SharedPtr, SynGlyphX::UUIDHash> DataMappingGlyphGraphMap;

		typedef boost::shared_ptr<DataTransformMapping> SharedPtr;
		typedef boost::shared_ptr<const DataTransformMapping> ConstSharedPtr;

		DataTransformMapping();
		~DataTransformMapping();

		bool operator==(const DataTransformMapping& mapping) const;
		bool operator!=(const DataTransformMapping& mapping) const;

		const DatasourceMaps& GetDatasources() const;

		void RemoveDatasource(const boost::uuids::uuid& id);
		boost::uuids::uuid AddFileDatasource(FileDatasource::SourceType type, 
			const std::wstring& name,
            const std::wstring& host = L"localhost",
            unsigned int port = 0,
            const std::wstring& username = L"",
            const std::wstring& password = L"");

		void EnableTables(const boost::uuids::uuid& id, const Datasource::TableSet& tables, bool enable = true);

		boost::uuids::uuid AddGlyphTree(const DataMappingGlyphGraph::SharedPtr glyphTree);
		void RemoveGlyphTree(const boost::uuids::uuid& id);
		const DataMappingGlyphGraphMap& GetGlyphGraphs() const;

		void AddChildGlyph(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::iterator& parent, const DataMappingGlyph& glyphTemplate, unsigned int numberOfChildren = 1);
		void AddChildTree(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::iterator& parent, const stlplus::ntree<SynGlyphX::DataMappingGlyph>& glyphGraph);
		void RemoveGlyph(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::const_iterator& parent, int child);

		void SetInputField(const boost::uuids::uuid& treeID, DataMappingGlyphGraph::const_iterator& node, DataMappingGlyph::MappableField field, const InputField& inputfield);
		void ClearInputBinding(const boost::uuids::uuid& treeID, DataMappingGlyphGraph::const_iterator& node, DataMappingGlyph::MappableField field);

		bool IsTransformable() const;

		void Clear();

		void AddBaseObject(const BaseImage& baseObject);
		void RemoveBaseObject(unsigned int index);
		void SetBaseObject(unsigned int index, const BaseImage& baseObject);
		const std::vector<BaseImage>& GetBaseObjects() const;

		const boost::uuids::uuid& GetID() const;
		void ResetID();

		void UpdateDatasourceName(const boost::uuids::uuid& id, const std::wstring& name);

		const DataMappingDefaults& GetDefaults() const;
		void SetDefaults(const DataMappingDefaults& defaults);

    protected:
		void Clear(bool addADefaultBaseObjectAfterClear);
		virtual void ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree);
		virtual void ExportToPropertyTree(boost::property_tree::wptree& filePropertyTree) const;

		DataMappingDefaults m_defaults;
		DatasourceMaps m_datasources;
		DataMappingGlyphGraphMap m_glyphTrees;
		std::vector<BaseImage> m_baseObjects;
		boost::uuids::uuid m_id;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATATRANSFORMMAPPING_H