#include "glyphdesignerwindow.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QSplashScreen>
#include <QtCore/QTimer>
#include "glyphbuilderapplication.h"
#include "licensingdialog.h"
#include "exception_handler.h"
#include <QtCore/QStandardPaths>
#include <QtWidgets/QMessageBox>
#include <boost/filesystem.hpp>

int main(int argc, char *argv[])
{
    SynGlyphX::GlyphBuilderApplication::Setup("Glyph Builder - Glyph Designer", "0.7.43");
	SynGlyphX::GlyphBuilderApplication a(argc, argv);

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

	SynGlyphX::GlyphBuilderApplication::SetupIconsAndLogos();

	if (!SynGlyphX::LicensingDialog::CheckLicense()) {

		return 0;
	}

	//Setup and show the splash screen
	QPixmap pixmap(SynGlyphX::GlyphBuilderApplication::GetSplashScreenLocation());
	QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);
	splash.show();

    splash.showMessage("Loading Glyph Designer", Qt::AlignHCenter | Qt::AlignBottom);

    a.processEvents();

	try {

		GlyphDesignerWindow w;
		w.resize(1200, 700);

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
