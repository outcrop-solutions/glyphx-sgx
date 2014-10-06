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

		std::ofstream filestream;
		filestream.open(filename);

		if (filestream.fail()) {

			throw std::exception("CSVT file failed to open");
		}

		unsigned int lastIndex = types.size() - 1;
		for (unsigned int i = 0; i < types.size(); ++i) {

			const std::string& type = types[i];

			if (*type.begin() != '\"') {

				filestream << '\"';
			}
			filestream << boost::to_upper_copy(types[i]);
			if (*type.rbegin() != '\"') {

				filestream << '\"';
			}

			if (i != lastIndex) {

				filestream << std::endl;
			}
		}

		filestream.close();
	}

} //namespace SynGlyphX