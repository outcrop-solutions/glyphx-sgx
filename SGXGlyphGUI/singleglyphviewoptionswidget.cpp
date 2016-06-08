#include "singleglyphviewoptionswidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QCheckBox>
#include "defaultbaseimagescombobox.h"

namespace SynGlyphX {

	SingleGlyphViewOptionsWidget::SingleGlyphViewOptionsWidget(QWidget *parent)
		: QWidget(parent)
	{
		m_defaultBaseImagesComboBox = new DefaultBaseImagesComboBox(this);

		QVBoxLayout* mainLayout = new QVBoxLayout(this);

		m_zIsAlwaysZeroIn3DCheckBox = new QCheckBox(tr("Keep Root Glyph's Z Position At Zero in 3D View"), this);
		m_zIsAlwaysZeroIn3DCheckBox->setChecked(true);

		mainLayout->addWidget(m_zIsAlwaysZeroIn3DCheckBox);

		QFormLayout* formLayout = new QFormLayout(this);
		formLayout->setContentsMargins(0, 0, 0, 0);

		formLayout->addRow(tr("Base Image In 3D View:"), m_defaultBaseImagesComboBox);

		mainLayout->addLayout(formLayout);

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

	void SingleGlyphViewOptionsWidget::SetZToAlwaysZeroIn3D(bool setToZero) {

		m_zIsAlwaysZeroIn3DCheckBox->setChecked(setToZero);
	}

	bool SingleGlyphViewOptionsWidget::IsZAlwaysZeroIn3D() const {

		return m_zIsAlwaysZeroIn3DCheckBox->isChecked();
	}
}