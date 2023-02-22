#include "AppServices.h"
#include "mainwindow.h"
#include <QtGui/QUndoStack>
#include <QtWidgets/QMessageBox>
#include <QtCore/QObject>

namespace SynGlyphX {
	AppServices::AppServices(MainWindow* mw) : m_window(mw) {

	}
	
	QUndoStack* AppServices::GetUndoStack() {
		return m_window->m_undoStack;
	}

	void AppServices::ClearUndoStack() {
		m_window->m_undoStack->clear();
	}

	void AppServices::SetModified(bool m) {
		m_window->setWindowModified(m);
	}

	void AppServices::ShowWarningDialog(const QString& msg) {

		QMessageBox::warning(m_window, QObject::tr("Warning"), msg);
	}

	QString AppServices::GetFileNameOpenDialog(const QString& settingKey, const QString& caption, const QString& defaultDir, const QString& filter) {

		return m_window->GetFileNameOpenDialog(settingKey, caption, defaultDir, filter);
	}

	QString AppServices::GetFileNameSaveDialog(const QString& settingKey, const QString& caption, const QString& defaultDir, const QString& filter) {

		return m_window->GetFileNameSaveDialog(settingKey, caption, defaultDir, filter);
	}

}
