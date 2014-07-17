#ifndef SYNGLYPHX_DATATRANSFORM
#define SYNGLYPHX_DATATRANSFORM

#include "sgxdatatransform_global.h"
#include "datasource.h"
#include <unordered_map>
#include <boost/property_tree/ptree.hpp>
#include "UUID.h"
#include <boost/uuid/uuid_generators.hpp>
#include "minmaxglyph.h"

namespace SynGlyphX {

    class SGXDATATRANSFORM_EXPORT DataTransform
    {
    public:
		typedef std::unordered_map<boost::uuids::uuid, Datasource, SynGlyphX::UUIDHash> DatasourceMap;

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

    private:
        void Clear();
		void AddDatasourcesToPropertyTree(boost::property_tree::wptree& propertyTree) const;
		void CreateDatasourcesFromPropertyTree(boost::property_tree::wptree& propertyTree);

		DatasourceMap m_datasources;
        boost::uuids::random_generator m_uuidGenerator;
		MinMaxGlyphTree m_glyphTree;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATATRANSFORM