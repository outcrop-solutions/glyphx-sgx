#include "GDServices.h"
#include "glyphdesignerwindow.h"
#include "GDGlobal.h"
using SynGlyphX::TransactionType;

class GDServicesImpl  {
public:
	GDServicesImpl(GlyphDesignerWindow* w) : m_w(w), m_us(GDGlobal::Services()->GetUndoStack()) {}

	void BeginTransaction(const char* name, SynGlyphX::TransactionType t) {

		if (t == TransactionType::ChangeTree) {
			

		}
	}
	void EndTransaction(){}
	GlyphDesignerWindow* m_w;
	QUndoStack* m_us;
};

GDServices::GDServices(GlyphDesignerWindow* w) : SynGlyphX::AppServices(w) {
	pImpl = new GDServicesImpl(w);
}

void GDServices::BeginTransaction(const char* name, SynGlyphX::TransactionType t) {
	pImpl->BeginTransaction(name, t);
}

void GDServices::EndTransaction() {
	pImpl->EndTransaction();
}

GDServices::~GDServices() {
	delete pImpl;
}