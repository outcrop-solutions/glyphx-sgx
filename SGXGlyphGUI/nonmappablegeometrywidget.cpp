#include "nonmappablegeometrywidget.h"
#include <QtWidgets/QHBoxLayout>

namespace SynGlyphX {

	NonMappableGeometryWidget::NonMappableGeometryWidget(QWidget *parent)
		: QWidget(parent)
	{
		QHBoxLayout* mainLayout = new QHBoxLayout(this);
		mainLayout->setContentsMargins(0, 0, 0, 0);

		m_surfaceRadioButtonGroup = new SurfaceRadioButtonWidget(Qt::Horizontal, this);
		QGroupBox* surfaceGroupBox = new GroupBoxSingleWidget(tr("Surface"), m_surfaceRadioButtonGroup, this);

		mainLayout->addWidget(surfaceGroupBox);

		m_ratioSpinBox = new QDoubleSpinBox(this);
		m_ratioSpinBox->setSingleStep(0.05);
		m_ratioSpinBox->setDecimals(2);
		m_ratioGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Torus Ratio"), m_ratioSpinBox, this);

		mainLayout->addWidget(m_ratioGroupBox);
		
		setLayout(mainLayout);

		QObject::connect(m_surfaceRadioButtonGroup, &SynGlyphX::SurfaceRadioButtonWidget::ButtonClicked, this, &NonMappableGeometryWidget::PropertiesChanged);
		QObject::connect(m_ratioSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &NonMappableGeometryWidget::PropertiesChanged);
	}

	NonMappableGeometryWidget::~NonMappableGeometryWidget()
	{

	}

	void NonMappableGeometryWidget::SetProperties(const NonMappableGeometryProperties& properties) {

		m_surfaceRadioButtonGroup->SetSurface(properties.GetSurface());
		m_ratioSpinBox->setValue(properties.GetTorusRatio());
	}

	NonMappableGeometryProperties NonMappableGeometryWidget::GetProperties() const {

		NonMappableGeometryProperties properties;
		properties.SetSurface(m_surfaceRadioButtonGroup->GetSurface());
		properties.SetTorusRatio(m_ratioSpinBox->value());
		return properties;
	}

	void NonMappableGeometryWidget::ShowTorusRatioWidget(bool show) {

		m_ratioGroupBox->setVisible(show);
	}

} //namespace SynGlyphX