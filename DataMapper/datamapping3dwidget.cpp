#include "datamapping3dwidget.h"
#include <stack>

DataMapping3DWidget::DataMapping3DWidget(DataTransformModel* model, QWidget *parent)
	: MinMaxGlyph3DWidget(parent),
	m_internalModel(nullptr),
	m_internalSelectionModel(nullptr),
	m_externalModel(nullptr),
	m_externalSelectionModel(nullptr),
	m_glyphTreeIndex(-1),
	m_dataTransformModel(model)
{
	m_internalModel = new MinMaxGlyphTreeModel(this);
	m_internalSelectionModel = new QItemSelectionModel(m_internalModel, this);

	MinMaxGlyph3DWidget::SetModel(m_internalModel, m_internalSelectionModel);
}

DataMapping3DWidget::~DataMapping3DWidget()
{

}

void DataMapping3DWidget::SetModel(SynGlyphX::RoleDataFilterProxyModel* model, QItemSelectionModel* selectionModel) {

	m_externalModel = model;
	m_externalSelectionModel = selectionModel;

	ConnectInternalSelection();
	ConnectExternalSelection();
}

void DataMapping3DWidget::ConnectInternalSelection() {

	m_internalSelectionConnection = QObject::connect(m_internalSelectionModel, &QItemSelectionModel::selectionChanged, this, &DataMapping3DWidget::OnInternalSelectionChanged);
}

void DataMapping3DWidget::ConnectExternalSelection() {

	m_externalSelectionConnection = QObject::connect(m_externalSelectionModel, &QItemSelectionModel::selectionChanged, this, &DataMapping3DWidget::OnExternalSelectionChanged);
}

void DataMapping3DWidget::OnInternalSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	if (selected.empty() || !selected.indexes()[0].isValid()) {

		m_externalSelectionModel->clear();
		return;
	}

	QModelIndex glyphTreeModelIndex = selected.indexes()[0];

	std::stack<int> childPositions;
	while (glyphTreeModelIndex.isValid()) {

		childPositions.push(glyphTreeModelIndex.row());
		glyphTreeModelIndex = glyphTreeModelIndex.parent();
	}

	//The top position will be 0 which we don't need
	childPositions.pop();

	QModelIndex minMaxIndex = m_externalModel->index(m_glyphTreeIndex, 0);

	while (!childPositions.empty()) {

		minMaxIndex = m_externalModel->index(childPositions.top(), 0, minMaxIndex);
		childPositions.pop();
	}

	QObject::disconnect(m_externalSelectionConnection);
	m_externalSelectionModel->select(minMaxIndex, QItemSelectionModel::ClearAndSelect);
	ConnectExternalSelection();
}

void DataMapping3DWidget::OnExternalSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	if (selected.empty() || !selected.indexes()[0].isValid() || (selected.indexes()[0].data(DataTransformModel::DataTypeRole) != DataTransformModel::DataType::GlyphTrees)) {

		m_glyphTreeIndex = -1;
		m_internalModel->removeRow(0);
		return;
	}

	QModelIndex minMaxIndex = selected.indexes()[0];

	std::stack<int> childPositions;
	while (minMaxIndex.isValid()) {

		childPositions.push(minMaxIndex.row());
		minMaxIndex = minMaxIndex.parent();
	}

	if (m_glyphTreeIndex != childPositions.top()) {

		m_glyphTreeIndex = childPositions.top();
		SynGlyphX::DataTransformMapping::MinMaxGlyphTreeMap::const_iterator glyphTree = m_dataTransformModel->GetDataMapping()->GetGlyphTrees().begin();
		std::advance(glyphTree, m_glyphTreeIndex);
		m_internalModel->SetMinMaxGlyphTree(glyphTree->second);
	}

	//The top position will be the row of the glyph since the glyph tree view has multiple glyphs.  We don't need it anymore
	childPositions.pop();

	QModelIndex glyphTreeModelIndex = m_internalModel->index(0);

	while (!childPositions.empty()) {

		glyphTreeModelIndex = m_internalModel->index(childPositions.top(), 0, glyphTreeModelIndex);
		childPositions.pop();
	}

	QObject::disconnect(m_internalSelectionConnection);
	m_internalSelectionModel->select(glyphTreeModelIndex, QItemSelectionModel::ClearAndSelect);
	ConnectInternalSelection();
}