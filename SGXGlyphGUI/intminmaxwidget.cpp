#include "intminmaxwidget.h"

namespace SynGlyphX {

	IntMinMaxWidget::IntMinMaxWidget(QWidget *parent)
		: HorizontalFormWidget(parent)
	{
		m_minSpinBox = new QSpinBox(this);
		AddWidget(tr("Min:"), m_minSpinBox);
		QObject::connect(m_minSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &IntMinMaxWidget::OnUserUpdated);

		m_maxSpinBox = new QSpinBox(this);
		AddWidget(tr("Max:"), m_maxSpinBox);
		QObject::connect(m_maxSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &IntMinMaxWidget::OnUserUpdated);
	}

	IntMinMaxWidget::~IntMinMaxWidget()
	{

	}

	IntMinDiff IntMinMaxWidget::GetValue() const {

		IntMinDiff minDiff;
		minDiff.SetMinMax(m_minSpinBox->value(), m_maxSpinBox->value());
		return minDiff;
	}

	void IntMinMaxWidget::SetValue(const IntMinDiff& minDiff) {

		m_minSpinBox->blockSignals(true);
		m_maxSpinBox->blockSignals(true);

		m_minSpinBox->setValue(minDiff.GetMin());
		m_maxSpinBox->setValue(minDiff.GetMax());

		m_minSpinBox->blockSignals(false);
		m_maxSpinBox->blockSignals(false);
	}

	void IntMinMaxWidget::SetKeyboardTracking(bool enable) {

		m_minSpinBox->setKeyboardTracking(enable);
		m_maxSpinBox->setKeyboardTracking(enable);
	}

	void IntMinMaxWidget::SetRange(int min, int max) {

		m_minSpinBox->setRange(min, max);
		m_maxSpinBox->setRange(min, max);
	}

	void IntMinMaxWidget::OnUserUpdated() {

		emit ValueChanged(GetValue());
	}

} //namespace SynGlypX