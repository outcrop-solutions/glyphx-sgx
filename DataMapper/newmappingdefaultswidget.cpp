#include "newmappingdefaultswidget.h"
#include <QtWidgets/QVBoxLayout>
#include "groupboxsinglewidget.h"

NewMappingDefaultsWidget::NewMappingDefaultsWidget(QWidget *parent)
	: QWidget(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	m_glyphDefaultsWidget = new SynGlyphX::GlyphDefaultsWidget(this);
	SynGlyphX::GroupBoxSingleWidget* glyphDefaultsGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Glyph"), m_glyphDefaultsWidget, this);
	mainLayout->addWidget(glyphDefaultsGroupBox);
	
	m_scenePropertiesWidget = new SynGlyphX::ScenePropertiesWidget(this);
	SynGlyphX::GroupBoxSingleWidget* scenePropertiesGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Scene Properties"), m_scenePropertiesWidget, this);
	mainLayout->addWidget(scenePropertiesGroupBox);
	
	setLayout(mainLayout);
}

NewMappingDefaultsWidget::~NewMappingDefaultsWidget()
{

}

void NewMappingDefaultsWidget::SetDefaults(const SynGlyphX::DataMappingDefaults& dataMappingDefaults, const SynGlyphX::SceneProperties& scenePropertiesDefaults) {

	m_glyphDefaultsWidget->SetDefaults(dataMappingDefaults);
	m_scenePropertiesWidget->SetWidgetFromProperties(scenePropertiesDefaults);
}

SynGlyphX::DataMappingDefaults NewMappingDefaultsWidget::GetDataMappingDefaults() const {

	return m_glyphDefaultsWidget->GetDefaults();
}

SynGlyphX::SceneProperties NewMappingDefaultsWidget::GetScenePropertiesDefaults() const {

	return m_scenePropertiesWidget->GetPropertiesFromWidget();
}