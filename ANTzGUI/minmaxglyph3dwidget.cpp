#include "minmaxglyph3dwidget.h"
#include <QtWidgets/QVBoxLayout>

MinMaxGlyph3DWidget::MinMaxGlyph3DWidget(MinMaxGlyphTreeModel* model, QWidget *parent)
	: QWidget(parent),
	m_model(model)
{
	QObject::connect(m_model, &MinMaxGlyphTreeModel::modelReset, this, &MinMaxGlyph3DWidget::OnModelReset);
	m_selectionModel = new QItemSelectionModel(m_model, this);

	m_maxGlyphModel = new GlyphTreeModel(this);
	m_maxGlyphSelectionModel = new QItemSelectionModel(m_maxGlyphModel, this);
	m_maxGlyph3DWidget = new ANTzWidget(m_maxGlyphModel, m_maxGlyphSelectionModel, false, this);
	m_maxGlyph3DWidget->SetEditingMode(ANTzWidget::EditingMode::None);
	ConnectMaxViewSelection();

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(m_maxGlyph3DWidget);

	setLayout(layout);
}

MinMaxGlyph3DWidget::~MinMaxGlyph3DWidget()
{

}

void MinMaxGlyph3DWidget::ConnectMaxViewSelection() {

	m_maxViewSelectConnection = QObject::connect(m_maxGlyphSelectionModel, &QItemSelectionModel::selectionChanged, this, &MinMaxGlyph3DWidget::UpdateSelectionFromMaxView);
}

void MinMaxGlyph3DWidget::OnModelReset() {

	QObject::disconnect(m_maxViewSelectConnection);
	SynGlyphX::GlyphTree::SharedPtr maxGlyphTree = m_model->GetMinMaxGlyphTree()->GetMaxGlyphTree();
	maxGlyphTree->root()->SetPosition({ { 0.0, 0.0, 0.0 } });
	m_maxGlyphModel->CreateNewTree(maxGlyphTree, true);
	ConnectMaxViewSelection();
}

void MinMaxGlyph3DWidget::UpdateSelection(const QItemSelection& selected, const QItemSelection& deselected) {

	QObject::disconnect(m_maxViewSelectConnection);
	m_maxGlyphSelectionModel->select(selected, QItemSelectionModel::ClearAndSelect);
	ConnectMaxViewSelection();
}

void MinMaxGlyph3DWidget::UpdateSelectionFromMaxView(const QItemSelection& selected, const QItemSelection& deselected) {

	QObject::disconnect(m_maxViewSelectConnection);
	m_selectionModel->select(selected, QItemSelectionModel::ClearAndSelect);
	ConnectMaxViewSelection();
}