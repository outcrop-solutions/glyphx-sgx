#include "datamapperwindow.h"
#include "application.h"
#include <QtWidgets/QSplashScreen>
#include <QtCore/QTimer>
#include "sourcedatamanager.h"

int main(int argc, char *argv[])
{
    SynGlyphX::Application::Setup("Data Mapper", "0.6.8");
    SynGlyphX::Application a(argc, argv);

    SynGlyphX::Application::SetupIcons(QIcon(":SGXGUI/Resources/synglyphx_x.ico"));

    //Setup and show the splash screen
    QPixmap pixmap(":SGXGUI/Resources/synglyphx_splash.png");
    QSplashScreen splash;
    splash.setPixmap(pixmap);
    splash.show();

    splash.showMessage("Loading Data Mapper", Qt::AlignHCenter | Qt::AlignBottom);

    a.processEvents();

    DataMapperWindow w;
    w.resize(1200, 700);

    //Need to figure out better way to not have the splash screen disappear before the user sees it
    QTimer::singleShot(1500, &splash, SLOT(close()));
    QTimer::singleShot(1600, &w, SLOT(show()));

    //w.show();
    //splash.finish(&w);
    
    return a.exec();
}
