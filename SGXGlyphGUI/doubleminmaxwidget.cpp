#include "doubleminmaxwidget.h"

namespace SynGlyphX {

	DoubleMinMaxWidget::DoubleMinMaxWidget(QWidget *parent)
		: HorizontalFormWidget(parent)
	{
		m_minSpinBox = new QDoubleSpinBox(this);
		AddWidget(tr("Min:"), m_minSpinBox);

		m_maxSpinBox = new QDoubleSpinBox(this);
		AddWidget(tr("Max:"), m_maxSpinBox);
	}

	DoubleMinMaxWidget::~DoubleMinMaxWidget()
	{

	}

	DoubleMinDiff DoubleMinMaxWidget::GetValue() const {

		DoubleMinDiff minDiff;
		minDiff.SetMinMax(m_minSpinBox->value(), m_maxSpinBox->value());
		return minDiff;
	}

	void DoubleMinMaxWidget::SetValue(const DoubleMinDiff& minDiff) {

		m_minSpinBox->setValue(minDiff.GetMin());
		m_maxSpinBox->setValue(minDiff.GetMax());
	}

} //namespace SynGlypX