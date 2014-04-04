#include "glyphbuilderwindow.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QSplashScreen>
#include <QtCore/QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //QPixmap pixmap(":/Resources/synglyphx_splash.png");
    QPixmap pixmap("D:\\Development\\ANTzPlus\\SGXGlyphBuilder\\Resources\\synglyphx_splash.png");
    QSplashScreen splash;
    splash.setPixmap(pixmap);
    splash.show();

    splash.showMessage("Loading Glyph Builder", Qt::AlignHCenter | Qt::AlignBottom);

    a.processEvents();

    GlyphBuilderWindow w;

    QTimer::singleShot(2500, &splash, SLOT(close()));
    QTimer::singleShot(2600, &w, SLOT(show()));

    //w.show();
    //splash.finish(&w);
    return a.exec();
}
