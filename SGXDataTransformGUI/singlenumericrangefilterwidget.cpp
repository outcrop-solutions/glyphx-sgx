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
		m_rangeSlider->setTracking(true);
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

		SetMaxRangeExtents(0.0, 99.0);
	}

	SingleNumericRangeFilterWidget::~SingleNumericRangeFilterWidget()
	{

	}

	void SingleNumericRangeFilterWidget::SetMaxRangeExtents(double min, double max) {

		bool wasRangeUpdated = false;

		m_minSpinBox->blockSignals(true);
		m_maxSpinBox->blockSignals(true);
		m_rangeSlider->blockSignals(true);

		if ((m_minSpinBox->value() < min) || (m_maxSpinBox->value() > max)) {

			wasRangeUpdated = true;
		}
		
		m_minSpinBox->setRange(min, max);
		m_maxSpinBox->setRange(min, max);

		m_ratio = (max - min) / (m_rangeSlider->maximum() - m_rangeSlider->minimum());

		m_rangeSlider->SetLowerValue(ConvertValueInRangeToSliderPosition(m_minSpinBox->value()));
		m_rangeSlider->SetUpperValue(ConvertValueInRangeToSliderPosition(m_maxSpinBox->value()));

		m_minSpinBox->blockSignals(false);
		m_maxSpinBox->blockSignals(false);
		m_rangeSlider->blockSignals(false);

		if (wasRangeUpdated) {

			EmitRangeUpdate();
		}
	}

	void SingleNumericRangeFilterWidget::SetRange(const std::pair<double, double> range) {

		blockSignals(true);
		m_minSpinBox->setValue(range.first);
		m_maxSpinBox->setValue(range.second);
		blockSignals(false);

		EmitRangeUpdate();
	}

	std::pair<double, double> SingleNumericRangeFilterWidget::GetRange() const {

		return (std::pair<double, double>(m_minSpinBox->value(), m_maxSpinBox->value()));
	}

	void SingleNumericRangeFilterWidget::OnSliderLowerUpdated(int lower) {

		SetMinSpinBoxBlockSignals(ConvertSliderPositionToValueInRange(lower));
	}

	void SingleNumericRangeFilterWidget::OnSliderUpperUpdated(int upper) {

		SetMaxSpinBoxBlockSignals(ConvertSliderPositionToValueInRange(upper));
	}

	void SingleNumericRangeFilterWidget::OnMinSpinBoxUpdated(double min) {

		m_rangeSlider->blockSignals(true);
		m_rangeSlider->SetLowerValue(ConvertValueInRangeToSliderPosition(min));
		m_rangeSlider->blockSignals(false);

		if (min > m_maxSpinBox->value()) {

			SetMaxSpinBoxBlockSignals(min);
		}

		EmitRangeUpdate();
	}

	void SingleNumericRangeFilterWidget::OnMaxSpinBoxUpdated(double max) {

		m_rangeSlider->blockSignals(true);
		m_rangeSlider->SetUpperValue(ConvertValueInRangeToSliderPosition(max));
		m_rangeSlider->blockSignals(false);

		if (max < m_minSpinBox->value()) {

			SetMinSpinBoxBlockSignals(max);
		}

		EmitRangeUpdate();
	}

	void SingleNumericRangeFilterWidget::EmitRangeUpdate() {

		emit RangeUpdated(m_minSpinBox->value(), m_maxSpinBox->value());
	}

	double SingleNumericRangeFilterWidget::ConvertSliderPositionToValueInRange(int value) {

		return ((m_ratio * value) + m_minSpinBox->minimum());
	}

	int SingleNumericRangeFilterWidget::ConvertValueInRangeToSliderPosition(double value) {

		return static_cast<int>(std::floor((value - m_minSpinBox->minimum()) / m_ratio));
	}

	void SingleNumericRangeFilterWidget::SetMinSpinBoxBlockSignals(double min) {

		m_minSpinBox->blockSignals(true);
		m_minSpinBox->setValue(min);
		m_minSpinBox->blockSignals(false);
	}

	void SingleNumericRangeFilterWidget::SetMaxSpinBoxBlockSignals(double max) {

		m_maxSpinBox->blockSignals(true);
		m_maxSpinBox->setValue(max);
		m_maxSpinBox->blockSignals(false);
	}

} //namespace SynGlyphX