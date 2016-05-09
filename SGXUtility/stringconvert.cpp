#include "stringconvert.h"

namespace SynGlyphX {

	std::wstring_convert<std::codecvt_utf8<wchar_t>> StringConvert::s_toStdString;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> StringConvert::s_toStdWString;

	StringConvert::StringConvert()
	{
	}


	StringConvert::~StringConvert()
	{
	}

	std::string StringConvert::ToStdString(const std::wstring& str) {

		return s_toStdString.to_bytes(str);
	}
	
	std::wstring StringConvert::ToStdWString(const std::string& str) {

		return s_toStdWString.from_bytes(str);
	}

} //namespace SynGlyphX