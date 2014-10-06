#ifndef SYNGLYPHX_CSVFILEREADER_H
#define SYNGLYPHX_CSVFILEREADER_H

#include "sgxfoundation.h"
#include <string>
#include <boost/tokenizer.hpp>
#include <fstream>
#include <vector>

namespace SynGlyphX {

	class SGXFOUNDATION_API CSVFileReader
	{
	public:
		typedef boost::tokenizer< boost::escaped_list_separator<char> > Tokenizer;
		typedef std::vector<std::string> CSVValues;

		CSVFileReader(const std::string& filename, char separator = ',');
		~CSVFileReader();

		const CSVValues& GetHeaders() const;

		CSVValues GetValuesFromLine();
		bool IsAtEndOfFile() const;

	protected:
		boost::escaped_list_separator<char> m_separators;
		CSVValues m_headers;
		std::ifstream m_filestream;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_CSFFILEREADER_H