#include "glyphviewerwindow.h"
#include "glyphbuilderapplication.h"
#include <QtWidgets/QSplashScreen>
#include <QtCore/QTimer>
#include "licensingdialog.h"
#include "exception_handler.h"
#include <QtCore/QStandardPaths>
#include <QtWidgets/QMessageBox>
#include <QtCore/QTextStream>

/*
void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	QString txt;
	switch (type) {
	case QtDebugMsg:
		txt = QString("Debug: %1\n").arg(msg);
		break;
	case QtWarningMsg:
		txt = QString("Warning: %1\n").arg(msg);
		break;
	case QtCriticalMsg:
		txt = QString("Critical: %1\n").arg(msg);
		break;
	case QtFatalMsg:
		txt = QString("Fatal: %1\n").arg(msg);
		break;
	}
	QFile outFile(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QDir::separator() + "sgx_gv_log.txt");
	outFile.open(QIODevice::WriteOnly | QIODevice::Append);
	QTextStream ts(&outFile);
	ts << txt;
}
*/

int main(int argc, char *argv[])
{
	SynGlyphX::GlyphBuilderApplication::Setup("Glyph Builder - Glyph Viewer", "0.7.36");
	SynGlyphX::GlyphBuilderApplication a(argc, argv);

	//qInstallMessageHandler(myMessageHandler);

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

	splash.showMessage("Loading Glyph Viewer", Qt::AlignHCenter | Qt::AlignBottom);

	a.processEvents();

	try {

		GlyphViewerWindow w;
		w.move(50, 50);
		w.resize(1200, 700);

		//Need to figure out better way to not have the splash screen disappear before the user sees it
		QTimer::singleShot(1500, &splash, SLOT(close()));
		w.show();
		//QTimer::singleShot(1600, &w, SLOT(show()));

		//w.show();
		//splash.finish(&w);

		return a.exec();
		w.closeJVM();
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
