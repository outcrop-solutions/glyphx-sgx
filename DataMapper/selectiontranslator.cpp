#include "selectiontranslator.h"
#include <stack>

SelectionTranslator::SelectionTranslator(DataTransformModel* dataTransformModel, GlyphTreeModel* glyphTreeModel, QItemSelectionModel* treeViewSelectionModel, QObject *parent)
	: QObject(parent),
	m_dataTransformModel(dataTransformModel),
	m_glyphTreeModel(glyphTreeModel),
	m_treeViewSelectionModel(treeViewSelectionModel)
{
	m_3DViewSelectionModel = new QItemSelectionModel(m_glyphTreeModel, this);
	Connect3DViewSelectionModel();
	ConnectTreeViewSelectionModel();
}

SelectionTranslator::~SelectionTranslator()
{

}

void SelectionTranslator::Connect3DViewSelectionModel() {

	m_3DViewConnection = QObject::connect(m_3DViewSelectionModel, &QItemSelectionModel::selectionChanged, this, &SelectionTranslator::On3DViewSelectionChanged);
}

void SelectionTranslator::ConnectTreeViewSelectionModel() {

	m_treeViewConnection = QObject::connect(m_treeViewSelectionModel, &QItemSelectionModel::selectionChanged, this, &SelectionTranslator::OnTreeViewSelectionChanged);
}

QItemSelectionModel* SelectionTranslator::Get3DViewSelectionModel() const {

	return m_3DViewSelectionModel;
}

void SelectionTranslator::OnTreeViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	if (selected.empty() || !selected.indexes()[0].isValid()) {
		return;
	}

	QModelIndex minMaxIndex = selected.indexes()[0];

	SynGlyphX::MinMaxGlyphTree::Node* node = static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(minMaxIndex.internalPointer());
	SynGlyphX::MinMaxGlyphTree::iterator newGlyph = SynGlyphX::MinMaxGlyphTree::iterator(node);
	const SynGlyphX::MinMaxGlyphTree* newGlyphTree = static_cast<const SynGlyphX::MinMaxGlyphTree*>(newGlyph.owner());

	if (m_glyphTree != newGlyphTree) {

		m_glyphTreeIndex = 0;
		m_glyphTree = newGlyphTree;
		for (auto minMaxGlyphTree : m_dataTransformModel->GetDataTransform()->GetGlyphTrees()) {

			if (minMaxGlyphTree.second.get() == m_glyphTree) {

				SynGlyphX::GlyphTree::SharedPtr glyphTree = m_dataTransformModel->GetDataTransform()->GetGlyphTrees().at(minMaxGlyphTree.first)->GetMinGlyphTree();
				glyphTree->root()->SetPosition({ { 0.0, 0.0, 0.0 } });
				m_glyphTreeModel->CreateNewTree(glyphTree);
				break;
			}

			++m_glyphTreeIndex;
		}
	}

	if (m_glyph != newGlyph) {

		m_glyph = newGlyph;
		std::stack<int> childPositions;
		while (minMaxIndex.isValid()) {

			childPositions.push(minMaxIndex.row());
			minMaxIndex = minMaxIndex.parent();
		}

		//The top position will be the row of the glyph since the glyph tree view has multiple glyphs.  We don't need it
		childPositions.pop();

		QModelIndex glyphTreeModelIndex = m_glyphTreeModel->index(0);

		while (!childPositions.empty()) {
			
			glyphTreeModelIndex = m_glyphTreeModel->index(childPositions.top(), 0, glyphTreeModelIndex);
			childPositions.pop();
		}

		QObject::disconnect(m_3DViewConnection);
		m_3DViewSelectionModel->select(glyphTreeModelIndex, QItemSelectionModel::ClearAndSelect);
		Connect3DViewSelectionModel();
	}
}

void SelectionTranslator::On3DViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	if (selected.empty() || !selected.indexes()[0].isValid()) {
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

	QModelIndex minMaxIndex = m_dataTransformModel->index(m_glyphTreeIndex);

	while (!childPositions.empty()) {

		minMaxIndex = m_dataTransformModel->index(childPositions.top(), 0, minMaxIndex);
		childPositions.pop();
	}

	QObject::disconnect(m_treeViewConnection);
	m_treeViewSelectionModel->select(minMaxIndex, QItemSelectionModel::ClearAndSelect);
	ConnectTreeViewSelectionModel();
}

void SelectionTranslator::UpdateSelectedGlyphProperties(SynGlyphX::GlyphProperties::ConstSharedPtr glyph) {

	if (m_3DViewSelectionModel->selectedIndexes().isEmpty()) {
		
		return;
	}

	QModelIndex index = m_3DViewSelectionModel->selectedIndexes()[0];
	GlyphTreeModel::PropertyUpdates updates = GlyphTreeModel::UpdateAll;
	if (!index.parent().isValid()) {
		//If index is the root glyph don't update its position
		updates ^= GlyphTreeModel::UpdatePosition;
	}
	m_glyphTreeModel->UpdateNode(m_3DViewSelectionModel->selectedIndexes()[0], glyph, updates);
}