#ifndef SYNGLYPHX_DATATRANSFORM
#define SYNGLYPHX_DATATRANSFORM

#include "sgxdatatransform_global.h"
#include "datasource.h"
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace SynGlyphX {

    class SGXDATATRANSFORM_EXPORT DataTransform
    {
    public:
        DataTransform();
        ~DataTransform();

        void ReadFromFile(const std::string& filename);
        void WriteToFile(const std::string& filename) const;

        const std::map<std::wstring, Datasource>& GetDatasources() const;

        std::wstring AddDatasource(const std::wstring& name,
            const std::wstring& type,
            const std::wstring& host = L"localhost",
            const unsigned int port = 0,
            const std::wstring& username = L"",
            const std::wstring& password = L"");

        void AddTables(const std::wstring& id, const std::vector<std::wstring>& tables);

    private:
        void Clear();
		void AddDatasourcesToPropertyTree(boost::property_tree::wptree& propertyTree) const;

        std::map<std::wstring, Datasource> m_datasources;
        boost::uuids::random_generator m_uuidGenerator;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATATRANSFORM