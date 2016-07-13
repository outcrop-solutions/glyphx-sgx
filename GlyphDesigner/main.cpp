#include "glyphdesignerwindow.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QSplashScreen>
#include <QtCore/QTimer>
#include "glyphbuilderapplication.h"
#include "licensingdialog.h"
#include <QtCore/QStandardPaths>
#include <QtWidgets/QMessageBox>
#include <boost/filesystem.hpp>
#include "GDServices.h"
#include "GDGlobal.h"

#ifdef USE_BREAKPAD
#include "exception_handler.h"
#endif

int main(int argc, char *argv[])
{
#ifdef __APPLE__
    // Mac: Add plugin path in package.
    // 'macdeployqt' needs to be run on the app package after building for this to work.
    QDir dir(argv[0]); // e.g. appdir/Contents/MacOS/appname
    dir.cdUp();
    dir.cdUp();
    dir.cd("PlugIns"); // e.g. appdir/Contents/PlugIns
    QCoreApplication::setLibraryPaths(QStringList(dir.absolutePath()));
    printf("after change, libraryPaths=(%s)\n", QCoreApplication::libraryPaths().join(",").toUtf8().data());
#endif
    
    SynGlyphX::GlyphBuilderApplication::Setup("Glyph Builder - Glyph Designer", "0.7.51");
	SynGlyphX::GlyphBuilderApplication a(argc, argv);

#ifdef USE_BREAKPAD
	const QString dumpPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Minidumps";
	std::wstring pathAsStr = dumpPath.toStdWString();
	boost::filesystem::path dir(pathAsStr);
	try {

		boost::filesystem::create_directory(dir);
	}
	catch (const std::exception& e) {

		QMessageBox::critical(nullptr, SynGlyphX::GlyphBuilderApplication::tr("Minidump Failure"), SynGlyphX::GlyphBuilderApplication::tr("Failed to create directory for crash reporting") + e.what());
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
#endif

	SynGlyphX::GlyphBuilderApplication::SetupIconsAndLogos();

#ifdef USE_LICENSING
	if (!SynGlyphX::LicensingDialog::CheckLicense()) {

		return 0;
	}
#endif

	//Setup and show the splash screen
	QPixmap pixmap(SynGlyphX::GlyphBuilderApplication::GetSplashScreenLocation());
	QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);
	splash.show();

    splash.showMessage("Loading Glyph Designer", Qt::AlignHCenter | Qt::AlignBottom);

    a.processEvents();
	
	try {

		GlyphDesignerWindow w;
		w.resize(1200, 700);
		GDGlobal::Init(new GDServices(&w));
		//Need to figure out better way to not have the splash screen disappear before the user sees it
		QTimer::singleShot(1500, &splash, SLOT(close()));
		w.show();
		//QTimer::singleShot(1600, &w, SLOT(show()));4 yet

		//w.show();
		//splash.finish(&w);
		return a.exec();
	}
	catch (const std::exception& e) {

		QMessageBox::critical(nullptr, QObject::tr("Error"), QObject::tr("Error: ") + e.what() + "\n\n" + QObject::tr("Application is shutting down."), QMessageBox::StandardButton::Ok);
		return 1;
	}
	catch (...) {

		QMessageBox::critical(nullptr, QObject::tr("Unknown Error"), QObject::tr("Unknown Error: ") + "\n\n" + QObject::tr("Application is shutting down."), QMessageBox::StandardButton::Ok);
		return 1;
	}

	return 0;
}
