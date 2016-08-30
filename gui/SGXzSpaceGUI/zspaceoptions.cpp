#include "zspaceoptions.h"

namespace SynGlyphX {

	ZSpaceOptions::ZSpaceOptions() :
		m_stylusLength(0.15f),
		m_stylusColor(Qt::green)
	{
	}

	ZSpaceOptions::ZSpaceOptions(const ZSpaceOptions& options) :
		m_stylusLength(options.m_stylusLength),
		m_stylusColor(options.m_stylusColor) {


	}

	ZSpaceOptions::~ZSpaceOptions()
	{
	}

	ZSpaceOptions& ZSpaceOptions::operator=(const ZSpaceOptions& options) {

		m_stylusLength = options.m_stylusLength;
		m_stylusColor = options.m_stylusColor;

		return *this;
	}

	bool ZSpaceOptions::operator==(const ZSpaceOptions& options) const {

		if (m_stylusLength != options.m_stylusLength) {

			return false;
		}

		if (m_stylusColor != options.m_stylusColor) {

			return false;
		}

		return true;
	}

	bool ZSpaceOptions::operator!=(const ZSpaceOptions& options) const {

		return !operator==(options);
	}

	void ZSpaceOptions::SetStylusLength(float length) {

		if (length <= 0.0f) {

			throw std::invalid_argument("Stylus length can not be negative or zero");
		}

		m_stylusLength = length;
	}

	float ZSpaceOptions::GetStylusLength() const {

		return m_stylusLength;
	}

	void ZSpaceOptions::SetStylusColor(const QColor& color) {

		m_stylusColor = color;
	}

	const QColor& ZSpaceOptions::GetStylusColor() const {

		return m_stylusColor;
	}

} //namespace SynGlyphX