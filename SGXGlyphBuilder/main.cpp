#include "glyphbuilderwindow.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QSplashScreen>
#include <QtCore/QTimer>
#include <QtCore/QDir>

int main(int argc, char *argv[])
{
    QApplication::addLibraryPath(QDir::currentPath() + QDir::separator() + "qt_plugins");
    QApplication a(argc, argv);

    //Set the default application icon
    QApplication::setWindowIcon(QIcon(":GlyphBuilderWindow/Resources/synglyphx_x.ico"));

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
