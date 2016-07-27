#include "datamapping3dwidget.h"
#include <stack>
#include "roledatafilterproxymodel.h"
#include "datatransformmodel.h"

DataMapping3DWidget::DataMapping3DWidget(SynGlyphX::DataTransformModel* model, QWidget *parent)
	: SynGlyphXANTz::MinMaxGlyph3DWidget(parent),
	m_internalModel(nullptr),
	m_internalSelectionModel(nullptr),
	m_externalModel(nullptr),
	m_externalSelectionModel(nullptr),
	m_glyphTreeIndex(-1),
	m_dataTransformModel(model)
{
	m_internalModel = new SynGlyphXANTz::MinMaxGlyphTreeModel(this);
	m_internalSelectionModel = new QItemSelectionModel(m_internalModel, this);

	SynGlyphXANTz::MinMaxGlyph3DWidget::SetModel(m_internalModel, m_internalSelectionModel);
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
	QObject::connect(m_externalModel, &SynGlyphX::RoleDataFilterProxyModel::modelReset, this, &DataMapping3DWidget::OnExternalModelReset);
	QObject::connect(m_externalModel, &SynGlyphX::RoleDataFilterProxyModel::rowsRemoved, this, &DataMapping3DWidget::OnExternalRowsRemoved);
	QObject::connect(m_externalModel, &SynGlyphX::RoleDataFilterProxyModel::rowsInserted, this, &DataMapping3DWidget::OnExternalRowsInserted);
}

void DataMapping3DWidget::ConnectInternalSelection() {

	m_internalSelectionConnection = QObject::connect(m_internalSelectionModel, &QItemSelectionModel::selectionChanged, this, &DataMapping3DWidget::OnInternalSelectionChanged);
}

void DataMapping3DWidget::ConnectExternalSelection() {

	m_externalSelectionConnection = QObject::connect(m_externalSelectionModel, &QItemSelectionModel::selectionChanged, this, &DataMapping3DWidget::OnExternalSelectionChanged);
}

void DataMapping3DWidget::OnInternalSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	if (m_internalSelectionModel->selection().empty()) {

		m_externalSelectionModel->clear();
		return;
	}

	QItemSelection newSelection;
	for (const QModelIndex& index : m_internalSelectionModel->selectedIndexes()) {

		std::stack<unsigned int> childPositions = GetRowIndiciesFromStack(index);
		childPositions.pop();
		childPositions.push(m_glyphTreeIndex);

		QModelIndex externalIndex = GetModelIndexFromStack(childPositions, m_externalModel);
		newSelection.select(externalIndex, externalIndex);
	}

	QObject::disconnect(m_externalSelectionConnection);
	m_externalSelectionModel->select(newSelection, QItemSelectionModel::ClearAndSelect);
	ConnectExternalSelection();
}

void DataMapping3DWidget::OnExternalSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	QModelIndexList selectedIndexes = GetValidExternalIndexes(m_externalSelectionModel->selectedIndexes());
	if (selectedIndexes.empty()) {

		Clear();
		return;
	}

	QItemSelection newSelection;
	for (const QModelIndex& index : selectedIndexes) {

		std::stack<unsigned int> childPositions = GetRowIndiciesFromStack(index);

		if (m_glyphTreeIndex != childPositions.top()) {

			m_glyphTreeIndex = childPositions.top();
			SynGlyphX::DataTransformMapping::DataMappingGlyphGraphMap::const_iterator glyphTree = m_dataTransformModel->GetDataMapping()->GetGlyphGraphs().begin();
			std::advance(glyphTree, m_glyphTreeIndex);
			m_internalModel->SetMinMaxGlyphTree(std::make_shared<SynGlyphX::DataMappingGlyphGraph>(*glyphTree->second.get()));
		}

		//The top position will be the row of the glyph since the glyph tree view has multiple glyphs.  We don't need it anymore
		childPositions.pop();
		childPositions.push(0);

		QModelIndex internalIndex = GetModelIndexFromStack(childPositions, m_internalModel);
		newSelection.select(internalIndex, internalIndex);
	}

	QObject::disconnect(m_internalSelectionConnection);
	m_internalSelectionModel->select(newSelection, QItemSelectionModel::ClearAndSelect);
	ConnectInternalSelection();
}

void DataMapping3DWidget::OnExternalDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {

	if (m_glyphTreeIndex == -1) {

		return;
	}

	QModelIndex topLeftSource = m_externalModel->mapToSource(topLeft);
	QModelIndex bottomRightSource = m_externalModel->mapToSource(bottomRight);

	//QModelIndex topLeftInternal = GetInternalModelIndex(topLeft);
	//QModelIndex bottomRightInternal = GetInternalModelIndex(bottomRight);

	QModelIndex parent = GetInternalModelIndex(topLeft.parent());

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

void DataMapping3DWidget::OnExternalRowsRemoved(const QModelIndex& parent, int first, int last) {

	if (!parent.isValid() && (first != m_glyphTreeIndex)) {

		return;
	}

	QObject::disconnect(m_internalSelectionConnection);
	QModelIndex internalParent = GetInternalModelIndex(m_externalModel->mapToSource(parent));
	m_internalModel->removeRows(first, last - first + 1, internalParent);
	ConnectInternalSelection();
}

void DataMapping3DWidget::OnExternalRowsInserted(const QModelIndex& parent, int first, int last) {

	QModelIndex externalParentSource = m_externalModel->mapToSource(parent);
	if (externalParentSource.isValid()) {

		std::stack<unsigned int> childPositions = GetRowIndiciesFromStack(externalParentSource);

		if (m_glyphTreeIndex == childPositions.top()) {

			childPositions.pop();
			childPositions.push(0);
			QModelIndex internalParent = GetModelIndexFromStack(childPositions, m_internalModel);
			for (int i = first; i <= last; ++i) {

				//Need to make sure children of this index also get added
				m_internalModel->AppendChildGraph(internalParent, m_dataTransformModel->GetSubgraph(m_dataTransformModel->index(i, 0, externalParentSource), true));
			}
		}
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

void DataMapping3DWidget::OnExternalModelReset() {

	//if ((m_externalModel->rowCount() == 0) && (m_internalModel->rowCount() > 0)) {

		Clear();
	//}
}

void DataMapping3DWidget::Clear() {

	QObject::disconnect(m_internalSelectionConnection);
	m_glyphTreeIndex = -1;
	m_internalModel->removeRow(0);
	ConnectInternalSelection();
}

QModelIndex DataMapping3DWidget::GetInternalModelIndex(const QModelIndex& externalIndex) const {

	std::stack<unsigned int> childPositions = GetRowIndiciesFromStack(externalIndex);
	if (!childPositions.empty()) {

		childPositions.pop();
		childPositions.push(0);
	}
	return GetModelIndexFromStack(childPositions, m_internalModel);
}

QModelIndexList DataMapping3DWidget::GetValidExternalIndexes(const QModelIndexList& indexList) const {

	QModelIndexList validIndexList;
	for (const QModelIndex& index : indexList) {

		if (index.isValid() && (index.data(SynGlyphX::DataTransformModel::DataTypeRole) == SynGlyphX::DataTransformModel::DataType::GlyphTrees)) {

			validIndexList.push_back(index);
		}
	}

	return validIndexList;
}