#include "colorminmaxwidget.h"
#include "colorconverter.h"

namespace SynGlyphX {

	ColorMinMaxWidget::ColorMinMaxWidget(bool showAlphaInDialog, QWidget *parent)
		: HorizontalFormWidget(parent)
	{
		m_minColorButton = new ColorButton(showAlphaInDialog, this);
		m_minColorButton->setSizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Fixed);
		AddWidget(tr("Min:"), m_minColorButton);
		QObject::connect(m_minColorButton, &ColorButton::ColorChanged, this, &ColorMinMaxWidget::OnUserUpdated);

		m_maxColorButton = new ColorButton(showAlphaInDialog, this);
		m_maxColorButton->setSizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::Fixed);
		AddWidget(tr("Max:"), m_maxColorButton);
		QObject::connect(m_maxColorButton, &ColorButton::ColorChanged, this, &ColorMinMaxWidget::OnUserUpdated);
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

		m_minColorButton->blockSignals(true);
		m_maxColorButton->blockSignals(true);

		m_minColorButton->SetColor(ColorConverter::GlyphColor2QColor(minDiff.GetMin()));
		m_maxColorButton->SetColor(ColorConverter::GlyphColor2QColor(minDiff.GetMax()));

		m_minColorButton->blockSignals(false);
		m_maxColorButton->blockSignals(false);
	}

	void ColorMinMaxWidget::OnUserUpdated() {

		emit ValueChanged(GetValue());
	}

} //namespace SynGlypX