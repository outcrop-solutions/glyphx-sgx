#include "DMServices.h"
#include "datamapperwindow.h"
#include "DMGlobal.h"
#include <QtWidgets/QUndoCommand>
#include "datatransformmodel.h"
#include "roledatafilterproxymodel.h"

using namespace SynGlyphX;
class TreeChangeCommand : public QUndoCommand {
public:
	TreeChangeCommand() :
		m_tv(nullptr), //tree view is also used as a flag to avoid first redo execution when command added to undo stack
		m_tvmBegin(nullptr),
		m_tvmEnd(nullptr)
	{
	}

	void undo() {
		if (m_tv)
			m_tv->ReinstateMemento(m_tvmBegin);
	}
	void redo() {
		if (m_tv)
			m_tv->ReinstateMemento(m_tvmEnd);
	}
	virtual ~TreeChangeCommand() {
		delete m_tvmBegin;
		delete m_tvmEnd;
	}
	GlyphTreesView* m_tv;
	GlyphTreesViewMemento* m_tvmBegin;
	GlyphTreesViewMemento* m_tvmEnd;
};

class TreeSelection
{
public:
	typedef  QPair<boost::uuids::uuid, unsigned long> TreeNode;
	QList<TreeNode > m_list;
};

class DMServicesImpl  {
public:
	DMServicesImpl(DataMapperWindow* w) : m_w(w)  {}

	void BeginTransaction(const char* name, SynGlyphX::TransactionType t) {

		if (t == TransactionType::ChangeTree) {
			tcc = new TreeChangeCommand();
			tcc->setText(name);
			tcc->m_tvmBegin = m_w->m_glyphTreesView->CreateMemento();
		}

	}

	void EndTransaction(){
		tcc->m_tvmEnd = m_w->m_glyphTreesView->CreateMemento();
		auto us = DMGlobal::Services()->GetUndoStack();
		us->push(tcc);
		tcc->m_tv = m_w->m_glyphTreesView;
	}

	GlyphRolesTableModel* GetGlyphRolesTableModel() {
		return m_w->m_glyphRolesTableModel;
	}

	SynGlyphX::DataTransformModel*  GetDataTransformModel() {
		return m_w->m_dataTransformModel;
	}

	GlyphTreesView* GetTreeView() {
		return m_w->m_glyphTreesView;
	}

	TreeSelection* CreateTreeSelection() {
		TreeSelection* s = new TreeSelection;
		SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(GetTreeView()->model());
		auto selectIndexes = filterModel->mapSelectionToSource(GetTreeView()->selectionModel()->selection()).indexes();

		for (auto index : selectIndexes) {
			if (index.isValid()){
				SynGlyphX::DataMappingGlyphGraph::Node* treeNode = static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer());
				SynGlyphX::DataMappingGlyphGraph::GlyphIterator glyph(treeNode);
				s->m_list << TreeSelection::TreeNode(m_w->m_dataTransformModel->GetTreeId(index), glyph->first);
			}
		}
		return s;
	}

	void GetSelectedOnLevel(const TreeSelection* s, QItemSelection* qs, QModelIndex& index){
		SynGlyphX::DataMappingGlyphGraph::Node* treeNode = static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer());
		if (treeNode){
			SynGlyphX::DataMappingGlyphGraph::GlyphIterator glyph(treeNode);
			if (s->m_list.contains(TreeSelection::TreeNode(m_w->m_dataTransformModel->GetTreeId(index), glyph->first))) {
				qs->select(index, index);
			}
			for (int row = 0; row < m_w->m_dataTransformModel->rowCount(index); ++row)
				GetSelectedOnLevel(s, qs, index.child(row, 0));
		}
	}

	void ApplyTreeSelection(const TreeSelection& selection)  {
		SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(GetTreeView()->model());
		QItemSelection qselection;
		for (int row = 0; row < m_w->m_dataTransformModel->rowCount(); ++row)
			GetSelectedOnLevel(&selection, &qselection, m_w->m_dataTransformModel->index(row, 0));
		GetTreeView()->selectionModel()->select(filterModel->mapSelectionFromSource(qselection), QItemSelectionModel::ClearAndSelect);
	}


	DataMapperWindow* m_w;
	TreeChangeCommand* tcc;
};

DMServices::DMServices(DataMapperWindow* w) : SynGlyphX::AppServices(w) {
	pImpl = new DMServicesImpl(w);
}

void DMServices::BeginTransaction(const char* name, SynGlyphX::TransactionType t) {
	pImpl->BeginTransaction(name, t);
}

void DMServices::EndTransaction() {
	pImpl->EndTransaction();
}

GlyphRolesTableModel* DMServices::GetGlyphRolesTableModel() {
	return pImpl->GetGlyphRolesTableModel();
}

SynGlyphX::DataTransformModel*  DMServices::GetDataTransformModel() {
	return pImpl->GetDataTransformModel();
}

QItemSelectionModel* DMServices::GetTreeViewSelectionModel() {
	return pImpl->GetTreeView()->selectionModel();
}

TreeSelection* DMServices::CreateTreeSelection() {
	return pImpl->CreateTreeSelection();
}
void DMServices::ApplyTreeSelection(const TreeSelection& selection) {
	pImpl->ApplyTreeSelection(selection);
}

DMServices::~DMServices() {
	delete pImpl;
}
