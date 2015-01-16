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