#include "scenepropertieswidget.h"
#include <QtWidgets/QFormLayout>
#include "colorconverter.h"
#include "colorbutton.h"
#include "sceneproperties.h"

namespace SynGlyphX {

	ScenePropertiesWidget::ScenePropertiesWidget(QWidget *parent)
		: QWidget(parent)
	{
		QFormLayout* mainLayout = new QFormLayout(this);

		m_backgroundColorButton = new ColorButton(false, this);
		mainLayout->addRow(tr("Background Color") + ":", m_backgroundColorButton);

		setLayout(mainLayout);
	}

	ScenePropertiesWidget::~ScenePropertiesWidget()
	{

	}

	void ScenePropertiesWidget::SetWidgetFromProperties(const SceneProperties& properties) {

		m_backgroundColorButton->SetColor(ColorConverter::GlyphColor2QColor(properties.GetBackgroundColor()));
	}

	SceneProperties ScenePropertiesWidget::GetPropertiesFromWidget() const {

		SceneProperties properties;
		properties.SetBackgroundColor(ColorConverter::QColor2GlyphColor(m_backgroundColorButton->GetColor()));

		return properties;
	}

} //namespace SynGlyphX