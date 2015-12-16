#include "intsizewidget.h"

namespace SynGlyphX {

	IntSizeWidget::IntSizeWidget(bool showLockRatioCheckBox, QWidget *parent)
		: SizeWidget(showLockRatioCheckBox, parent)
	{
		m_intWidthSpinBox = new QSpinBox(this);
		m_intHeightSpinBox = new QSpinBox(this);
		AddSpinBoxes(m_intWidthSpinBox, m_intHeightSpinBox);
	}

	IntSizeWidget::IntSizeWidget(bool showLockRatioCheckBox, const QString& widthLabel, const QString& heightLabel, QWidget *parent)
		: SizeWidget(showLockRatioCheckBox, parent) {

		m_intWidthSpinBox = new QSpinBox(this);
		m_intHeightSpinBox = new QSpinBox(this);
		AddSpinBoxes(m_intWidthSpinBox, m_intHeightSpinBox, widthLabel, heightLabel);
	}

	IntSizeWidget::~IntSizeWidget()
	{

	}

	void IntSizeWidget::SetSize(const QSize& size) {

		m_intWidthSpinBox->setValue(size.width());
		m_intHeightSpinBox->setValue(size.height());
	}

	QSize IntSizeWidget::GetSize() const {

		QSize size(m_intWidthSpinBox->value(), m_intHeightSpinBox->value());
		return size;
	}

	void IntSizeWidget::SetRange(int min, int max) {

		m_intWidthSpinBox->setRange(min, max);
		m_intHeightSpinBox->setRange(min, max);
	}

	QMetaObject::Connection IntSizeWidget::ConnectLockRatioToWidthSpinBox() {

		return QObject::connect(m_intWidthSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &IntSizeWidget::OnWidthChanged);
	}

	QMetaObject::Connection IntSizeWidget::ConnectLockRatioToHeightSpinBox() {

		return QObject::connect(m_intHeightSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &IntSizeWidget::OnHeightChanged);
	}

	double IntSizeWidget::GetRatio() {

		return static_cast<double>(m_intWidthSpinBox->value()) / m_intHeightSpinBox->value();
	}

	void IntSizeWidget::OnWidthChanged() {

		m_intHeightSpinBox->setValue(static_cast<int>(m_intWidthSpinBox->value() / m_ratio));
	}
	
	void IntSizeWidget::OnHeightChanged() {

		m_intWidthSpinBox->setValue(static_cast<int>(m_intHeightSpinBox->value() * m_ratio));
	}

	QSize IntSizeWidget::ConvertToQSize(const SynGlyphX::IntSize& size) {

		return QSize(size[0], size[1]);
	}

	SynGlyphX::IntSize IntSizeWidget::ConvertFromQSize(const QSize& size) {

		return SynGlyphX::IntSize({ { size.width(), size.height() } });
	}

} //namespace SynGlyphX