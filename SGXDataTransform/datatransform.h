#ifndef SYNGLYPHX_DATATRANSFORM
#define SYNGLYPHX_DATATRANSFORM

#include "sgxdatatransform_global.h"
#include "datasource.h"
#include <unordered_map>
#include <boost/property_tree/ptree.hpp>
#include "UUID.h"
#include <boost/uuid/uuid_generators.hpp>
#include "glyphmappableproperties.h"

namespace SynGlyphX {

    class SGXDATATRANSFORM_EXPORT DataTransform
    {
    public:
		typedef std::unordered_map<boost::uuids::uuid, Datasource, SynGlyphX::UUIDHash> DatasourceMap;
        DataTransform();
        ~DataTransform();

        void ReadFromFile(const std::string& filename);
        void WriteToFile(const std::string& filename) const;

		const DatasourceMap& GetDatasources() const;

		boost::uuids::uuid AddDatasource(const std::wstring& name,
            const std::wstring& type,
            const std::wstring& host = L"localhost",
            const unsigned int port = 0,
            const std::wstring& username = L"",
            const std::wstring& password = L"");

		void AddTables(const boost::uuids::uuid& id, const std::vector<std::wstring>& tables);

    private:
        void Clear();
		void AddDatasourcesToPropertyTree(boost::property_tree::wptree& propertyTree) const;

		DatasourceMap m_datasources;
        boost::uuids::random_generator m_uuidGenerator;
		GlyphMappableProperties m_minGlyph;
		GlyphMappableProperties m_maxGlyph;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATATRANSFORM