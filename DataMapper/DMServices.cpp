#include "DMServices.h"
#include "datamapperwindow.h"
#include "DMGlobal.h"
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
	GlyphTreesView* m_tv;
	GlyphTreesViewMemento* m_tvmBegin;
	GlyphTreesViewMemento* m_tvmEnd;
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

DMServices::~DMServices() {
	delete pImpl;
}
