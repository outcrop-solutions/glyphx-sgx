#include "colorconverter.h"

namespace SynGlyphX {

	ColorConverter::ColorConverter()
	{

	}

	ColorConverter::~ColorConverter()
	{

	}

	QColor ColorConverter::GlyphColor2QColor(const GlyphColor& color) {

		QColor qColor(color[0], color[1], color[2]);
		return qColor;
	}
	
	QColor ColorConverter::GlyphColor2QColor(const GlyphColor& color, unsigned int alpha) {

		QColor qColor(color[0], color[1], color[2], alpha);
		return qColor;
	}

	GlyphColor ColorConverter::QColor2GlyphColor(const QColor& qColor) {

		GlyphColor color;

		color.Set(0, qColor.red());
		color.Set(1, qColor.green());
		color.Set(2, qColor.blue());

		return color;
	}
}