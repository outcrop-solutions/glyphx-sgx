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
		QString path = QDir::currentPath() + QDir::separator() + "Help" + QDir::separator();
		path.replace( "\\", "/" );
		dlg->load(QUrl("http://sample-env.zccmpffgqr.us-west-2.elasticbeanstalk.com/help/" ) );
		return dlg;
	}
}