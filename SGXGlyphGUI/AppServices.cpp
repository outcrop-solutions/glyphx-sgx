#include "AppServices.h"
#include "mainwindow.h"
#include <QtWidgets/QUndoStack>
namespace SynGlyphX {
	AppServices::AppServices(MainWindow* mw) : m_window(mw) {

	}
	
	QUndoStack* AppServices::GetUndoStack() {
		return m_window->m_undoStack;
	}
	void AppServices::ClearUndoStack() {
		m_window->m_undoStack->clear();
	}

}
