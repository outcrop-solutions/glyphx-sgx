#include "minmaxglyph3dwidget.h"
#include <QtWidgets/QVBoxLayout>

MinMaxGlyph3DWidget::MinMaxGlyph3DWidget(MinMaxGlyphTreeModel* model, QItemSelectionModel* selectionModel, QWidget *parent)
	: QWidget(parent),
	m_model(model)
{
	m_selectionModel = new QItemSelectionModel(m_model, this);

	m_maxGlyph3DWidget = new ANTzSingleGlyphTreeWidget(MinMaxGlyphTreeModel::GlyphType::Max, this);
	m_maxGlyph3DWidget->SetEditingMode(ANTzSingleGlyphTreeWidget::EditingMode::None);
	m_maxGlyph3DWidget->SetModel(m_model, selectionModel);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_maxGlyph3DWidget);

	setLayout(layout);
}

MinMaxGlyph3DWidget::~MinMaxGlyph3DWidget()
{

}