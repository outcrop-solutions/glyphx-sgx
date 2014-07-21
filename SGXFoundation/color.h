#ifndef SYNGLYPHX_COLOR_H
#define SYNGLYPHX_COLOR_H

#include "sgxfoundation.h"
#include <array>

namespace SynGlyphX {

	class SGXFOUNDATION_API Color
	{
	public:
		Color();
		Color(const std::array<unsigned char, 4>& color);
		Color(const Color& color);
		~Color();

		Color& operator=(const Color& color);
		unsigned char& operator[](unsigned int index);
		unsigned char operator[](unsigned int index) const;
		bool operator==(const Color& color) const;
		bool operator!=(const Color& color) const;

		std::wstring ToHexString(unsigned int length) const;
		void FromHexString(const std::wstring& hexString);

	private:
		std::array<unsigned char, 4> m_color;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_COLOR_H