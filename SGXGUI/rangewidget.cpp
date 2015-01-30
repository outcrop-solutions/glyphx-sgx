#include "rangewidget.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <utility>

namespace SynGlyphX {

	RangeWidget::RangeWidget(QWidget *parent)
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
		layout->addWidget(m_minSpinBox);
		
		QLabel* labelMax = new QLabel(tr("Max:"), this);
		layout->addWidget(labelMax);

		m_maxSpinBox = new QDoubleSpinBox(this);
		m_maxSpinBox->setKeyboardTracking(false);
		m_maxSpinBox->setDecimals(3);
		m_maxSpinBox->setMaximum(1000000000.0);
		layout->addWidget(m_maxSpinBox);

		setLayout(layout);

		//Make sure spin boxes have different values
		m_minSpinBox->setValue(0.0);
		m_maxSpinBox->setValue(1.0);
	}

	RangeWidget::~RangeWidget()
	{

	}

	void RangeWidget::SetRange(const Range& range) {

		m_minSpinBox->setValue(range.GetMin());
		m_maxSpinBox->setValue(range.GetMax());
	}

	Range RangeWidget::GetRange() const {

		Range range(m_minSpinBox->value(), m_maxSpinBox->value());
		return range;
	}

	void RangeWidget::OnMinChanged() {

		m_maxSpinBox->setMinimum(m_minSpinBox->value() + 0.1);
	}

	void RangeWidget::OnMaxChanged() {

		m_minSpinBox->setMaximum(m_maxSpinBox->value() - 0.1);
	}

} //namespace SynGlyphX