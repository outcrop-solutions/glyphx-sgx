#ifndef SYNGLYPHX_DATATRANSFORM
#define SYNGLYPHX_DATATRANSFORM

#include "sgxdatatransform_global.h"
#include <string>
#include "datasource.h"
#include <unordered_map>
#include <boost/property_tree/ptree.hpp>
#include "UUID.h"
#include <boost/uuid/uuid_generators.hpp>
#include "minmaxglyphtree.h"

namespace SynGlyphX {

    class SGXDATATRANSFORM_EXPORT DataTransform
    {
    public:
		typedef std::unordered_map<boost::uuids::uuid, Datasource, SynGlyphX::UUIDHash> DatasourceMap;
		typedef std::unordered_map<boost::uuids::uuid, MinMaxGlyphTree::SharedPtr, SynGlyphX::UUIDHash> MinMaxGlyphTreeMap;

		typedef boost::shared_ptr<DataTransform> SharedPtr;
		typedef boost::shared_ptr<const DataTransform> ConstSharedPtr;

        DataTransform();
		DataTransform(const GlyphTree& glyphTree);
        ~DataTransform();

        void ReadFromFile(const std::string& filename);
        void WriteToFile(const std::string& filename) const;

		const DatasourceMap& GetDatasources() const;

		boost::uuids::uuid AddDatasource(const std::wstring& name,
			Datasource::SourceType type,
            const std::wstring& host = L"localhost",
            unsigned int port = 0,
            const std::wstring& username = L"",
            const std::wstring& password = L"");

		void AddTables(const boost::uuids::uuid& id, const std::vector<std::wstring>& tables);

		boost::uuids::uuid AddGlyphTree(const MinMaxGlyphTree::SharedPtr glyphTree);
		const MinMaxGlyphTreeMap& DataTransform::GetGlyphTrees() const;

		void SetInputField(const boost::uuids::uuid& treeID, MinMaxGlyphTree::const_iterator& node, int index, const InputField& inputfield);
		void ClearInputBinding(const boost::uuids::uuid& treeID, MinMaxGlyphTree::const_iterator& node, int index);

		bool IsTransformable() const;

		void TransformToCSV(const std::string& filename) const;

    private:
        void Clear();
		void AddChildrenToGlyphTree(GlyphTree::SharedPtr tree, GlyphTree::iterator newNode, MinMaxGlyphTree::SharedPtr minMaxTree, MinMaxGlyphTree::const_iterator node, const std::unordered_map<InputField::HashID, QVariantList>& queryResultData, unsigned int index) const;
		QVariantList RunSqlQuery(const InputField& inputfield) const;
		void GetMinMax(InputBinding& binding, const InputField& inputField) const;
		double LinearInterpolate(const InputBinding& binding, double min, double difference, const std::unordered_map<InputField::HashID, QVariantList>& queryResultData, unsigned int index) const;
		Color ColorRGBInterpolate(const InputBinding& binding, const Color& min, const Color& difference, const std::unordered_map<InputField::HashID, QVariantList>& queryResultData, unsigned int index) const;
		GlyphProperties ProcessMinMaxGlyph(const MinMaxGlyphTree::const_iterator& minMaxGlyph, const std::unordered_map<InputField::HashID, QVariantList>& queryResultData, unsigned int index) const;

		DatasourceMap m_datasources;
        boost::uuids::random_generator m_uuidGenerator;
		MinMaxGlyphTreeMap m_glyphTrees;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATATRANSFORM