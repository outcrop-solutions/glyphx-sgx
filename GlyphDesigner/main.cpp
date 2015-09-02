#include "glyphdesignerwindow.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QSplashScreen>
#include <QtCore/QTimer>
#include "glyphbuilderapplication.h"
#include "licensingdialog.h"
#include "exception_handler.h"
#include <QtCore/QStandardPaths>
#include <iostream>
#include <boost/filesystem.hpp>

int main(int argc, char *argv[])
{
    SynGlyphX::GlyphBuilderApplication::Setup("Glyph Builder - Glyph Designer", "0.7.12");
	SynGlyphX::GlyphBuilderApplication a(argc, argv);

	const QString dumpPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Minidumps";
	std::wstring pathAsStr = (const wchar_t*)dumpPath.utf16();
	boost::filesystem::path dir(pathAsStr);
	if (boost::filesystem::create_directory(dir))
	{
		std::cerr << "Directory Created: " << std::endl;
	}

	google_breakpad::ExceptionHandler *pHandler = new google_breakpad::ExceptionHandler(
		pathAsStr,
		0,
		0,
		0,
		google_breakpad::ExceptionHandler::HANDLER_ALL,
		MiniDumpNormal,
		L"",
		0);

	SynGlyphX::GlyphBuilderApplication::SetupIcons(QIcon(":SGXGUI/Resources/synglyphx_x.ico"));

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
