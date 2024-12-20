#include "valuemappingrangewidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <utility>
#include <stdexcept>

namespace SynGlyphX {

	ValueMappingRangeWidget::ValueMappingRangeWidget(QWidget *parent)
		: QWidget(parent)
	{
		QHBoxLayout* layout = new QHBoxLayout(this);
		layout->setContentsMargins(0, 0, 0, 0);

		QLabel* labelMin = new QLabel(tr("Min:"), this);
		layout->addWidget(labelMin);

		m_minSpinBox = new QDoubleSpinBox(this);
		m_minSpinBox->setKeyboardTracking(false);
		m_minSpinBox->setDecimals(3);
		m_minSpinBox->setMinimum(-1000000000.0);
		m_minSpinBox->setMaximum(1000000000.0);
		layout->addWidget(m_minSpinBox);
		
		QLabel* labelMax = new QLabel(tr("Max:"), this);
		layout->addWidget(labelMax);

		m_maxSpinBox = new QDoubleSpinBox(this);
		m_maxSpinBox->setKeyboardTracking(false);
		m_maxSpinBox->setDecimals(3);
		m_maxSpinBox->setMinimum(-1000000000.0);
		m_maxSpinBox->setMaximum(1000000000.0);
		layout->addWidget(m_maxSpinBox);

		setLayout(layout);

		//QObject::connect(m_minSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &RangeWidget::OnMinChanged);
		//QObject::connect(m_maxSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &RangeWidget::OnMaxChanged);

		//Make sure spin boxes have different values
		m_minSpinBox->setValue(0.0);
		m_maxSpinBox->setValue(1.0);
	}

	ValueMappingRangeWidget::~ValueMappingRangeWidget()
	{

	}

	void ValueMappingRangeWidget::SetRange(const ValueMappingRange& range) {

		m_minSpinBox->setValue(range.GetMin());
		m_maxSpinBox->setValue(range.GetMax());
	}

	ValueMappingRange ValueMappingRangeWidget::GetRange() const {

		if (!IsValid()) {

			throw std::runtime_error("Min value of range must be less than max value of range.");
		}

		ValueMappingRange range(m_minSpinBox->value(), m_maxSpinBox->value());
		return range;
	}
	
	bool ValueMappingRangeWidget::IsValid() const {

		return (m_minSpinBox->value() < m_maxSpinBox->value());
	}

	/*
	void RangeWidget::OnMinChanged(double newMin) {

		m_maxSpinBox->setMinimum(newMin + 0.1);
	}

	void RangeWidget::OnMaxChanged(double newMax) {

		m_minSpinBox->setMaximum(newMax - 0.1);
	}
	*/
} //namespace SynGlyphX
