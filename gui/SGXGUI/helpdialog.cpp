#include "helpdialog.h"
#include "application.h"
#include <QtWidgets/QFileDialog>
#include <QtCore/QStandardPaths>
#include <QtWebEngineWidgets/QWebEngineView>

namespace SynGlyphX {

	QWebEngineView* createHelpDialog( int width, int height, QWidget* parent )
	{
		QWebEngineView* dlg = new QWebEngineView( parent );
		dlg->setMinimumSize( width, height );
		dlg->load(QUrl("https://s3.amazonaws.com/glyphed/userguide/index.htm"));
		return dlg;
	}
}
