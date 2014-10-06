#ifndef SYNGLYPHX_CSVFILEREADER_H
#define SYNGLYPHX_CSFFILEREADER_H

#include <string>
#include <boost/tokenizer.hpp>
#include <fstream>
#include <vector>

namespace SynGlyphX {

	class CSVFileReader
	{
	public:
		typedef boost::tokenizer< boost::escaped_list_separator<char> > Tokenizer;
		typedef std::vector<std::string> CSVValues;

		CSVFileReader(const std::string& filename, char separator = ',');
		~CSVFileReader();

		const CSVValues& GetHeaders() const;

		CSVValues GetValuesFromLine();
		bool IsAtEndOfFile() const;

	private:
		boost::escaped_list_separator<char> m_separators;
		CSVValues m_headers;
		std::ifstream m_filestream;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_CSFFILEREADER_H