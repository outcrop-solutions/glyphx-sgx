#include "GDServices.h"
#include "glyphdesignerwindow.h"
#include "GDGlobal.h"
#include <QtWidgets/QUndoCommand>

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
	GlyphTreeView* m_tv;
	GlyphTreeViewMemento* m_tvmBegin;
	GlyphTreeViewMemento* m_tvmEnd;
};

class GDServicesImpl  {
public:
	GDServicesImpl(GlyphDesignerWindow* w) : m_w(w)  {}

	void BeginTransaction(const char* name, int t) {

		if (t == TransactionType::ChangeTree) {
			tcc = new TreeChangeCommand();
			tcc->m_tvmBegin = m_w->m_treeView->CreateMemento();
		}
		m_name = name;
	}

	void EndTransaction(){
		tcc->m_tvmEnd = m_w->m_treeView->CreateMemento();
		auto us = GDGlobal::Services()->GetUndoStack();
		us->push(tcc);
		tcc->m_tv = m_w->m_treeView;
	}

	QString m_name;
	GlyphDesignerWindow* m_w;
	TreeChangeCommand* tcc;
};

GDServices::GDServices(GlyphDesignerWindow* w) : SynGlyphX::AppServices(w) {
	pImpl = new GDServicesImpl(w);
}

void GDServices::BeginTransaction(const char* name, int t) {
	pImpl->BeginTransaction(name, t);
}

void GDServices::EndTransaction() {
	pImpl->EndTransaction();
}

GDServices::~GDServices() {
	delete pImpl;
}