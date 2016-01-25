#include "glyphoptionswidget.h"
#include <QtWidgets/QVBoxLayout>

GlyphOptionsWidget::GlyphOptionsWidget(QWidget *parent)
	: QWidget(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	mainLayout->addStretch(1);

	m_mergeRootsCheckBox = new QCheckBox(tr("Merge Roots"), this);
	mainLayout->addWidget(m_mergeRootsCheckBox);

	mainLayout->addStretch(1);

	setLayout(mainLayout);
}

GlyphOptionsWidget::~GlyphOptionsWidget()
{

}

bool GlyphOptionsWidget::GetMergeRoots() const {

	return m_mergeRootsCheckBox->isChecked();
}

void GlyphOptionsWidget::SetMergeRoots(bool mergeRoots) {

	m_mergeRootsCheckBox->setChecked(mergeRoots);
}
