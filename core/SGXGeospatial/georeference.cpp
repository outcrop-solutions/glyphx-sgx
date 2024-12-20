#include "georeference.h"
#include <cstdlib>
#include <boost/filesystem.hpp>

namespace SynGlyphX {

    GeoReference GeoReference::s_instance;

	GeoReference::GeoReference()
	{
	}


	GeoReference::~GeoReference()
	{
	}

    GeoReference& GeoReference::Instance() {

        return s_instance;
    }

	void GeoReference::GeoReferenceImage(const std::string& inputFilename, const std::string& outputFilename, const GeographicBoundingBox& boundingBox, const std::string& spatialRef) const {

        //This should probably use GDAL as a library but for faster initial development this will call gdal_translate

        boost::filesystem::path gdalExe = boost::filesystem::current_path() / "gdal" / "gdal_translate.exe";
        boost::filesystem::path gdalData = boost::filesystem::current_path() / "gdal" / "data";

        std::string imageFormat = outputFilename.substr(outputFilename.length() - 3);
        std::transform(imageFormat.begin(), imageFormat.end(), imageFormat.begin(), [](unsigned char c) { return std::toupper(c); });

        std::string gdalEnv = "GDAL_DATA=" + gdalData.string();
        std::string gdalCommand = "gdal\\gdal_translate.exe -a_ullr " + boundingBox.ToString() + " -a_srs " + spatialRef + " -of " + imageFormat + " \"" + inputFilename + "\" \"" + outputFilename + "\"";

        boost::filesystem::path outputErrFilename = "gdal\\gdal_translate_output.txt";
        if (!boost::filesystem::exists(outputErrFilename)) {
            boost::filesystem::remove(outputErrFilename);
        }
        gdalCommand += " > " + outputErrFilename.string() + " 2>&1";

        putenv(const_cast<char*>(gdalEnv.c_str()));

        if (std::system(gdalCommand.c_str()) == -1) {
#ifdef WIN32
            throw std::runtime_error(_strerror("Georeferce command failed:"));
#else
            throw std::runtime_error("Georeferce command failed:");		
#endif
        }

        if (!boost::filesystem::exists(outputFilename)) {
            
            throw std::runtime_error("Georefercing failed.");
        }

#ifdef _DEBUG
        boost::filesystem::remove(boost::filesystem::current_path() / "gdal" / "output.txt");
#endif
	}

} //namespace SynGlyphX
