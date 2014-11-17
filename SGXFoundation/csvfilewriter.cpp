#include "csvfilewriter.h"
#include <stdexcept>

namespace SynGlyphX {

	CSVFileWriter::CSVFileWriter(const std::string& filename, char separator) :
		CSVFileHandler(separator),
		m_numFields(0)
	{
		m_filestream.open(filename, std::ofstream::out | std::ofstream::trunc);
		if (m_filestream.fail()) {

			throw std::exception("CSV file failed to open");
		}
	}


	CSVFileWriter::~CSVFileWriter()
	{
	}

	void CSVFileWriter::Close() {

		if (m_filestream.is_open()) {

			m_filestream.close();
		}
	}

	void CSVFileWriter::WriteLine(const CSVValues& values) {

		if (values.empty()) {

			throw std::invalid_argument("CSVFileWriter needs at least one value to write a line");
		}

		if (m_numFields == 0) {

			m_numFields = values.size();
		}
		else if (m_numFields != values.size()) {

			throw std::invalid_argument("CSVFileWriter needs to have the same number of values as headers to write a line");
		}

		m_filestream << values[0];
		for (int i = 1; i < m_numFields; ++i) {
			
			m_filestream << m_separator << values[i];
		}
		m_filestream << std::endl;
	}

} //namespace SynGlyphX