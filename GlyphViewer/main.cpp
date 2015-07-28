#include "glyphviewerwindow.h"
#include "glyphbuilderapplication.h"
#include <QtWidgets/QSplashScreen>
#include <QtCore/QTimer>
#include "licensingdialog.h"

int main(int argc, char *argv[])
{
	SynGlyphX::GlyphBuilderApplication::Setup("Glyph Builder - Glyph Viewer", "0.7.10");
	SynGlyphX::GlyphBuilderApplication a(argc, argv);

	SynGlyphX::GlyphBuilderApplication::SetupIcons(QIcon(":SGXGUI/Resources/synglyphx_x.ico"));

	if (!SynGlyphX::LicensingDialog::CheckLicense()) {

		return 0;
	}

	//Setup and show the splash screen
	QPixmap pixmap(":SGXGUI/Resources/synglyphx_splash.png");
	QSplashScreen splash;
	splash.setPixmap(pixmap);
	splash.show();

	splash.showMessage("Loading Glyph Viewer", Qt::AlignHCenter | Qt::AlignBottom);

	a.processEvents();

	try {
		GlyphViewerWindow w;
		w.move(50, 50);
		w.resize(1200, 700);

		//Need to figure out better way to not have the splash screen disappear before the user sees it
		QTimer::singleShot(1500, &splash, SLOT(close()));
		QTimer::singleShot(1600, &w, SLOT(show()));

		//w.show();
		//splash.finish(&w);

		return a.exec();
	}
	catch (...) {
		return 1;
	}
}
