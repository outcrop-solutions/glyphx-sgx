#include "minmaxglyph3dwidget.h"
#include <QtWidgets/QVBoxLayout>

MinMaxGlyph3DWidget::MinMaxGlyph3DWidget(QWidget *parent)
	: QWidget(parent)
	//m_model(nullptr),
	//m_selectionModel(nullptr)
{
	m_maxGlyph3DWidget = new ANTzSingleGlyphTreeWidget(MinMaxGlyphTreeModel::GlyphType::Max, this);
	m_maxGlyph3DWidget->SetEditingMode(ANTzSingleGlyphTreeWidget::EditingMode::None);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(m_maxGlyph3DWidget);

	setLayout(layout);
}

MinMaxGlyph3DWidget::~MinMaxGlyph3DWidget()
{

}

void MinMaxGlyph3DWidget::SetModel(MinMaxGlyphTreeModel* model, QItemSelectionModel* selectionModel) {

	m_maxGlyph3DWidget->SetModel(model, selectionModel);
}