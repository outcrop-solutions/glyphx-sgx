
#include "StringConvert.h"
#include <locale>
#include <codecvt>

namespace GlyphEngine {

	namespace
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> s_toStdString;
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> s_toStdWString;
	}

	std::string StringConvert::ToStdString(const std::wstring& str) {

		return s_toStdString.to_bytes(str);
	}
	
	std::wstring StringConvert::ToStdWString(const std::string& str) {

		return s_toStdWString.from_bytes(str);
	}

} //namespace SynGlyphX
