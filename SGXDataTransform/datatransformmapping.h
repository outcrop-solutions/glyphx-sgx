#ifndef SYNGLYPHX_DATATRANSFORMMAPPING_H
#define SYNGLYPHX_DATATRANSFORMMAPPING_H

#include "sgxdatatransform_global.h"
#include "xmlpropertytreefile.h"
#include <string>
#include "datasourcemaps.h"
#include <unordered_map>
#include <boost/property_tree/ptree.hpp>
#include "UUID.h"
#include "minmaxglyphtree.h"
#include "baseimage.h"
#include "transformer.h"

namespace SynGlyphX {

	class SGXDATATRANSFORM_EXPORT DataTransformMapping : public XMLPropertyTreeFile
    {
    public:
		typedef std::unordered_map<boost::uuids::uuid, MinMaxGlyphTree::SharedPtr, SynGlyphX::UUIDHash> MinMaxGlyphTreeMap;

		typedef boost::shared_ptr<DataTransformMapping> SharedPtr;
		typedef boost::shared_ptr<const DataTransformMapping> ConstSharedPtr;

		DataTransformMapping();
		DataTransformMapping(const GlyphTree& glyphTree);
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

		boost::uuids::uuid AddGlyphTree(const MinMaxGlyphTree::SharedPtr glyphTree);
		void RemoveGlyphTree(const boost::uuids::uuid& id);
		const MinMaxGlyphTreeMap& GetGlyphTrees() const;

		void SetInputField(const boost::uuids::uuid& treeID, MinMaxGlyphTree::const_iterator& node, int index, const InputField& inputfield);
		void ClearInputBinding(const boost::uuids::uuid& treeID, MinMaxGlyphTree::const_iterator& node, int index);

		bool IsTransformable() const;

		void Clear();

		void SetBaseImage(const BaseImage& baseImage);
		const BaseImage& GetBaseImage() const;

		const boost::uuids::uuid& GetID() const;

		void UpdateDatasourceName(const boost::uuids::uuid& id, const std::wstring& name);

    protected:
		virtual void ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree);
		virtual void ExportToPropertyTree(boost::property_tree::wptree& filePropertyTree) const;

		DatasourceMaps m_datasources;
		MinMaxGlyphTreeMap m_glyphTrees;
		BaseImage m_baseImage;
		boost::uuids::uuid m_id;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATATRANSFORMMAPPING_H