#include "csvtfilereaderwriter.h"
#include <fstream>
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

		std::ofstream filestream;
		filestream.open(filename);

		if (filestream.fail()) {

			throw std::exception("CSVT file failed to open");
		}

		unsigned int lastIndex = numberOfTypes - 1;
		for (unsigned int i = 0; i < numberOfTypes; ++i) {

			const std::string& type = types[i];

			if (*type.begin() != '\"') {

				filestream << '\"';
			}
			filestream << boost::to_upper_copy(types[i]);
			if (*type.rbegin() != '\"') {

				filestream << '\"';
			}

			if (i != lastIndex) {

				filestream << ",";
			}
		}

		filestream << std::endl;

		filestream.close();
	}

} //namespace SynGlyphX