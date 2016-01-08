#include "glyphviewerwindow.h"
#include "glyphbuilderapplication.h"
#include <QtWidgets/QSplashScreen>
#include <QtCore/QTimer>
#include "licensingdialog.h"
#include "exception_handler.h"
#include <QtCore/QStandardPaths>
#include <QtWidgets/QMessageBox>

int main(int argc, char *argv[])
{
	SynGlyphX::GlyphBuilderApplication::Setup("Glyph Builder - Glyph Viewer", "0.7.22");
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

	SynGlyphX::GlyphBuilderApplication::SetupIcons(QIcon(":SGXGUI/Resources/synglyphx_x.ico"));

	if (!SynGlyphX::LicensingDialog::CheckLicense()) {

		return 0;
	}

	//Setup and show the splash screen
	QPixmap pixmap(":SGXGUI/Resources/synglyphx_splash.png");
	QSplashScreen splash;
	splash.setPixmap(pixmap);
	splash.show();

	splash.showMessage("Loading Glyph Viewer", Qt::AlignHCenter | Qt::AlignBottom);

	a.processEvents();

	try {
		GlyphViewerWindow w;
		w.move(50, 50);
		w.resize(1200, 700);

		//Need to figure out better way to not have the splash screen disappear before the user sees it
		QTimer::singleShot(1500, &splash, SLOT(close()));
		QTimer::singleShot(1600, &w, SLOT(show()));

		//w.show();
		//splash.finish(&w);

		return a.exec();
		w.closeJVM();
	}
	catch (...) {
		return 1;
	}
}
