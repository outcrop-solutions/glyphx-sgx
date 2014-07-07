#ifndef SYNGLYPHX_DATATRANSFORM
#define SYNGLYPHX_DATATRANSFORM

#include "sgxdatatransform_global.h"
#include "datasource.h"
#include <map>

namespace SynGlyphX {

    class SGXDATATRANSFORM_EXPORT DataTransform
    {
    public:
        DataTransform();
        ~DataTransform();

        void ReadFromFile(const std::wstring& filename);
        void WriteToFile(const std::wstring& filename) const;

        const std::map<std::wstring, Datasource>& GetDatasources() const;

    private:
        void Clear();

        std::map<std::wstring, Datasource> m_datasources;
    };

} //namespace SynGlyphX

#endif //SYNGLYPHX_DATATRANSFORM