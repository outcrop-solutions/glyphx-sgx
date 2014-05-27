#ifndef OGRCONVERT_H
#define OGRCONVERT_H

#include "sgxggeospatial.h"
#include <string>
#include <boost/filesystem.hpp>

namespace SynGlyphX {

	class SGXGGEOSPATIAL_API OGRConvert
    {
    public:
        OGRConvert();
        ~OGRConvert();

        static OGRConvert& Instance();

        void ConvertKMLToCSV(const std::string& inputFilename, const std::string& outputFilename) const;

    private:
        void MergeCSVFiles(const boost::filesystem::path& inputDir, const std::string& outputFilename) const;

        static OGRConvert s_instance;
    };

} //namespace SynGlyphX

#endif //OGRCONVERT_H