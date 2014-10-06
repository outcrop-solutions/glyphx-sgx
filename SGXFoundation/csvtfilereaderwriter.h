#ifndef SYNGLYPHX_CSVTFILEREADERWRITER_H
#define SYNGLYPHX_CSVTFILEREADERWRITER_H

#include "sgxfoundation.h"
#include "csvfilereader.h"
#include <string>

namespace SynGlyphX {

	class SGXFOUNDATION_API CSVTFileReaderWriter : protected CSVFileReader
	{
	public:
		CSVTFileReaderWriter(const std::string& filename);
		~CSVTFileReaderWriter();

		const CSVValues& GetTypes() const;

		static void WriteCSVTFile(const std::string& filename, const CSVValues& types);
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_CSVTFILEREADERWRITER_H