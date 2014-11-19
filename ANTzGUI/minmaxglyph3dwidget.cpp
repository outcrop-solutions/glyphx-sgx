#include "minmaxglyph3dwidget.h"
#include <QtWidgets/QVBoxLayout>

MinMaxGlyph3DWidget::MinMaxGlyph3DWidget(MinMaxGlyphTreeModel* model, QWidget *parent)
	: QWidget(parent),
	m_model(model)
{
	m_selectionModel = new QItemSelectionModel(m_model, this);

	m_maxGlyph3DWidget = new ANTzSingleGlyphTreeWidget(ANTzSingleGlyphTreeWidget::MinMaxGlyphTreeType::Max, this);
	m_maxGlyph3DWidget->SetEditingMode(ANTzSingleGlyphTreeWidget::EditingMode::None);
	m_maxGlyph3DWidget->SetModel(m_model);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_maxGlyph3DWidget);

	setLayout(layout);
}

MinMaxGlyph3DWidget::~MinMaxGlyph3DWidget()
{

}