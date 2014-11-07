#include "selectiontranslator.h"
#include <stack>
#include "datatransformmodel.h"

SelectionTranslator::SelectionTranslator(SynGlyphX::RoleDataFilterProxyModel* glyphTreesModel, GlyphTreeModel* glyphTree3DModel, MinMaxGlyphModel* minMaxGlyphModel, QItemSelectionModel* treeViewSelectionModel, QObject *parent)
	: QObject(parent),
	m_glyphTreesModel(glyphTreesModel),
	m_glyphTree3DModel(glyphTree3DModel),
	m_minMaxGlyphModel(minMaxGlyphModel),
	m_treeViewSelectionModel(treeViewSelectionModel),
	m_glyphTreeIndex(-1)
{
	m_3DViewSelectionModel = new QItemSelectionModel(m_glyphTree3DModel, this);
	Connect3DViewSelectionModel();
	ConnectTreeViewSelectionModel();
}

SelectionTranslator::~SelectionTranslator()
{

}

void SelectionTranslator::Connect3DViewSelectionModel() {

	m_3DViewConnection = QObject::connect(m_3DViewSelectionModel, &QItemSelectionModel::selectionChanged, this, &SelectionTranslator::On3DViewSelectionChanged);
}

void SelectionTranslator::Clear() {

	m_glyphTreeIndex = -1;
	m_minMaxGlyphModel->Clear();
	m_glyphTree3DModel->removeRow(0);
}

void SelectionTranslator::ConnectTreeViewSelectionModel() {

	m_treeViewConnection = QObject::connect(m_treeViewSelectionModel, &QItemSelectionModel::selectionChanged, this, &SelectionTranslator::OnTreeViewSelectionChanged);
}

QItemSelectionModel* SelectionTranslator::Get3DViewSelectionModel() const {

	return m_3DViewSelectionModel;
}

void SelectionTranslator::OnTreeViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	if (selected.empty() || !selected.indexes()[0].isValid() || (selected.indexes()[0].data(DataTransformModel::DataTypeRole) != DataTransformModel::DataType::GlyphTrees)) {

		Clear();
		return;
	}

	QModelIndex minMaxIndex = selected.indexes()[0];
	m_minMaxGlyphModel->SetMinMaxGlyph(minMaxIndex);

	QModelIndex rootIndex = minMaxIndex;
	while (rootIndex.parent().isValid()) {

		rootIndex = rootIndex.parent();
	}

	SynGlyphX::MinMaxGlyphTree::Node* node = static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(minMaxIndex.internalPointer());
	SynGlyphX::MinMaxGlyphTree::iterator newGlyph = SynGlyphX::MinMaxGlyphTree::iterator(node);

	if (m_glyphTreeIndex != rootIndex.row()) {

		m_glyphTreeIndex = rootIndex.row();
		SynGlyphX::MinMaxGlyphTree::const_iterator node = SynGlyphX::MinMaxGlyphTree::const_iterator(static_cast<SynGlyphX::MinMaxGlyphTree::Node*>(rootIndex.internalPointer()));
		const SynGlyphX::MinMaxGlyphTree* minMaxGlyphTree = static_cast<const SynGlyphX::MinMaxGlyphTree*>(node.owner());

		SynGlyphX::GlyphTree::SharedPtr glyphTree = minMaxGlyphTree->GetMaxGlyphTree();
		glyphTree->root()->SetPosition({ { 0.0, 0.0, 0.0 } });
		m_glyphTree3DModel->CreateNewTree(glyphTree, true);
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

		QModelIndex glyphTreeModelIndex = m_glyphTree3DModel->index(0);

		while (!childPositions.empty()) {
			
			glyphTreeModelIndex = m_glyphTree3DModel->index(childPositions.top(), 0, glyphTreeModelIndex);
			childPositions.pop();
		}

		QObject::disconnect(m_3DViewConnection);
		m_3DViewSelectionModel->select(glyphTreeModelIndex, QItemSelectionModel::ClearAndSelect);
		Connect3DViewSelectionModel();
	}
}

void SelectionTranslator::On3DViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	if (selected.empty() || !selected.indexes()[0].isValid()) {

		m_minMaxGlyphModel->Clear();
		m_glyphTreeIndex = -1;
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

	QModelIndex minMaxIndex = m_glyphTreesModel->index(m_glyphTreeIndex, 0);

	while (!childPositions.empty()) {

		minMaxIndex = m_glyphTreesModel->index(childPositions.top(), 0, minMaxIndex);
		childPositions.pop();
	}

	m_minMaxGlyphModel->SetMinMaxGlyph(minMaxIndex);

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
	m_glyphTree3DModel->UpdateNode(m_3DViewSelectionModel->selectedIndexes()[0], glyph, updates);
}