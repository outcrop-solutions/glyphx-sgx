#include "zspaceoptionswidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>

namespace SynGlyphX {

	ZSpaceOptionsWidget::ZSpaceOptionsWidget(QWidget *parent)
		: QWidget(parent)
	{
		QVBoxLayout* mainLayout = new QVBoxLayout(this);

		QGroupBox* optionsGroupBox = new QGroupBox(tr("zSpace Options"), this);
		QVBoxLayout* optionsGroupBoxLayout = new QVBoxLayout(optionsGroupBox);
		
		QHBoxLayout* stylusLayout = new QHBoxLayout(optionsGroupBox);
		QLabel* stylusColorLabel = new QLabel(tr("Stylus Color:"), optionsGroupBox);
		stylusLayout->addWidget(stylusColorLabel);
		m_stylusColorButton = new SynGlyphX::ColorButton(false, optionsGroupBox);
		stylusLayout->addWidget(m_stylusColorButton);
		QLabel* stylusLengthLabel = new QLabel(tr("Stylus Length:"), optionsGroupBox);
		stylusLayout->addWidget(stylusLengthLabel);
		m_stylusLengthSpinBox = new QDoubleSpinBox(optionsGroupBox);
		m_stylusLengthSpinBox->setMinimum(0.05);
		m_stylusLengthSpinBox->setMaximum(1.0);
		m_stylusLengthSpinBox->setSingleStep(0.01);
		stylusLayout->addWidget(m_stylusLengthSpinBox);;

		optionsGroupBoxLayout->addLayout(stylusLayout);
		//stylusLayout->addStretch(1);

		QHBoxLayout* buttonLayout = new QHBoxLayout(this);

		QPushButton* defaultsButton = new QPushButton(tr("Set To Defaults"), optionsGroupBox);
		QObject::connect(defaultsButton, &QPushButton::clicked, this, &ZSpaceOptionsWidget::OnSetDefaults);
		buttonLayout->addWidget(defaultsButton);

		buttonLayout->addStretch(1);

		optionsGroupBoxLayout->addLayout(buttonLayout);

		optionsGroupBox->setLayout(optionsGroupBoxLayout);
		mainLayout->addWidget(optionsGroupBox);

		mainLayout->addStretch(1);

		setLayout(mainLayout);
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

} //namespace SynGlyphX