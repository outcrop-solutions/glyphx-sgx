#ifndef SYNGLYPHX_DATATRANSFORM
#define SYNGLYPHX_DATATRANSFORM

#include "sgxdatatransform_global.h"
#include "datasource.h"
#include <map>
#include <boost/property_tree/ptree.hpp>

namespace SynGlyphX {

    class SGXDATATRANSFORM_EXPORT DataTransform
    {
    public:
        DataTransform();
        ~DataTransform();

        void ReadFromFile(const std::string& filename);
        void WriteToFile(const std::string& filename) const;

        const std::map<std::wstring, Datasource>& GetDatasources() const;

    private:
        void Clear();
		void AddDatasourcesToPropertyTree(boost::property_tree::wptree& propertyTree) const;

        std::map<std::wstring, Datasource> m_datasources;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATATRANSFORM