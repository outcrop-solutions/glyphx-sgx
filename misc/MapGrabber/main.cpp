#include "mapgrabberdialog.h"
#include <QtWidgets/QSplashScreen>
#include <QtGui/QIcon>
#include <QtCore/QTimer>
#include "application.h"

int main(int argc, char *argv[])
{
    SynGlyphX::Application::Setup("Map Grabber", "1.0");
    SynGlyphX::Application a(argc, argv);

    SynGlyphX::Application::SetupIcons(QIcon(":SGXGUI/Resources/synglyphx_x.ico"));

    //Setup and show the splash screen
    QPixmap pixmap(":SGXGUI/Resources/synglyphx_splash.png");
    QSplashScreen splash;
    splash.setPixmap(pixmap);
    splash.show();

    splash.showMessage("Loading Map Grabber", Qt::AlignHCenter | Qt::AlignBottom);

    a.processEvents();

    MapGrabberDialog w;
    
    //Need to figure out better way to not have the splash screen disappear before the user sees it
    QTimer::singleShot(1500, &splash, SLOT(close()));
    QTimer::singleShot(1600, &w, SLOT(show()));

    return a.exec();
}
