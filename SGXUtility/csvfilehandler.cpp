#include "csvfilehandler.h"
#include <boost/lexical_cast.hpp>

namespace SynGlyphX {

	CSVFileHandler::CSVFileHandler(wchar_t separator) :
		m_separator(separator)
	{
	}


	CSVFileHandler::~CSVFileHandler()
	{
		
	}

	void CSVFileHandler::AddVector3ToCSVValues(CSVValues& values, const Vector3& vec) {

		values.push_back(boost::lexical_cast<std::wstring>(vec[0]));
		values.push_back(boost::lexical_cast<std::wstring>(vec[1]));
		values.push_back(boost::lexical_cast<std::wstring>(vec[2]));
	}

} //namespace SynGlyphX