#include "selectiontranslator.h"
#include <stack>

SelectionTranslator::SelectionTranslator(DataTransformModel* dataTransformModel, GlyphTreeModel* glyphTreeModel, QObject *parent)
	: QObject(parent),
	m_dataTransformModel(dataTransformModel),
	m_glyphTreeModel(glyphTreeModel)
{
	m_selectionModel = new QItemSelectionModel(m_glyphTreeModel, this);
}

SelectionTranslator::~SelectionTranslator()
{

}

QItemSelectionModel* SelectionTranslator::GetSelectionModel() const {

	return m_selectionModel;
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

		m_glyphTree = newGlyphTree;
		for (auto minMaxGlyphTree : m_dataTransformModel->GetDataTransform()->GetGlyphTrees()) {

			if (minMaxGlyphTree.second.get() == m_glyphTree) {

				SynGlyphX::GlyphTree::SharedPtr glyphTree = m_dataTransformModel->GetDataTransform()->GetGlyphTrees().at(minMaxGlyphTree.first)->GetMinGlyphTree();
				glyphTree->root()->SetPosition({ { 0.0, 0.0, 0.0 } });
				m_glyphTreeModel->CreateNewTree(glyphTree);
				break;
			}
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

		m_selectionModel->select(glyphTreeModelIndex, QItemSelectionModel::ClearAndSelect);
	}
}

void SelectionTranslator::UpdateSelectedGlyphProperties(SynGlyphX::GlyphProperties::ConstSharedPtr glyph) {

	if (m_selectionModel->selectedIndexes().isEmpty()) {
		
		return;
	}

	QModelIndex index = m_selectionModel->selectedIndexes()[0];
	GlyphTreeModel::PropertyUpdates updates = GlyphTreeModel::UpdateAll;
	if (!index.parent().isValid()) {
		//If index is the root glyph don't update its position
		updates ^= GlyphTreeModel::UpdatePosition;
	}
	m_glyphTreeModel->UpdateNode(m_selectionModel->selectedIndexes()[0], glyph, updates);
}