#include "zspaceoptionswidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

ZSpaceOptionsWidget::ZSpaceOptionsWidget(QWidget *parent)
	: QWidget(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	QHBoxLayout* stylusLayout = new QHBoxLayout(this);

	QGroupBox* stylusGroupBox = new QGroupBox(tr("Stylus"), this);

	QHBoxLayout* stylusGroupBoxLayout = new QHBoxLayout(stylusGroupBox);
	QLabel* stylusColorLabel = new QLabel(tr("Color:"), stylusGroupBox);
	stylusGroupBoxLayout->addWidget(stylusColorLabel);
	m_stylusColorButton = new SynGlyphX::ColorButton(false, stylusGroupBox);
	stylusGroupBoxLayout->addWidget(m_stylusColorButton);
	QLabel* stylusLengthLabel = new QLabel(tr("Length:"), stylusGroupBox);
	stylusGroupBoxLayout->addWidget(stylusLengthLabel);
	m_stylusLengthSpinBox = new QDoubleSpinBox(stylusGroupBox);
	m_stylusLengthSpinBox->setMinimum(0.05);
	m_stylusLengthSpinBox->setMaximum(1.0);
	m_stylusLengthSpinBox->setSingleStep(0.01);
	stylusGroupBoxLayout->addWidget(m_stylusLengthSpinBox);
	stylusGroupBox->setLayout(stylusLayout);

	stylusLayout->addWidget(stylusGroupBox);
	stylusLayout->addStretch(1);

	layout->addLayout(stylusLayout);

	QHBoxLayout* buttonLayout = new QHBoxLayout(this);

	QPushButton* defaultsButton = new QPushButton(tr("Set To Defaults"), this);
	QObject::connect(defaultsButton, &QPushButton::clicked, this, &ZSpaceOptionsWidget::OnSetDefaults);
	buttonLayout->addWidget(defaultsButton);

	buttonLayout->addStretch(1);

	layout->addLayout(buttonLayout);

	layout->addStretch(1);

	setLayout(layout);
}

ZSpaceOptionsWidget::~ZSpaceOptionsWidget()
{

}

void ZSpaceOptionsWidget::SetOptions(const ZSpaceOptions& options) {

	m_stylusColorButton->SetColor(options.GetStylusColor());
	m_stylusLengthSpinBox->setValue(options.GetStylusLength());
}

ZSpaceOptions ZSpaceOptionsWidget::GetOptions() const {

	ZSpaceOptions options;
	options.SetStylusColor(m_stylusColorButton->GetColor());
	options.SetStylusLength(m_stylusLengthSpinBox->value());
	return options;
}

void ZSpaceOptionsWidget::OnSetDefaults() {

	ZSpaceOptions options;
	SetOptions(options);
}