#include "color.h"
#include <stdexcept>
#include <string>
#include <sstream>

namespace SynGlyphX {

	Color::Color(Space space) :
		m_space(space),
		m_color{ {0, 0, 0, 255 } }
	{
	}

	Color::Color(const ColorArray& color, Space space) :
		m_space(space),
		m_color(color) {

	}

	Color::Color(const Color& color) : 
		m_space(color.m_space),
		m_color(color.m_color) {

	}

	Color::~Color()
	{
	}

	Color& Color::operator=(const Color& color) {

		m_color = color.m_color;
		m_space = color.m_space;

		return *this;
	}

	void Color::Set(unsigned int index, short value) {

		if ((value < -255) || (value > 255)) {

			throw std::out_of_range("For color, a value outside of [-255, 255] is invalid");
		} 
		else {

			if (index < 4) {
				m_color[index] = value;
			}
			else {
				throw std::out_of_range("For color, an index of 4 or greater is invalid");
			}
		}
	}

	void Color::Set(short red, short green, short blue, short alpha) {

		Set(0, red);
		Set(1, green);
		Set(2, blue);
		Set(3, alpha);
	}

	short Color::operator[](unsigned int index) const {

		if (index < 4) {
			return m_color[index];
		}
		else {
			throw std::out_of_range("For color, an index of 4 or greater is invalid");
		}
	}

	bool Color::operator==(const Color& color) const {

		return ((m_color == color.m_color) && (m_space == color.m_space));
	}

	bool Color::operator!=(const Color& color) const {

		return (!operator==(color));
	}

	Color& Color::operator+=(const Color& color) {

		m_color[0] += color.m_color[0];
		m_color[1] += color.m_color[1];
		m_color[2] += color.m_color[2];
		m_color[3] += color.m_color[3];

		return *this;
	}

	Color& Color::operator-=(const Color& color) {

		m_color[0] -= color.m_color[0];
		m_color[1] -= color.m_color[1];
		m_color[2] -= color.m_color[2];
		m_color[3] -= color.m_color[3];

		return *this;
	}

	std::wstring Color::ToHexString(unsigned int length) const {

		if ((length > 4) || (length == 0)) {
			throw std::invalid_argument("To create hex string length must be between 1 and 4");
		}

		std::wostringstream stream;
		stream << std::hex;
		for (unsigned int i = 0; i < length; ++i) {

			//If the color channel can be represented with one byte, need to force the leading zero to be written to the stream
			if (m_color[i] < 16) {
				stream << 0;
			}
			stream << m_color[i];
		}

		return stream.str();
	}

	void Color::FromHexString(const std::wstring& hexString) {

		if ((hexString.length() <= 10) && (hexString.length() % 2 == 0)) {

			for (unsigned int i = 0; i < hexString.length(); i += 2) {

				unsigned int num = std::stoul(hexString.substr(i, 2), nullptr, 16);
				m_color[i/2] = num;
			}
		}
		else {
			throw std::invalid_argument("Hexadecimal string is invalid");
		}
	}

	Color::Space Color::GetSpace() const {

		return m_space;
	}

	Color Color::ConvertRGBtoHSV(const Color& color) {

		float red = color[0] / 255.0f;
		float green = color[1] / 255.0f;
		float blue = color[2] / 255.0f;

		float min = std::min(std::min(red, green), blue);
		float max = std::max(std::max(red, green), blue);

		float value = max;
		float delta = max - min;

		float saturation = 0.0f;
		if (max != 0.0f) {

			saturation = delta / max;
		}

		float hue = 0.0f;
		if (min != max) {

			if (red == max) {

				hue = (green - blue) / delta;
				if (green < blue) {

					hue += 6.0f;
				}
			}
			else if (green == max) {

				hue = 2 + (blue - red) / delta;
			}
			else {

				hue = 4 + (red - green) / delta;
			}

			hue /= 6.0f;
		}

		Color hsvColor(Color::Space::HSV);

		//Copy alpha since it isn't a part of RGB->HSV conversion
		hsvColor.Set(static_cast<short>(hue * 255), static_cast<short>(saturation * 255), static_cast<short>(value * 255), color[3]);

		return hsvColor;
	}

	Color Color::ConvertHSVtoRGB(const Color& color) {

		float hue = color[0] / 255.0f;
		float saturation = color[1] / 255.0f;
		float value = color[2] / 255.0f;

		int i = std::floor(hue * 6);
		float f = (hue * 6) - i;
		float p = value * (1.0f - saturation);
		float q = value * (1.0f - f * saturation);
		float t = value * (1.0f - ((1.0f - f) * saturation));

		Color rgbColor;

		switch (i % 6) {

			//Copy alpha since it isn't a part of RGB->HSV conversion
			case 0: rgbColor.Set(value * 255, t * 255, p * 255, color[3]); break;
			case 1: rgbColor.Set(q * 255, value * 255, p * 255, color[3]); break;
			case 2: rgbColor.Set(p * 255, value * 255, t * 255, color[3]); break;
			case 3: rgbColor.Set(p * 255, q * 255, value * 255, color[3]); break;
			case 4: rgbColor.Set(t * 255, p * 255, value * 255, color[3]); break;
			case 5: rgbColor.Set(value * 255, p * 255, q * 255, color[3]); break;
		}

		return rgbColor;
	}

} //namespace SynGlyphX