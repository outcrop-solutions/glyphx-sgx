#include "csvfilewriter.h"
#include <stdexcept>

namespace SynGlyphX {

	CSVFileWriter::CSVFileWriter(char separator) :
		CSVFileHandler(separator),
		m_numFields(0)
	{
		
	}

	CSVFileWriter::CSVFileWriter(const std::string& filename, char separator) :
		CSVFileHandler(separator),
		m_numFields(0)
	{
		Open(filename);
	}


	CSVFileWriter::~CSVFileWriter()
	{
		Close();
	}

	void CSVFileWriter::Open(const std::string& filename) {

		m_filestream.open(filename, std::ofstream::out | std::ofstream::trunc);
		if (m_filestream.fail()) {

			throw std::runtime_error("CSV file failed to open");
		}
	}

	void CSVFileWriter::Close() {

		if (IsOpen()) {

			m_filestream.close();
		}
	}

	bool CSVFileWriter::IsOpen() const {

		return m_filestream.is_open();
	}

	void CSVFileWriter::WriteLine(const CSVValues& values) {

		if (!IsOpen()) {

			throw std::runtime_error("CSVFileWriter can't write line since file isn't open");
		}

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
		for (unsigned int i = 1; i < m_numFields; ++i) {
			
			m_filestream << m_separator << values[i];
		}
		m_filestream << std::endl;
	}

} //namespace SynGlyphX
