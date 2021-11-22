#include "ObjectColor.h"
#include <stdexcept>
#include <string>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>

namespace GlyphEngine {

	ObjectColor::ObjectColor(Space space) :
        m_color(ObjectColor::ColorArray({ { 0, 0, 0 } })),
		m_space(space)
	{
	}

	ObjectColor::ObjectColor(const ColorArray& color, Space space) :
		m_color(color),
        m_space(space) {

	}

	/*ObjectColor::ObjectColor(const boost::property_tree::wptree& propertyTree, Space space) :
		m_space(space),
		m_color({ { propertyTree.get<short>(L"R"), propertyTree.get<short>(L"G"), propertyTree.get<short>(L"B") } }) {

	}*/

	ObjectColor::ObjectColor(const ObjectColor& color) : 
        m_color(color.m_color),
        m_space(color.m_space) {

	}

	ObjectColor::~ObjectColor()
	{
	}

	ObjectColor& ObjectColor::operator=(const ObjectColor& color) {

		m_color = color.m_color;
		m_space = color.m_space;

		return *this;
	}

	void ObjectColor::Set(unsigned int index, short value) {

		if ((value < -255) || (value > 255)) {

			throw std::out_of_range("For color, a value outside of [-255, 255] is invalid");
		} 
		else {

			if (index < 3) {

				m_color[index] = value;
			}
			else {

				throw std::out_of_range("For color, an index of 4 or greater is invalid");
			}
		}
	}

	void ObjectColor::Set(short red, short green, short blue) {

		Set(0, red);
		Set(1, green);
		Set(2, blue);
	}

	void ObjectColor::SetF( float r, float g, float b )
	{
		Set( 0, short( r * 255.f ) );
		Set( 1, short( g * 255.f ) );
		Set( 2, short( b * 255.f ) );
	}

	short ObjectColor::operator[](unsigned int index) const {

		if (index < 3) {

			return m_color[index];
		}
		else {
			throw std::out_of_range("For color, an index of 4 or greater is invalid");
		}
	}

	bool ObjectColor::operator==(const ObjectColor& color) const {

		return ((m_color == color.m_color) && (m_space == color.m_space));
	}

	bool ObjectColor::operator!=(const ObjectColor& color) const {

		return (!operator==(color));
	}

	ObjectColor& ObjectColor::operator+=(const ObjectColor& color) {

		m_color[0] += color.m_color[0];
		m_color[1] += color.m_color[1];
		m_color[2] += color.m_color[2];

		return *this;
	}

	ObjectColor& ObjectColor::operator-=(const ObjectColor& color) {

		m_color[0] -= color.m_color[0];
		m_color[1] -= color.m_color[1];
		m_color[2] -= color.m_color[2];

		return *this;
	}

	const ObjectColor ObjectColor::operator+(const ObjectColor& color) const {

		return ObjectColor(*this) += color;
	}

	const ObjectColor ObjectColor::operator-(const ObjectColor& color) const {

		return ObjectColor(*this) -= color;
	}

	void ObjectColor::ExportToPropertyTree(boost::property_tree::wptree& propertyTree) const {

		propertyTree.put<short>(L"R", m_color[0]);
		propertyTree.put<short>(L"G", m_color[1]);
		propertyTree.put<short>(L"B", m_color[2]);
	}

	std::wstring ObjectColor::ToHexString(unsigned int length) const {

		if ((length > 3) || (length == 0)) {
			throw std::invalid_argument("To create hex string length must be between 1 and 3");
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

	void ObjectColor::FromHexString(const std::wstring& hexString) {

		if ((hexString.length() <= 10) && (hexString.length() % 2 == 0)) {

			for (unsigned int i = 0; i < hexString.length(); i += 2) {
				
				m_color[i / 2]  = (unsigned short)std::stoul(hexString.substr(i, 2), nullptr, 16);
			}
		}
		else {
			throw std::invalid_argument("Hexadecimal string is invalid");
		}
	}

	ObjectColor::Space ObjectColor::GetSpace() const {

		return m_space;
	}

	ObjectColor ObjectColor::ConvertRGBtoHSV(const ObjectColor& color) {

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

		ObjectColor hsvColor(ObjectColor::Space::HSV);

		//Copy alpha since it isn't a part of RGB->HSV conversion
		hsvColor.Set(static_cast<short>(hue * 255), static_cast<short>(saturation * 255), static_cast<short>(value * 255));

		return hsvColor;
	}

	ObjectColor ObjectColor::ConvertHSVtoRGB(const ObjectColor& color) {

		float hue = color[0] / 255.0f;
		float saturation = color[1] / 255.0f;
		float value = color[2] / 255.0f;

		float i = std::floor(hue * 6);
		float f = (hue * 6) - i;
		float p = value * (1.0f - saturation);
		float q = value * (1.0f - f * saturation);
		float t = value * (1.0f - ((1.0f - f) * saturation));

		ObjectColor rgbColor;

		switch (int( i ) % 6) {

			//Copy alpha since it isn't a part of RGB->HSV conversion
			case 0: rgbColor.SetF(value, t, p); break;
			case 1: rgbColor.SetF(q, value, p); break;
			case 2: rgbColor.SetF(p, value, t); break;
			case 3: rgbColor.SetF(p, q, value); break;
			case 4: rgbColor.SetF(t, p, value); break;
			case 5: rgbColor.SetF(value, p, q); break;
		}

		return rgbColor;
	}

	ObjectColorTranslator::ObjectColorTranslator() {


	}

	boost::optional<ObjectColor> ObjectColorTranslator::get_value(std::wstring const &v) {

		int firstComma = (int)v.find_first_of(L',');
		int secondComma = (int)v.find_last_of(L',');

		try {

			if ((firstComma > 0) && (secondComma > firstComma + 1) && (secondComma < (int( v.length() ) - 1))) {

				ObjectColor color;
				color.Set(0, boost::lexical_cast<short>(v.substr(0, firstComma)));
				color.Set(1, boost::lexical_cast<short>(v.substr(firstComma + 1, secondComma - firstComma - 1)));
				color.Set(2, boost::lexical_cast<short>(v.substr(secondComma + 1, v.length() - secondComma - 1)));

				return color;
			}
		}
		catch (...) {


		}

		return boost::none;
	}

	boost::optional<std::wstring> ObjectColorTranslator::put_value(ObjectColor const& v) {

		return (boost::lexical_cast<std::wstring>(v[0]) + L',' + boost::lexical_cast<std::wstring>(v[1]) + L',' + boost::lexical_cast<std::wstring>(v[2]));
	}

} 
