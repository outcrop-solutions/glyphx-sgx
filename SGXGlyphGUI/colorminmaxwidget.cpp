#include "colorminmaxwidget.h"
#include "colorconverter.h"

namespace SynGlyphX {

	ColorMinMaxWidget::ColorMinMaxWidget(QWidget *parent)
		: HorizontalFormWidget(parent)
	{
		m_minColorButton = new ColorButton(this);
		AddWidget(tr("Min:"), m_minColorButton);

		m_maxColorButton = new ColorButton(this);
		AddWidget(tr("Max:"), m_maxColorButton);
	}

	ColorMinMaxWidget::~ColorMinMaxWidget()
	{

	}

	ColorMinDiff ColorMinMaxWidget::GetValue() const {

		ColorMinDiff minDiff;
		minDiff.SetMinMax(ColorConverter::QColor2GlyphColor(m_minColorButton->GetColor()), ColorConverter::QColor2GlyphColor(m_maxColorButton->GetColor()));
		return minDiff;
	}

	void ColorMinMaxWidget::SetValue(const ColorMinDiff& minDiff) {

		m_minColorButton->SetColor(ColorConverter::GlyphColor2QColor(minDiff.GetMin()));
		m_maxColorButton->SetColor(ColorConverter::GlyphColor2QColor(minDiff.GetMax()));
	}

} //namespace SynGlypX