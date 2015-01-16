#ifndef SYNGLYPHX_GLYPHCOLOR_H
#define SYNGLYPHX_GLYPHCOLOR_H

#include "sgxfoundation.h"
#include <array>

namespace SynGlyphX {

	class SGXFOUNDATION_API GlyphColor
	{
	public:
		enum Space {RGB, HSV};

		typedef std::array<short, 3> ColorArray;

		GlyphColor(Space space = Space::RGB);
		GlyphColor(const ColorArray& color, Space space = Space::RGB);
		GlyphColor(const GlyphColor& color);
		~GlyphColor();

		GlyphColor& operator=(const GlyphColor& color);
		void Set(unsigned int index, short value);
		void Set(short red, short green, short blue);
		short operator[](unsigned int index) const;
		bool operator==(const GlyphColor& color) const;
		bool operator!=(const GlyphColor& color) const;
		GlyphColor& operator+=(const GlyphColor& color);
		GlyphColor& operator-=(const GlyphColor& color);

		std::wstring ToHexString(unsigned int length = 3) const;
		void FromHexString(const std::wstring& hexString);

		Space GetSpace() const;

		static GlyphColor ConvertRGBtoHSV(const GlyphColor& color);
		static GlyphColor ConvertHSVtoRGB(const GlyphColor& color);

	private:
		ColorArray m_color;
		Space m_space;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_GLYPHCOLOR_H