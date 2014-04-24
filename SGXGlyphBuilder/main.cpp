#include "glyphbuilderwindow.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QSplashScreen>
#include <QtCore/QTimer>
#include "application.h"

int main(int argc, char *argv[])
{
    SynGlyphX::Application::Setup("Glyph Builder", "0.5");
    SynGlyphX::Application a(argc, argv);

    SynGlyphX::Application::SetupIcons(QIcon(":GlyphBuilderWindow/Resources/synglyphx_x.ico"));

    //Setup and show the splash screen
    QPixmap pixmap(":GlyphBuilderWindow/Resources/synglyphx_splash.png");
    QSplashScreen splash;
    splash.setPixmap(pixmap);
    splash.show();

    splash.showMessage("Loading Glyph Builder", Qt::AlignHCenter | Qt::AlignBottom);

    a.processEvents();

    GlyphBuilderWindow w;
    w.resize(1200, 700);

    //Need to figure out better way to not have the splash screen disappear before the user sees it
    QTimer::singleShot(1500, &splash, SLOT(close()));
    QTimer::singleShot(1600, &w, SLOT(show()));

    //w.show();
    //splash.finish(&w);
    return a.exec();
}
