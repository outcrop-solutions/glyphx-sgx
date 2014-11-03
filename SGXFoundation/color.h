#ifndef SYNGLYPHX_COLOR_H
#define SYNGLYPHX_COLOR_H

#include "sgxfoundation.h"
#include <array>

namespace SynGlyphX {

	class SGXFOUNDATION_API Color
	{
	public:
		enum Space {RGB, HSV};

		typedef std::array<short, 4> ColorArray;

		Color(Space space = Space::RGB);
		Color(const ColorArray& color, Space space = Space::RGB);
		Color(const Color& color);
		~Color();

		Color& operator=(const Color& color);
		void Set(unsigned int index, short value);
		void Set(short red, short green, short blue, short alpha = 255);
		short operator[](unsigned int index) const;
		bool operator==(const Color& color) const;
		bool operator!=(const Color& color) const;
		Color& operator+=(const Color& color);
		Color& operator-=(const Color& color);

		std::wstring ToHexString(unsigned int length) const;
		void FromHexString(const std::wstring& hexString);

		Space GetSpace() const;

		static Color ConvertRGBtoHSV(const Color& color);
		static Color ConvertHSVtoRGB(const Color& color);

	private:
		ColorArray m_color;
		Space m_space;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_COLOR_H