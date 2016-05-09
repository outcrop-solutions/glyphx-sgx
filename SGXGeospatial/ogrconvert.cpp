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

        //boost::filesystem::path ogr2ogrExe = L"gdal" / "ogr2ogr.exe";
        boost::filesystem::path intermediateDir = boost::filesystem::current_path() / "gdal" / "intermediateCSV";

        boost::filesystem::path intermediateCSV = intermediateDir / "intermediate_temp_output.csv";

        if (!boost::filesystem::exists(intermediateDir)) {
			boost::filesystem::create_directories(intermediateDir);
        }

        boost::filesystem::path inputPath(inputFilename);

        //This should be replaced with using GDAL as a library at some point
        std::wstring ogr2ogrCommand = L"gdal\\ogr2ogr.exe -append -where \"OGR_GEOMETRY='Point'\" -lco GEOMETRY=AS_XYZ -select Name -f CSV \"";
#ifdef WIN32
	ogr2ogrCommand += intermediateCSV.native();
#else
	ogr2ogrCommand += intermediateCSV.wstring();
#endif
	ogr2ogrCommand += L"\"";
        if (inputPath.extension().string() == ".kmz") {
			std::wstring genericInputPath = inputPath.generic_wstring();
			ogr2ogrCommand += L" \"/vsizip/" + genericInputPath + L"/doc.kml\"";
        }
        else {
            ogr2ogrCommand += L" \"";
#ifdef WIN32
		ogr2ogrCommand += inputPath.native();
#else
		ogr2ogrCommand += inputPath.wstring();
#endif
            ogr2ogrCommand += L" \"";
        }

        boost::filesystem::path ogr2ogrOutput = boost::filesystem::current_path() / "gdal" / "ogr2ogr_output.txt";
        if (!boost::filesystem::exists(ogr2ogrOutput)) {
            boost::filesystem::remove(ogr2ogrOutput);
        }
        ogr2ogrCommand += L" > ";
#ifdef WIN32
		ogr2ogrCommand += ogr2ogrOutput.native();
#else
		ogr2ogrCommand += ogr2ogrOutput.wstring();
#endif
	ogr2ogrCommand += L" 2>&1";

        //Windows specific but since we will use GDAL as a library soon enough don't worry about it
#ifdef WIN32
	if (_wsystem(ogr2ogrCommand.c_str()) == -1) {
            throw std::runtime_error(_strerror("Failed to create intermediate CSV: "));
#else
	std::string s( ogr2ogrCommand.begin(), ogr2ogrCommand.end() );
	if (system(s.c_str()) == -1) {
            throw std::runtime_error("Failed to create intermediate CSV: ");
#endif
	}

        MergeCSVFiles(intermediateDir, outputFilename);

        //Cleanup
        boost::filesystem::remove_all(intermediateDir);
    }

    void OGRConvert::MergeCSVFiles(const boost::filesystem::path& inputDir, const std::string& outputFilename) const {

        std::wofstream outputCSV;
        outputCSV.open(outputFilename.c_str());

        if (!outputCSV.is_open()) {
            throw std::runtime_error("Output CSV failed to open");
        }

        bool headerNeedsToBeWritten = true;
        bool csvFilesWereProcessed = false;

        boost::filesystem::directory_iterator dirEnd;
        for (boost::filesystem::directory_iterator iT(inputDir); iT != dirEnd; ++iT) {
            if ((boost::filesystem::is_regular_file(iT->status())) && (iT->path().extension().string() == ".csv")) {
                std::wifstream inputCSV;
                inputCSV.open(iT->path().c_str());
                if (!outputCSV.is_open()) {
                    std::string error = iT->path().stem().string() + " failed to open";
                    throw std::runtime_error(error.c_str());
                }

                std::wstring line;
                std::getline(inputCSV, line);

                if (headerNeedsToBeWritten) {

                    outputCSV << line << std::endl;
                    headerNeedsToBeWritten = false;
                }

                while (!inputCSV.eof()) {

                    std::getline(inputCSV, line);
					if (!line.empty()) {
						outputCSV << line << std::endl;
					}
                }

                inputCSV.close();

                csvFilesWereProcessed = true;
            }
        }

        outputCSV.close();

        if (!csvFilesWereProcessed) {
            throw std::runtime_error("No intermediate CSVs were created");
        }
    }

} //namespace SynGlyphX
