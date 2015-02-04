#include "csvfilereader.h"
#include <exception>

namespace SynGlyphX {

	CSVFileReader::CSVFileReader(const std::string& filename, wchar_t separator) :
		CSVFileHandler(separator),
		m_separators('\\', m_separator, '\"')
	{
		m_filestream.open(filename);
		if (m_filestream.fail()) {

			throw std::exception("CSV file failed to open");
		}

		//Headers are on the first line
		m_headers = GetValuesFromLine();
		if (m_headers.empty()) {

			throw std::exception("CSV file has no headers");
		}
	}


	CSVFileReader::~CSVFileReader()
	{
		Close();
	}

	const CSVFileReader::CSVValues& CSVFileReader::GetHeaders() const {

		return m_headers;
	}

	CSVFileReader::CSVValues CSVFileReader::GetValuesFromLine() {

		if (m_filestream.eof()) {

			throw std::exception("CSV file is at its end");
		}

		std::wstring buffer, line;
		bool insideQuotes = false;

		//there could be line breaks in the middle of a field so if there is merge the lines
		do {

			std::getline(m_filestream, buffer);
			size_t lastQuotePosition = buffer.find_first_of('"');
			while (lastQuotePosition != std::string::npos) {
			
				insideQuotes = !insideQuotes;
				lastQuotePosition = buffer.find_first_of('"', lastQuotePosition + 1);
			}

			line.append(buffer);

			if (insideQuotes) {
			
				line.append(L"\n");
			}
		} while (insideQuotes);

		CSVValues values;
		Tokenizer csvTokenizer(line, m_separators);
		values.assign(csvTokenizer.begin(), csvTokenizer.end());

		return values;
	}

	bool CSVFileReader::IsAtEndOfFile() const {

		return (m_filestream.eof());
	}

	void CSVFileReader::Close() {

		if (m_filestream.is_open()) {

			m_filestream.close();
		}
	}

} //namespace SynGlyphX