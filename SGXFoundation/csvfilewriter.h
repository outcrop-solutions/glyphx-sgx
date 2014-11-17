#ifndef SYNGLYPHX_CSVFILEWRITER_H
#define SYNGLYPHX_CSVFILEWRITER_H

#include "csvfilehandler.h"
#include <string>
#include <fstream>

namespace SynGlyphX {

	class SGXFOUNDATION_API CSVFileWriter : public CSVFileHandler
	{
	public:
		CSVFileWriter(const std::string& filename, char separator = ',');
		virtual ~CSVFileWriter();

		virtual void Close();
		void WriteLine(const CSVValues& values);

	private:
		std::wofstream m_filestream;
		unsigned int m_numFields;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_CSVFILEWRITER_H