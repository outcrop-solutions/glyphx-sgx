#include "DMServices.h"
#include "datamapperwindow.h"
#include "DMGlobal.h"
#include <QtWidgets/QUndoCommand>
#include "datatransformmodel.h"
#include "roledatafilterproxymodel.h"
#include "Link.h"
#include "GlyphTreesViewMementoBase.h"
using namespace SynGlyphX;
class TreeChangeCommand : public QUndoCommand 
{
public:
	TreeChangeCommand() :
		m_tv(nullptr), //tree view is also used as a flag to avoid first redo execution when command added to undo stack
		m_tvmBegin(nullptr),
		m_tvmEnd(nullptr)
	{
	}

	void undo() 
	{
		if (m_tv)
			m_tv->ReinstateMemento(m_tvmBegin);
	}
	void redo() 
	{
		if (m_tv)
			m_tv->ReinstateMemento(m_tvmEnd);
	}
	virtual ~TreeChangeCommand() 
	{
		delete m_tvmBegin;
		delete m_tvmEnd;
	}
	GlyphTreesView* m_tv;
	GlyphTreesViewMemento* m_tvmBegin;
	GlyphTreesViewMemento* m_tvmEnd;
};

class LinksChangeCommand : public QUndoCommand 
{
public:
	LinksChangeCommand() : firstRun(true) {}
	LinksChangeCommand(const std::vector<Link>& links) : 
		firstRun(true),
		m_oldLinks(DMGlobal::Services()->GetDataTransformModel()->GetDataMapping()->GetLinks()),
		m_newLinks(links)
	{}
	void undo() 
	{
		DMGlobal::Services()->GetDataTransformModel()->SetLinks(m_oldLinks);
		firstRun = false;
	}
	void redo() 
	{
		if (!firstRun)
			DMGlobal::Services()->GetDataTransformModel()->SetLinks(m_newLinks);
	}

	bool firstRun;
	std::vector<Link> m_oldLinks;
	std::vector<Link> m_newLinks;
};

class TreeSelectionImpl : public TreeSelection
{
public:
	typedef  QPair<boost::uuids::uuid, unsigned long> TreeNode;
	QList<TreeNode > m_list;
	virtual ~TreeSelectionImpl() {}
};

class DMServicesImpl 
{
public:
	DMServicesImpl(DataMapperWindow* w) : m_w(w)  {}

	void BeginTransaction(const char* name, int type) 
	{
		m_type = type;
		DMGlobal::Services()->GetUndoStack()->beginMacro(name);
		if (type & TransactionType::ChangeTree) 
		{
			tcc = new TreeChangeCommand();
			//tcc->setText(name);
			tcc->m_tvmBegin = m_w->m_glyphTreesView->CreateMemento();
		}

		if (type & TransactionType::ChangeLinks) 
		{
			lcc = new LinksChangeCommand();
			lcc->m_oldLinks = GetDataTransformModel()->GetDataMapping()->GetLinks();
		}
	}

	void EndTransaction()
	{
		if (m_type & TransactionType::ChangeTree)
		{
			tcc->m_tvmEnd = m_w->m_glyphTreesView->CreateMemento();
			auto us = DMGlobal::Services()->GetUndoStack();
			us->push(tcc);
			tcc->m_tv = m_w->m_glyphTreesView;
		}

		if (m_type & TransactionType::ChangeLinks)
		{
			lcc->m_newLinks = GetDataTransformModel()->GetDataMapping()->GetLinks();
			DMGlobal::Services()->GetUndoStack()->push(lcc);
		}

		DMGlobal::Services()->GetUndoStack()->endMacro();
	}

	GlyphRolesTableModel* GetGlyphRolesTableModel() 
	{
		return m_w->m_glyphRolesTableModel;
	}

	SynGlyphX::DataTransformModel*  GetDataTransformModel() 
	{
		return m_w->m_dataTransformModel;
	}

	GlyphTreesView* GetTreeView() 
	{
		return m_w->m_glyphTreesView;
	}

	TreeSelection* CreateTreeSelection() 
	{
		TreeSelectionImpl* s = new TreeSelectionImpl;
		SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(GetTreeView()->model());
		auto selectIndexes = filterModel->mapSelectionToSource(GetTreeView()->selectionModel()->selection()).indexes();

		for (auto index : selectIndexes) 
		{
			if (index.isValid())
			{
				SynGlyphX::DataMappingGlyphGraph::Node* treeNode = static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer());
				SynGlyphX::DataMappingGlyphGraph::GlyphIterator glyph(treeNode);
				s->m_list << TreeSelectionImpl::TreeNode(m_w->m_dataTransformModel->GetTreeId(index), glyph->first);
			}
		}
		return s;
	}

	void GetSelectedOnLevel(const TreeSelectionImpl* s, QItemSelection* qs, const QModelIndex& index)
	{
		SynGlyphX::DataMappingGlyphGraph::Node* treeNode = static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer());
		if (treeNode)
		{
			SynGlyphX::DataMappingGlyphGraph::GlyphIterator glyph(treeNode);
			if (s->m_list.contains(TreeSelectionImpl::TreeNode(m_w->m_dataTransformModel->GetTreeId(index), glyph->first))) {
				qs->select(index, index);
			}
			for (int row = 0; row < m_w->m_dataTransformModel->rowCount(index); ++row)
				GetSelectedOnLevel(s, qs, index.child(row, 0));
		}
	}

	void ApplyTreeSelection(const TreeSelection& selection)  
	{
		SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(GetTreeView()->model());
		QItemSelection qselection;
		for (int row = 0; row < m_w->m_dataTransformModel->rowCount(); ++row)
			GetSelectedOnLevel(dynamic_cast<const TreeSelectionImpl*>(&selection), &qselection, m_w->m_dataTransformModel->index(row, 0));
		GetTreeView()->selectionModel()->select(filterModel->mapSelectionFromSource(qselection), QItemSelectionModel::ClearAndSelect);
	}

	int m_type;
	DataMapperWindow* m_w;
	TreeChangeCommand* tcc;
	LinksChangeCommand* lcc;
};

DMServices::DMServices(DataMapperWindow* w) : SynGlyphX::AppServices(w) 
{
	pImpl = new DMServicesImpl(w);
}

void DMServices::BeginTransaction(const char* name, int type) 
{
	pImpl->BeginTransaction(name, type);
}

void DMServices::EndTransaction() 
{
	pImpl->EndTransaction();
}

GlyphRolesTableModel* DMServices::GetGlyphRolesTableModel() 
{
	return pImpl->GetGlyphRolesTableModel();
}

SynGlyphX::DataTransformModel*  DMServices::GetDataTransformModel() 
{
	return pImpl->GetDataTransformModel();
}

QItemSelectionModel* DMServices::GetTreeViewSelectionModel() 
{
	return pImpl->GetTreeView()->selectionModel();
}

TreeSelection* DMServices::CreateTreeSelection() 
{
	return pImpl->CreateTreeSelection();
}
void DMServices::ApplyTreeSelection(const TreeSelection& selection) 
{
	pImpl->ApplyTreeSelection(selection);
}

DMServices::~DMServices() 
{
	delete pImpl;
}
