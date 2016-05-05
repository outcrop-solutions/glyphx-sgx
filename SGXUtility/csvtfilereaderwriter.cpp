#include "csvtfilereaderwriter.h"
#include "csvfilewriter.h"
#include <boost/algorithm/string.hpp>

namespace SynGlyphX {

	CSVTFileReaderWriter::CSVTFileReaderWriter(const std::string& filename) :
		CSVFileReader(filename)
	{

	}


	CSVTFileReaderWriter::~CSVTFileReaderWriter()
	{
	}

	const CSVTFileReaderWriter::CSVValues& CSVTFileReaderWriter::GetTypes() const {

		return m_headers;
	}

	void CSVTFileReaderWriter::WriteCSVTFile(const std::string& filename, const CSVTFileReaderWriter::CSVValues& types) {

		unsigned int numberOfTypes = types.size();
		if (numberOfTypes == 0) {

			throw std::invalid_argument("CSVT Writer needs at least one type to write out a CSVT file.");
		}

		CSVTFileReaderWriter::CSVValues upperCaseTypes;
		for (unsigned int i = 0; i < numberOfTypes; ++i) {

			upperCaseTypes.push_back(boost::to_upper_copy(types[i]));
		}

		try {

			CSVFileWriter csvWriter(filename);
			csvWriter.WriteLine(upperCaseTypes);
			csvWriter.Close();
		}
		catch (const std::exception& e) {

			throw std::runtime_error("CSVT file failed to write");
		}
	}

} //namespace SynGlyphX
