#include "doubleminmaxwidget.h"

namespace SynGlyphX {

	DoubleMinMaxWidget::DoubleMinMaxWidget(QWidget *parent)
		: HorizontalFormWidget(parent)
	{
		m_minSpinBox = new QDoubleSpinBox(this);
		AddWidget(tr("Min:"), m_minSpinBox);
		QObject::connect(m_minSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &DoubleMinMaxWidget::OnUserUpdated);

		m_maxSpinBox = new QDoubleSpinBox(this);
		AddWidget(tr("Max:"), m_maxSpinBox);
		QObject::connect(m_maxSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &DoubleMinMaxWidget::OnUserUpdated);
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

		m_minSpinBox->blockSignals(true);
		m_maxSpinBox->blockSignals(true);

		m_minSpinBox->setValue(minDiff.GetMin());
		m_maxSpinBox->setValue(minDiff.GetMax());

		m_minSpinBox->blockSignals(false);
		m_maxSpinBox->blockSignals(false);
	}

	void DoubleMinMaxWidget::SetKeyboardTracking(bool enable) {

		m_minSpinBox->setKeyboardTracking(enable);
		m_maxSpinBox->setKeyboardTracking(enable);
	}

	void DoubleMinMaxWidget::SetRange(double min, double max) {

		m_minSpinBox->setRange(min, max);
		m_maxSpinBox->setRange(min, max);
	}

	void DoubleMinMaxWidget::OnUserUpdated() {

		emit ValueChanged(GetValue());
	}

} //namespace SynGlypX