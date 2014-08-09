#ifndef SYNGLYPHX_COLOR_H
#define SYNGLYPHX_COLOR_H

#include "sgxfoundation.h"
#include <array>

namespace SynGlyphX {

	class SGXFOUNDATION_API Color
	{
	public:
		typedef std::array<short, 4> ColorArray;
		Color();
		Color(const ColorArray& color);
		Color(const Color& color);
		~Color();

		Color& operator=(const Color& color);
		void Set(unsigned int index, short value);
		short operator[](unsigned int index) const;
		bool operator==(const Color& color) const;
		bool operator!=(const Color& color) const;

		std::wstring ToHexString(unsigned int length) const;
		void FromHexString(const std::wstring& hexString);

	private:
		ColorArray m_color;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_COLOR_H