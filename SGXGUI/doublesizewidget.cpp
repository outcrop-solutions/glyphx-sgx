#include "doublesizewidget.h"

namespace SynGlyphX {

	DoubleSizeWidget::DoubleSizeWidget(bool showLockRatioCheckBox, QWidget *parent)
		: SizeWidget(showLockRatioCheckBox, parent)
	{
		m_doubleWidthSpinBox = new QDoubleSpinBox(this);
		m_doubleHeightSpinBox = new QDoubleSpinBox(this);
		AddSpinBoxes(m_doubleWidthSpinBox, m_doubleHeightSpinBox);
	}

	DoubleSizeWidget::~DoubleSizeWidget()
	{

	}

	void DoubleSizeWidget::SetSize(const QSizeF& size) {

		m_doubleWidthSpinBox->setValue(size.width());
		m_doubleHeightSpinBox->setValue(size.height());
	}

	QSizeF DoubleSizeWidget::GetSize() const {

		QSizeF size(m_doubleWidthSpinBox->value(), m_doubleHeightSpinBox->value());
		return size;
	}

	void DoubleSizeWidget::SetRange(double min, double max) {

		m_doubleWidthSpinBox->setRange(min, max);
		m_doubleHeightSpinBox->setRange(min, max);
	}

	QMetaObject::Connection DoubleSizeWidget::ConnectLockRatioToWidthSpinBox() {

		return QObject::connect(m_doubleWidthSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &DoubleSizeWidget::OnWidthChanged);
	}

	QMetaObject::Connection DoubleSizeWidget::ConnectLockRatioToHeightSpinBox() {

		return QObject::connect(m_doubleHeightSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &DoubleSizeWidget::OnHeightChanged);
	}

	double DoubleSizeWidget::GetRatio() {

		return m_doubleWidthSpinBox->value() / m_doubleHeightSpinBox->value();
	}

	void DoubleSizeWidget::OnWidthChanged() {

		m_doubleHeightSpinBox->setValue(m_doubleWidthSpinBox->value() / m_ratio);
	}

	void DoubleSizeWidget::OnHeightChanged() {

		m_doubleWidthSpinBox->setValue(m_doubleHeightSpinBox->value() * m_ratio);
	}

} //namespace SynGlyphX
