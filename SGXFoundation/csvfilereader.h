#ifndef SYNGLYPHX_CSVFILEREADER_H
#define SYNGLYPHX_CSVFILEREADER_H

#include "csvfilehandler.h"
#include <string>
#include <boost/tokenizer.hpp>
#include <fstream>
#include <vector>

namespace SynGlyphX {

	class SGXFOUNDATION_API CSVFileReader : public CSVFileHandler
	{
	public:
		typedef boost::tokenizer< boost::escaped_list_separator<wchar_t>, std::wstring::const_iterator, std::wstring > Tokenizer;

		CSVFileReader(const std::string& filename, wchar_t separator = ',');
		virtual ~CSVFileReader();

		const CSVValues& GetHeaders() const;

		CSVValues GetValuesFromLine();
		bool IsAtEndOfFile() const;

		virtual void Close();

	protected:
		boost::escaped_list_separator<wchar_t> m_separators;
		CSVValues m_headers;
		std::wifstream m_filestream;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_CSFFILEREADER_H