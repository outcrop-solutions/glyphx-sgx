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
		mainLayout->addWidget(m_minSpinBox);

		m_rangeSlider = new RangeSlider(orientation, this);
		m_rangeSlider->setRange(0, 99);
		mainLayout->addWidget(m_rangeSlider);

		m_maxSpinBox = new QDoubleSpinBox(this);
		m_maxSpinBox->setDecimals(3);
		mainLayout->addWidget(m_maxSpinBox);

		setLayout(mainLayout);
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
		m_rangeSlider->SetLowerPosition(m_rangeSlider->minimum());
		m_rangeSlider->SetUpperPosition(m_rangeSlider->maximum());

		m_minSpinBox->blockSignals(false);
		m_maxSpinBox->blockSignals(false);
		m_rangeSlider->blockSignals(false);

		emit RangeUpdated(min, max);
	}

	void SingleNumericRangeFilterWidget::OnSliderLowerUpdated(int lower) {


	}

	void SingleNumericRangeFilterWidget::OnSliderUpperUpdated(int upper) {


	}

	void SingleNumericRangeFilterWidget::OnMinSpinBoxUpdated(double min) {


	}

	void SingleNumericRangeFilterWidget::OnMaxSpinBoxUpdated(double max) {


	}

} //namespace SynGlyphX