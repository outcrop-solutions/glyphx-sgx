#include "helpdialog.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include "application.h"
#include <string>
#include <QtWidgets/QMessageBox>
#include "groupboxsinglewidget.h"
#include <QtWidgets/QFileDialog>
#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtGui/QTextDocument>
#include <QtCore/QTextStream>
#include "filesystem.h"
#include <QtWidgets/QLabel>
#include <QtWebEngineWidgets/QWebEngineView>

namespace SynGlyphX {

	HelpDialog::HelpDialog(QWidget *parent)
		: QDialog(parent)
	{
		setWindowTitle(tr("Help"));

		QVBoxLayout* layout = new QVBoxLayout(this);

		QWebEngineView *view = new QWebEngineView(this);
		view->setMinimumSize(970,920);
		QString path = SynGlyphX::Application::GetCommonDataLocation() + QDir::separator() + "Help" + QDir::separator();
		path.replace("\\", "/");
		view->load(QUrl((path + "index.htm")));

		layout->addWidget(view);

		setLayout(layout);
	}

	HelpDialog::~HelpDialog()
	{

	}
}