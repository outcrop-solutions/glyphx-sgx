#include "selectiontranslator.h"

SelectionTranslator::SelectionTranslator(DataTransformModel* dataTransformModel, QObject *parent)
	: QObject(parent),
	m_dataTransformModel(dataTransformModel)
{

}

SelectionTranslator::~SelectionTranslator()
{

}

void SelectionTranslator::OnTreeViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	if (selected.empty() || !selected.indexes()[0].isValid()) {
		return;
	}

	SynGlyphX::MinMaxGlyphTree::Node* node = static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(selected.indexes()[0].internalPointer());
	SynGlyphX::MinMaxGlyphTree::iterator newGlyph = SynGlyphX::MinMaxGlyphTree::iterator(node);
	const SynGlyphX::MinMaxGlyphTree* newGlyphTree = static_cast<const SynGlyphX::MinMaxGlyphTree*>(newGlyph.owner());

	if (m_glyphTree != newGlyphTree) {

		m_glyphTree = newGlyphTree;
		for (auto glyphTree : m_dataTransformModel->GetDataTransform()->GetGlyphTrees()) {

			if (glyphTree.second.get() == m_glyphTree) {
				emit GlyphTreeChanged(glyphTree.first);
				break;
			}
		}
	}

	m_glyph = newGlyph;
	
}