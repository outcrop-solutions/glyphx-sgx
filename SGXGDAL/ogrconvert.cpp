#include "ogrconvert.h"
#include <fstream>
#include <cstdlib>
#include <string.h>

namespace SynGlyphX {

    OGRConvert OGRConvert::s_instance;

    OGRConvert::OGRConvert()
    {
    }


    OGRConvert::~OGRConvert()
    {
    }

    OGRConvert& OGRConvert::Instance() {

        return s_instance;
    }

    void OGRConvert::ConvertKMLToCSV(const std::string& inputFilename, const std::string& outputFilename) const {

        boost::filesystem::path ogr2ogrExe = boost::filesystem::current_path() / "gdal" / "ogr2ogr.exe";
        boost::filesystem::path intermediateDir = boost::filesystem::temp_directory_path() / "intermediateCSV";

        boost::filesystem::path intermediateCSV = intermediateDir / "intermediate_temp_output.csv";

        if (!boost::filesystem::exists(intermediateDir)) {
            boost::filesystem::create_directory(intermediateDir);
        }

        boost::filesystem::path inputPath(inputFilename);

        //This should be replaced with using GDAL as a library at some point
        std::wstring ogr2ogrCommand = ogr2ogrExe.native() + L" -where \"OGR_GEOMETRY='Point'\" -lco GEOMETRY=AS_XYZ -select Name -f CSV " + intermediateCSV.native();
        if (inputPath.extension().string() == ".kmz") {
            ogr2ogrCommand += L" /vsizip/" + inputPath.generic_wstring() + L"/doc.kml";
        }
        else {
            ogr2ogrCommand += L" " + inputPath.native();
        }

        //Windows specific but since we will use GDAL as a library soon enough don't worry about it
        if (_wsystem(ogr2ogrCommand.c_str()) == -1) {
            
            throw std::exception(_strerror("Failed to create intermediate CSV: "));
        }

        MergeCSVFiles(intermediateDir, outputFilename);

        //Cleanup
        boost::filesystem::remove_all(intermediateDir);
    }

    void OGRConvert::MergeCSVFiles(const boost::filesystem::path& inputDir, const std::string& outputFilename) const {

        std::wofstream outputCSV;
        outputCSV.open(outputFilename.c_str());

        if (!outputCSV.is_open()) {
            throw std::exception("Output CSV failed to open");
        }

        bool headerNeedsToBeWritten = true;

        boost::filesystem::directory_iterator dirEnd;
        for (boost::filesystem::directory_iterator iT(inputDir); iT != dirEnd; ++iT) {
            if ((boost::filesystem::is_regular_file(iT->status())) && (iT->path().extension().string() == ".csv")) {
                std::wifstream inputCSV;
                inputCSV.open(iT->path().c_str());
                if (!outputCSV.is_open()) {
                    std::string error = iT->path().stem().string() + " failed to open";
                    throw std::exception(error.c_str());
                }

                std::wstring line;
                std::getline(inputCSV, line);

                if (headerNeedsToBeWritten) {

                    outputCSV << line << std::endl;
                    headerNeedsToBeWritten = false;
                }

                while (!inputCSV.eof()) {

                    std::getline(inputCSV, line);
                    outputCSV << line << std::endl;
                }

                inputCSV.close();
            }
        }

        outputCSV.close();
    }

} //namespace SynGlyphX