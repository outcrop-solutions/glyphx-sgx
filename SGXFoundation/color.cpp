#include "color.h"
#include <stdexcept>
#include <string>
#include <sstream>

namespace SynGlyphX {

	Color::Color() :
		m_color{ {0, 0, 0, 255 } }
	{
	}

	Color::Color(const ColorArray& color) :
		m_color(color){

	}

	Color::Color(const Color& color) : 
		m_color(color.m_color) {

	}

	Color::~Color()
	{
	}

	Color& Color::operator=(const Color& color) {

		m_color = color.m_color;

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

	short Color::operator[](unsigned int index) const {

		if (index < 4) {
			return m_color[index];
		}
		else {
			throw std::out_of_range("For color, an index of 4 or greater is invalid");
		}
	}

	bool Color::operator==(const Color& color) const {

		return (m_color == color.m_color);
	}

	bool Color::operator!=(const Color& color) const {

		return (m_color != color.m_color);
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

} //namespace SynGlyphX