#include "singlenumericrangefilterwidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

namespace SynGlyphX {

	SingleNumericRangeFilterWidget::SingleNumericRangeFilterWidget(Qt::Orientation orientation, QWidget *parent)
		: QWidget(parent)
	{
		QBoxLayout* mainLayout = nullptr;
		if (orientation == Qt::Horizontal) {

			mainLayout = new QHBoxLayout(this);
		}
		else {

			mainLayout = new QVBoxLayout(this);
		}

		m_minSpinBox = new QDoubleSpinBox(this);
		m_minSpinBox->setDecimals(3);
		m_minSpinBox->setKeyboardTracking(false);
		mainLayout->addWidget(m_minSpinBox);

		m_rangeSlider = new RangeSlider(orientation, this);
		m_rangeSlider->setRange(0, 99);
		m_rangeSlider->setTracking(false);
		mainLayout->addWidget(m_rangeSlider);

		m_maxSpinBox = new QDoubleSpinBox(this);
		m_maxSpinBox->setDecimals(3);
		m_maxSpinBox->setKeyboardTracking(false);
		mainLayout->addWidget(m_maxSpinBox);

		setLayout(mainLayout);

		QObject::connect(m_rangeSlider, &RangeSlider::LowerPositionChanged, this, &SingleNumericRangeFilterWidget::OnSliderLowerUpdated);
		QObject::connect(m_rangeSlider, &RangeSlider::UpperPositionChanged, this, &SingleNumericRangeFilterWidget::OnSliderUpperUpdated);

		QObject::connect(m_rangeSlider, &RangeSlider::LowerValueChanged, this, &SingleNumericRangeFilterWidget::EmitRangeUpdate);
		QObject::connect(m_rangeSlider, &RangeSlider::UpperValueChanged, this, &SingleNumericRangeFilterWidget::EmitRangeUpdate);

		QObject::connect(m_minSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &SingleNumericRangeFilterWidget::OnMinSpinBoxUpdated);
		QObject::connect(m_maxSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &SingleNumericRangeFilterWidget::OnMaxSpinBoxUpdated);

		SetMinMax(0.0, 99.0);
	}

	SingleNumericRangeFilterWidget::~SingleNumericRangeFilterWidget()
	{

	}

	void SingleNumericRangeFilterWidget::SetMinMax(double min, double max) {

		m_minSpinBox->blockSignals(true);
		m_maxSpinBox->blockSignals(true);
		m_rangeSlider->blockSignals(true);
		
		m_minSpinBox->setRange(min, max);
		m_minSpinBox->setValue(min);
		m_maxSpinBox->setRange(min, max);
		m_maxSpinBox->setValue(max);
		m_rangeSlider->SetLowerValue(m_rangeSlider->minimum());
		m_rangeSlider->SetUpperValue(m_rangeSlider->maximum());

		m_minSpinBox->blockSignals(false);
		m_maxSpinBox->blockSignals(false);
		m_rangeSlider->blockSignals(false);

		emit RangeUpdated(min, max);
	}

	void SingleNumericRangeFilterWidget::OnSliderLowerUpdated(int lower) {

		m_minSpinBox->blockSignals(true);
		m_minSpinBox->setValue(lower);
		m_minSpinBox->blockSignals(false);
	}

	void SingleNumericRangeFilterWidget::OnSliderUpperUpdated(int upper) {

		m_maxSpinBox->blockSignals(true);
		m_maxSpinBox->setValue(upper);
		m_maxSpinBox->blockSignals(false);
	}

	void SingleNumericRangeFilterWidget::OnMinSpinBoxUpdated(double min) {

		m_rangeSlider->blockSignals(true);
		m_rangeSlider->SetLowerValue(static_cast<int>(min));
		m_rangeSlider->blockSignals(false);

		EmitRangeUpdate();
	}

	void SingleNumericRangeFilterWidget::OnMaxSpinBoxUpdated(double max) {

		m_rangeSlider->blockSignals(true);
		m_rangeSlider->SetUpperValue(static_cast<int>(max));
		m_rangeSlider->blockSignals(false);

		EmitRangeUpdate();
	}

	void SingleNumericRangeFilterWidget::EmitRangeUpdate() {

		emit RangeUpdated(m_minSpinBox->value(), m_maxSpinBox->value());
	}

} //namespace SynGlyphX