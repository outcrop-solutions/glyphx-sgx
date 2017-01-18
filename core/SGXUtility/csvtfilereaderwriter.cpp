
#include "csvtfilereaderwriter.h"
#include "csvfilewriter.h"
#include <boost/algorithm/string.hpp>

namespace SynGlyphX {

	CSVTFileReaderWriter::CSVTFileReaderWriter(const char* filename) :
		CSVFileReader(filename)
	{

	}


	CSVTFileReaderWriter::~CSVTFileReaderWriter()
	{
	}

	const CSVFileHandler::CSVValues& CSVTFileReaderWriter::GetTypes() const {

		return GetHeaders();
	}

	void CSVTFileReaderWriter::WriteCSVTFile(const std::string& filename, const CSVFileHandler::CSVValues& types) {

		unsigned int numberOfTypes = (int)types.size();
		if (numberOfTypes == 0) {

			throw std::invalid_argument("CSVT Writer needs at least one type to write out a CSVT file.");
		}

		CSVFileHandler::CSVValues upperCaseTypes;
		for (unsigned int i = 0; i < numberOfTypes; ++i) {

			upperCaseTypes.push_back(boost::to_upper_copy(types[i]));
		}

		try {

			CSVFileWriter csvWriter(filename.c_str());
			csvWriter.WriteLine(upperCaseTypes);
			csvWriter.Close();
		}
		catch (const std::exception&) {

			throw std::runtime_error("CSVT file failed to write");
		}
	}

}
