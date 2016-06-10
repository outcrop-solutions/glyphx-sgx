#include "AppServices.h"
#include "mainwindow.h"
namespace SynGlyphX {
	AppServices::AppServices(MainWindow* mw) : m_window(mw) {

	}
	
	QUndoStack* AppServices::GetUndoStack() {
		return m_window->m_undoStack;
	}

}