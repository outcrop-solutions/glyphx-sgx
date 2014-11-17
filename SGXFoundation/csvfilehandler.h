#ifndef SYNGLYPHX_CSVFILEHANDLER_H
#define SYNGLYPHX_CSVFILEHANDLER_H

#include <vector>
#include "sgxfoundation.h"
#include "foundationtypes.h"

namespace SynGlyphX {

	class SGXFOUNDATION_API CSVFileHandler
	{
	public:
		typedef std::vector<std::wstring> CSVValues;

		CSVFileHandler(wchar_t separator = ',');
		virtual ~CSVFileHandler();

		virtual void Close() = 0;

		static void AddVector3ToCSVValues(CSVValues& values, const Vector3& vec);

	protected:
		wchar_t m_separator;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_CSVFILEHANDLER_H