#include "datamapperwindow.h"
#include "glyphbuilderapplication.h"
#include <QtWidgets/QSplashScreen>
#include <QtCore/QTimer>
#include "sourcedatamanager.h"
#include "licensingdialog.h"
#include "doubleminmaxwidget.h"
#include "intminmaxwidget.h"
#include "colorminmaxwidget.h"

int main(int argc, char *argv[])
{
    SynGlyphX::GlyphBuilderApplication::Setup("Glyph Builder - Data Mapper", "0.7.11");
	SynGlyphX::GlyphBuilderApplication a(argc, argv);

	SynGlyphX::GlyphBuilderApplication::SetupIcons(QIcon(":SGXGUI/Resources/synglyphx_x.ico"));

	if (!SynGlyphX::LicensingDialog::CheckLicense()) {

		return 0;
	}

	qRegisterMetaType<SynGlyphX::ColorMinDiff>("ColorMinDiff");
	qRegisterMetaType<SynGlyphX::IntMinDiff>("IntMinDiff");
	qRegisterMetaType<SynGlyphX::DoubleMinDiff>("DoubleMinDiff");
	qRegisterMetaType<SynGlyphX::InputField>("InputField");

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
