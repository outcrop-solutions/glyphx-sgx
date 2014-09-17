#ifndef SYNGLYPHX_DATATRANSFORMMAPPING_H
#define SYNGLYPHX_DATATRANSFORMMAPPING_H

#include "sgxdatatransform_global.h"
#include <string>
#include "datasource.h"
#include <unordered_map>
#include <boost/property_tree/ptree.hpp>
#include "UUID.h"
#include <boost/uuid/uuid_generators.hpp>
#include "minmaxglyphtree.h"
#include "baseimage.h"
#include "geographicboundingbox.h"
#include "transformer.h"

namespace SynGlyphX {

    class SGXDATATRANSFORM_EXPORT DataTransformMapping
    {
    public:
		typedef std::unordered_map<boost::uuids::uuid, Datasource, SynGlyphX::UUIDHash> DatasourceMap;
		typedef std::unordered_map<boost::uuids::uuid, MinMaxGlyphTree::SharedPtr, SynGlyphX::UUIDHash> MinMaxGlyphTreeMap;

		typedef boost::shared_ptr<DataTransformMapping> SharedPtr;
		typedef boost::shared_ptr<const DataTransformMapping> ConstSharedPtr;

		DataTransformMapping();
		DataTransformMapping(const GlyphTree& glyphTree);
		~DataTransformMapping();

        void ReadFromFile(const std::string& filename);
        void WriteToFile(const std::string& filename, bool resetID);

		const DatasourceMap& GetDatasources() const;

		boost::uuids::uuid AddDatasource(const std::wstring& name,
			Datasource::SourceType type,
            const std::wstring& host = L"localhost",
            unsigned int port = 0,
            const std::wstring& username = L"",
            const std::wstring& password = L"");

		void AddTables(const boost::uuids::uuid& id, const std::vector<std::wstring>& tables);

		boost::uuids::uuid AddGlyphTree(const MinMaxGlyphTree::SharedPtr glyphTree);
		const MinMaxGlyphTreeMap& GetGlyphTrees() const;

		void SetInputField(const boost::uuids::uuid& treeID, MinMaxGlyphTree::const_iterator& node, int index, const InputField& inputfield);
		void ClearInputBinding(const boost::uuids::uuid& treeID, MinMaxGlyphTree::const_iterator& node, int index);

		bool IsTransformable() const;

		void Clear();

		void SetBaseImage(const BaseImage& baseImage);
		const BaseImage& GetBaseImage() const;

		void SetPositionXYMinMaxToGeographicForAllGlyphTrees(const GeographicBoundingBox& boundingBox);
		void GetPositionXYForAllGlyphTrees(std::vector<GeographicPoint>& points) const;

		const boost::uuids::uuid& GetID() const;
		const unsigned long GetVersion() const;

    private:
		void GetMinMax(InputBinding& binding, const InputField& inputField) const;

		static boost::uuids::random_generator s_uuidGenerator;

		DatasourceMap m_datasources;
		MinMaxGlyphTreeMap m_glyphTrees;
		BaseImage m_baseImage;
		boost::uuids::uuid m_id;
		bool m_updated;
		unsigned long m_version;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATATRANSFORMMAPPING_H