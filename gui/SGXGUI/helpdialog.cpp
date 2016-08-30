#include "helpdialog.h"
#include "application.h"
#include <QtWidgets/QFileDialog>
#include <QtCore/QStandardPaths>
#include <QtWebEngineWidgets/QWebEngineView>

namespace SynGlyphX {

	HelpDialog::HelpDialog(int width, int height, QWidget *parent)
		: QWebEngineView(parent)
	{
		setMinimumSize(width, height);
		QString path = SynGlyphX::Application::GetCommonDataLocation() + QDir::separator() + "Help" + QDir::separator();
		path.replace("\\", "/");
		load(QUrl((path + "index.htm")));
	}

	HelpDialog::~HelpDialog()
	{

	}
}