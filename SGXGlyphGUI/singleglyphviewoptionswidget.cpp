#include "singleglyphviewoptionswidget.h"
#include <QtWidgets/QFormLayout>

namespace SynGlyphX {

	SingleGlyphViewOptionsWidget::SingleGlyphViewOptionsWidget(QWidget *parent)
		: QWidget(parent)
	{
		m_defaultBaseImagesComboBox = new DefaultBaseImagesComboBox(this);

		QFormLayout* mainLayout = new QFormLayout(this);
		mainLayout->addRow(tr("Base Image In 3D View:"), m_defaultBaseImagesComboBox);

		setLayout(mainLayout);
	}

	SingleGlyphViewOptionsWidget::~SingleGlyphViewOptionsWidget()
	{

	}

	void SingleGlyphViewOptionsWidget::SetDefaultBaseImage(DefaultBaseImageProperties::Type defaultBaseImage) {

		m_defaultBaseImagesComboBox->SetDefaultBaseImage(defaultBaseImage);
	}

	DefaultBaseImageProperties::Type SingleGlyphViewOptionsWidget::GetDefaultBaseImage() const {

		return m_defaultBaseImagesComboBox->GetDefaultBaseImage();
	}

}