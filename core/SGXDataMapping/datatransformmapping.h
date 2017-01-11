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
#ifndef SYNGLYPHX_DATATRANSFORMMAPPING_H
#define SYNGLYPHX_DATATRANSFORMMAPPING_H

#include "sgxdatamapping.h"
#include "xmlpropertytreefile.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <boost/property_tree/ptree_fwd.hpp>
#include "uuid.h"
#include "datamappingglyphgraph.h"
#include "datamappingdefaults.h"
#include "sceneproperties.h"
#include "datasource.h"
#include "filedatasource.h"
#include "databaseserverdatasource.h"
#include "FrontEndFilter.h"

namespace SynGlyphX {

	//Hash function for boost uuid so that it can be used in STL classes like unordered_map
	struct UUIDHash {
		std::size_t operator()( const boost::uuids::uuid& uuid ) const
		{
			return boost::uuids::hash_value( uuid );
		}
	};

	typedef std::unordered_set<boost::uuids::uuid, SynGlyphX::UUIDHash> UUIDUnorderedSet;

	class Link;
	class Legend;
	class BaseImage;
	class DataTransformMapping;

	class SGXDATAMAPPING_API InputFieldManager
	{
	public:
		InputFieldManager(DataTransformMapping* m) : m_dataTransformMapping(m) {}
		const InputField& GetInputField(const std::wstring& fieldID) const;
		void SetInputField(const std::wstring& fieldID, const InputField& field);
		void RemoveInputFieldAndBindings(const std::wstring& fieldID);
		void OnRemoveDataSource(const boost::uuids::uuid& id);
		std::wstring GenerateInputFieldID(const InputField& field); //may not be const with future implementation

		void Clear();


		void ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const;
		void ImportFromPropertyTree(const boost::property_tree::wptree& propertyTree);

		const std::unordered_map<std::wstring, InputField>& GetFieldMap() const;
	private:
		std::unordered_map<std::wstring, InputField> m_inputFields;
		DataTransformMapping* m_dataTransformMapping;
	};

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

		Datasource::SharedPtr GetDataSource(boost::uuids::uuid sourceId);


		bool HasDatasourceWithId(const boost::uuids::uuid& id) const;
		void RemoveDatasource(const boost::uuids::uuid& id);
		boost::uuids::uuid AddFileDatasource(const FileDatasource& datasource);
		boost::uuids::uuid AddDatabaseServer(const DatabaseServerDatasource& datasource);

		void EnableTables(const boost::uuids::uuid& id, const Datasource::TableNames& tables, bool enable = true);

		boost::uuids::uuid AddGlyphTree(const DataMappingGlyphGraph::SharedPtr glyphTree);
		void RemoveGlyphTree(const boost::uuids::uuid& id);
		const DataMappingGlyphGraphMap& GetGlyphGraphs() const;

		void SetGlyphGraphMap(const DataMappingGlyphGraphMap& glyphGraphs);

		DataMappingGlyphGraph GetSubgraph(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::ConstGlyphIterator& vertex, bool includeChildren);

		void AddChildGlyph(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::GlyphIterator& parent, const DataMappingGlyph& glyphTemplate, unsigned int numberOfChildren = 1);
		void AddChildTree(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::GlyphIterator& parent, const SynGlyphX::DataMappingGlyphGraph& glyphGraph);
		void AddChildTreeResetPosition(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::GlyphIterator parent, const SynGlyphX::DataMappingGlyphGraph& glyphGraph);
		void RemoveGlyph(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::ConstGlyphIterator& parent, int child);

		void UpdateGlyph(const boost::uuids::uuid& treeId, DataMappingGlyphGraph::GlyphIterator& vertex, const DataMappingGlyph& glyph);

		void SetInputField(const boost::uuids::uuid& treeID, DataMappingGlyphGraph::ConstGlyphIterator node, DataMappingGlyph::MappableField field, const std::wstring& inputfield);

		void ClearInputBinding(const boost::uuids::uuid& treeID, DataMappingGlyphGraph::ConstGlyphIterator& node, DataMappingGlyph::MappableField field);
		void ClearAllInputBindings(const boost::uuids::uuid& treeID, DataMappingGlyphGraph::ConstGlyphIterator& node);
		void ClearInputFieldBindings(const boost::uuids::uuid& treeID, const std::wstring& inputfield);
		void ClearInputFieldBindings(const std::wstring& inputfield);
		
		const InputTable& GetInputTable(const boost::uuids::uuid& treeId) const;
		const InputTable& GetInputTable(const DataMappingGlyphGraph& graph) const;

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

		ConstSharedPtr CreateSubsetMapping(const std::unordered_map<InputTable, std::wstring, InputTableHash>& inputTableToFileMap) const;

		void AddLegend(const Legend& legend);
		void RemoveLegend(unsigned int index);
		void SetLegend(unsigned int index, const Legend& legend);
		const std::vector<Legend>& GetLegends() const;
		void SetLegends(const std::vector<Legend>& legends);

		void AddLink(const Link& link);
		void RemoveLink(unsigned int index);
		void InsertLink(unsigned int index, const Link& link);
		void SetLink(unsigned int index, const Link& link);
		const std::vector<Link>& GetLinks() const { return m_links; }
		void SetLinks(const std::vector<Link>& links); 

		InputFieldManager* GetInputFieldManager() { return &m_inputFieldManager; }
		std::unordered_map<std::wstring, std::wstring> GetFieldToAliasMapForTable(const InputTable& table) const;

		const MultiTableFrontEndFilters& GetFrontEndFilters() const { return m_frontEndFilters; }
		void SetFrontEndFilters(const MultiTableFrontEndFilters& filters) { m_frontEndFilters = filters; }

		std::wstring GetFormattedName(const boost::uuids::uuid& datasourceID, const std::wstring& table) const;

		std::map<std::wstring, std::vector<std::wstring>> GetElasticListFields() const { return m_elasticListMap; }
		void SaveElasticListFields(std::map<std::wstring, std::vector<std::wstring>> elasticListMap) { m_elasticListMap = elasticListMap; }

    protected:
		void CopyInputBindingsForSubsetMapping(DataMappingGlyphGraph::SharedPtr newGlyphGraph, 
											   DataMappingGlyphGraph::GlyphIterator newNode, 
											   const std::unordered_map<std::wstring, std::wstring>& oldToNewIDMap) const;
		void Clear(bool addADefaultBaseObjectAfterClear);

		void AddGlyphTree(const boost::uuids::uuid& id, const DataMappingGlyphGraph::SharedPtr glyphTree);

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
		std::vector<Link> m_links;
		InputFieldManager m_inputFieldManager;
		MultiTableFrontEndFilters m_frontEndFilters;
		std::map<std::wstring, std::vector<std::wstring>> m_elasticListMap;
		boost::uuids::uuid m_id;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATATRANSFORMMAPPING_H
