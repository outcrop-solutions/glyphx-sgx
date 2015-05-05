#include "glyphdesignerwindow.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QSplashScreen>
#include <QtCore/QTimer>
#include "application.h"
#include "licensingdialog.h"

int main(int argc, char *argv[])
{
    SynGlyphX::Application::Setup("Glyph Builder - Glyph Designer", "0.7.05");
    SynGlyphX::Application a(argc, argv);

    SynGlyphX::Application::SetupIcons(QIcon(":SGXGUI/Resources/synglyphx_x.ico"));

	if (!SynGlyphX::LicensingDialog::CheckLicense()) {

		return 0;
	}

    //Setup and show the splash screen
    QPixmap pixmap(":SGXGUI/Resources/synglyphx_splash.png");
    QSplashScreen splash;
    splash.setPixmap(pixmap);
    splash.show();

    splash.showMessage("Loading Glyph Designer", Qt::AlignHCenter | Qt::AlignBottom);

    a.processEvents();

    GlyphDesignerWindow w;
    w.resize(1200, 700);

    //Need to figure out better way to not have the splash screen disappear before the user sees it
    QTimer::singleShot(1500, &splash, SLOT(close()));
    QTimer::singleShot(1600, &w, SLOT(show()));

    //w.show();
    //splash.finish(&w);
    return a.exec();
}
