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

#include "sgxdatamapping.h"
#include "xmlpropertytreefile.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <boost/property_tree/ptree.hpp>
#include "uuid.h"
#include "datamappingglyphgraph.h"
#include "baseimage.h"
#include "datamappingdefaults.h"
#include "sceneproperties.h"
#include "datasource.h"
#include "filedatasource.h"
#include "databaseserverdatasource.h"
#include "legend.h"

namespace SynGlyphX {

	class SGXDATAMAPPING_API DataTransformMapping : public XMLPropertyTreeFile
    {
    public:
		typedef std::unordered_map<boost::uuids::uuid, DataMappingGlyphGraph::SharedPtr, SynGlyphX::UUIDHash> DataMappingGlyphGraphMap;
		typedef std::unordered_map<boost::uuids::uuid, Datasource::SharedPtr, SynGlyphX::UUIDHash> DatasourceMap;

		typedef std::wstring FieldGroupName;
		typedef std::unordered_map<FieldGroupName, FieldGroup> FieldGroupMap;

		typedef std::shared_ptr<DataTransformMapping> SharedPtr;
		typedef std::shared_ptr<const DataTransformMapping> ConstSharedPtr;

		DataTransformMapping();
		DataTransformMapping(const DataTransformMapping& mapping);
		~DataTransformMapping();

		bool operator==(const DataTransformMapping& mapping) const;
		bool operator!=(const DataTransformMapping& mapping) const;

		const DatasourceMap& GetDatasources() const;
		DatasourceMap GetDatasourcesInUse() const;
		UUIDUnorderedSet GetDatasourcesBySourceType(Datasource::SourceType type) const;

		bool HasDatasourceWithId(const boost::uuids::uuid& id) const;
		void RemoveDatasource(const boost::uuids::uuid& id);
		boost::uuids::uuid AddFileDatasource(const FileDatasource& datasource);
		boost::uuids::uuid AddDatabaseServer(const DatabaseServerDatasource& datasource);

		void EnableTables(const boost::uuids::uuid& id, const Datasource::TableNames& tables, bool enable = true);

		boost::uuids::uuid AddGlyphTree(const DataMappingGlyphGraph::SharedPtr glyphTree);
		void RemoveGlyphTree(const boost::uuids::uuid& id);
		const DataMappingGlyphGraphMap& GetGlyphGraphs() const;
		DataMappingGlyphGraph GetSubgraph(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::ConstGlyphIterator& vertex, bool includeChildren);

		void AddChildGlyph(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::GlyphIterator& parent, const DataMappingGlyph& glyphTemplate, unsigned int numberOfChildren = 1);
		void AddChildTree(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::GlyphIterator& parent, const SynGlyphX::DataMappingGlyphGraph& glyphGraph);
		void AddChildTreeResetPosition(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::GlyphIterator& parent, const SynGlyphX::DataMappingGlyphGraph& glyphGraph);
		void RemoveGlyph(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::ConstGlyphIterator& parent, int child);

		void UpdateGlyph(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::GlyphIterator& vertex, const DataMappingGlyph& glyph);

		void SetInputField(const boost::uuids::uuid& treeID, DataMappingGlyphGraph::ConstGlyphIterator& node, DataMappingGlyph::MappableField field, const InputField& inputfield);
		void ClearInputBinding(const boost::uuids::uuid& treeID, DataMappingGlyphGraph::ConstGlyphIterator& node, DataMappingGlyph::MappableField field);
		void ClearAllInputBindings(const boost::uuids::uuid& treeID, DataMappingGlyphGraph::ConstGlyphIterator& node);

		bool IsTransformable() const;
		bool DoesAtLeastOneGlyphGraphHaveBindingsOnPosition() const;

		void Clear();

		void AddBaseObject(const BaseImage& baseObject);
		void RemoveBaseObject(unsigned int index);
		void SetBaseObject(unsigned int index, const BaseImage& baseObject);
		const std::vector<BaseImage>& GetBaseObjects() const;

		const boost::uuids::uuid& GetID() const;
		void ResetID();

		void UpdateDatasourceName(const boost::uuids::uuid& id, const std::wstring& name);

		const FieldGroupMap& GetFieldGroupMap() const;
		void UpdateFieldGroup(const FieldGroupName& groupName, const FieldGroup& fieldGroup);
		void RemoveFieldGroup(const FieldGroupName& groupName);
		bool DoesFieldGroupHaveFieldsFromDatasource(const FieldGroupName& groupName, const boost::uuids::uuid& datasourceId) const;

		const DataMappingDefaults& GetDefaults() const;
		void SetDefaults(const DataMappingDefaults& defaults);

		const SceneProperties& GetSceneProperties() const;
		void SetSceneProperties(const SceneProperties& sceneProperties);

		std::vector<boost::uuids::uuid> GetFileDatasourcesWithInvalidFiles(bool onlyUseDatasourcesInUse) const;
		std::vector<unsigned int> GetFileBaseObjectsWithInvalidFiles() const;
		std::vector<unsigned int> GetLegendsWithInvalidFiles() const;

		ConstSharedPtr CreateSubsetMappingWithSingleTable(const InputTable& inputTable, const std::wstring& csvFilename) const;

		void AddLegend(const Legend& legend);
		void RemoveLegend(unsigned int index);
		void SetLegend(unsigned int index, const Legend& legend);
		const std::vector<Legend>& GetLegends() const;
		void SetLegends(const std::vector<Legend>& legends);

    protected:
		void CopyInputBindingsForSubsetMapping(DataMappingGlyphGraph::SharedPtr newGlyphGraph, 
											   DataMappingGlyphGraph::GlyphIterator& newNode, 
											   DataMappingGlyphGraph::ConstSharedPtr oldGlyphGraph, 
											   DataMappingGlyphGraph::ConstGlyphIterator& oldNode,
											   const boost::uuids::uuid& datasourceID) const;
		void Clear(bool addADefaultBaseObjectAfterClear);

		void ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree) override;
		void ExportToPropertyTree(boost::property_tree::wptree& filePropertyTree) const override;
		bool IsDifferentFromGivenPropertyTree(const boost::property_tree::wptree& originalPropertyTree) const override;

		SceneProperties m_sceneProperties;
		DataMappingDefaults m_defaults;
		DatasourceMap m_datasources;
		DataMappingGlyphGraphMap m_glyphTrees;
		std::vector<BaseImage> m_baseObjects;
		FieldGroupMap m_fieldGroups;
		std::vector<Legend> m_legends;

		boost::uuids::uuid m_id;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATATRANSFORMMAPPING_H