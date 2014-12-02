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
	QObject::connect(m_externalModel, &SynGlyphX::RoleDataFilterProxyModel::dataChanged, this, &DataMapping3DWidget::OnExternalDataChanged);
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

	std::stack<unsigned int> childPositions = GetRowIndiciesFromStack(selected.indexes()[0]);
	childPositions.pop();
	childPositions.push(m_glyphTreeIndex);

	QObject::disconnect(m_externalSelectionConnection);
	m_externalSelectionModel->select(GetModelIndexFromStack(childPositions, m_externalModel), QItemSelectionModel::ClearAndSelect);
	ConnectExternalSelection();
}

void DataMapping3DWidget::OnExternalSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	if (selected.empty() || !selected.indexes()[0].isValid() || (selected.indexes()[0].data(DataTransformModel::DataTypeRole) != DataTransformModel::DataType::GlyphTrees)) {

		m_glyphTreeIndex = -1;
		m_internalModel->removeRow(0);
		return;
	}

	std::stack<unsigned int> childPositions = GetRowIndiciesFromStack(selected.indexes()[0]);

	if (m_glyphTreeIndex != childPositions.top()) {

		m_glyphTreeIndex = childPositions.top();
		SynGlyphX::DataTransformMapping::MinMaxGlyphTreeMap::const_iterator glyphTree = m_dataTransformModel->GetDataMapping()->GetGlyphTrees().begin();
		std::advance(glyphTree, m_glyphTreeIndex);
		m_internalModel->SetMinMaxGlyphTree(glyphTree->second);
	}

	//The top position will be the row of the glyph since the glyph tree view has multiple glyphs.  We don't need it anymore
	childPositions.pop();
	childPositions.push(0);

	QObject::disconnect(m_internalSelectionConnection);
	m_internalSelectionModel->select(GetModelIndexFromStack(childPositions, m_internalModel), QItemSelectionModel::ClearAndSelect);
	ConnectInternalSelection();
}

void DataMapping3DWidget::OnExternalDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {

	QModelIndex topLeftSource = m_externalModel->mapToSource(topLeft);
	QModelIndex bottomRightSource = m_externalModel->mapToSource(bottomRight);

	std::stack<unsigned int> childPositions = GetRowIndiciesFromStack(topLeftSource.parent());
	if (!childPositions.empty()) {

		childPositions.pop();
		childPositions.push(0);
	}
	QModelIndex parent = GetModelIndexFromStack(childPositions, m_internalModel);

	if (parent.isValid()) {

		for (int i = topLeftSource.column(); i <= bottomRightSource.column(); ++i) {

			for (int j = topLeftSource.row(); j <= bottomRightSource.row(); ++j) {

				m_internalModel->UpdateGlyph(m_internalModel->index(j, i, parent), m_dataTransformModel->GetGlyph(m_dataTransformModel->index(j, i, topLeftSource.parent())));
			}
		}
	}
	else {

		m_internalModel->UpdateGlyph(m_internalModel->index(0), m_dataTransformModel->GetGlyph(m_dataTransformModel->index(topLeftSource.row(), topLeftSource.column(), topLeftSource.parent())));
	}
}

std::stack<unsigned int> DataMapping3DWidget::GetRowIndiciesFromStack(const QModelIndex& index) const {

	QModelIndex currentIndex = index;
	std::stack<unsigned int> childPositions;
	while (currentIndex.isValid()) {

		childPositions.push(currentIndex.row());
		currentIndex = currentIndex.parent();
	}

	return childPositions;
}

QModelIndex DataMapping3DWidget::GetModelIndexFromStack(std::stack<unsigned int> rows, const QAbstractItemModel* const model) const {

	QModelIndex index;

	while (!rows.empty()) {

		index = model->index(rows.top(), 0, index);
		rows.pop();
	}

	return index;
}