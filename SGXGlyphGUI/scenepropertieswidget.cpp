#include "scenepropertieswidget.h"
#include <QtWidgets/QFormLayout>
#include "colorconverter.h"

namespace SynGlyphX {

	ScenePropertiesWidget::ScenePropertiesWidget(QWidget *parent)
		: QWidget(parent)
	{
		QFormLayout* mainLayout = new QFormLayout(this);

		m_backgroundColorButton = new ColorButton(false, this);
		mainLayout->addRow(tr("Background Color") + ":", m_backgroundColorButton);

		m_legendLineEdit = new BrowseLineEdit(BrowseLineEdit::FileDialogType::FileOpen, true, this);
		mainLayout->addRow(tr("Legend") + ":", m_legendLineEdit);

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
		properties.SetLegend(m_legendLineEdit->GetText().toStdWString());

		return properties;
	}

} //namespace SynGlyphX