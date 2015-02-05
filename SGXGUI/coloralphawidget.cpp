#include "coloralphawidget.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>

namespace SynGlyphX {

	ColorAlphaWidget::ColorAlphaWidget(QWidget *parent)
		: QWidget(parent)
	{
		QHBoxLayout* layout = new QHBoxLayout(this);

		m_button = new ColorButton(true, this);

		QLabel* alphaLabel = new QLabel(tr("Alpha:"), this);

		m_alphaSpinBox = new QSpinBox(this);
		m_alphaSpinBox->setRange(0, 255);
		QObject::connect(m_alphaSpinBox, SIGNAL(valueChanged(int)), this, SLOT(OnAlphaSpinBoxChanged(int)));

		QObject::connect(m_button, &ColorButton::ColorChanged, this, static_cast<void (ColorAlphaWidget::*)(const QColor&)>(&ColorAlphaWidget::SetColor));

		layout->addWidget(m_button);
		layout->addWidget(alphaLabel);
		layout->addWidget(m_alphaSpinBox);

		setLayout(layout);

		SetColor(Qt::gray);
	}

	ColorAlphaWidget::~ColorAlphaWidget()
	{

	}

	void ColorAlphaWidget::SetColor(const QColor& color) {

		bool isButtonSender = (sender() == m_button);
		m_color = color;
		if (!isButtonSender) {
			m_button->SetColor(color);
		}
		m_alphaSpinBox->setValue(color.alpha());
		if (isButtonSender) {
			emit ColorChanged(m_color);
		}
	}

	void ColorAlphaWidget::OnAlphaSpinBoxChanged(int value) {

		m_color.setAlpha(value);
		m_button->SetColor(m_color);
		if (sender() == m_alphaSpinBox) {
			emit ColorChanged(m_color);
		}
	}

	const QColor& ColorAlphaWidget::GetColor() const {
		return m_color;
	}

} //namespace SynGlyphX