#include "glyphviewerwindow.h"
#include "glyphbuilderapplication.h"
#include <QtWidgets/QSplashScreen>
#include <QtCore/QTimer>
#include <QtCore/QDir>
#include "licensingdialog.h"
#include <QtCore/QDir>
#ifdef USE_BREAKPAD
#include "exception_handler.h"
#endif

#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtWidgets/QMessageBox>
#include <QtCore/QTextStream>
#include <QtCore/QString>
#include "nonmappablegeometryproperties.h"
#include "GVGlobal.h"

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

	// Note: there seems to be a Qt bug on Intel chipsets where Qt's internal shaders won't compile if you specifically
	// request a 3.3 context (even in compat profile!), causing the app to crash on startup. Instead we'll use the
	// default (which gets us the latest available compat profile and doesn't expose the bug, for reasons that are
	// somewhat unclear to me). This unfortunately means we can only use Core profiles in testing-- hopefully future Qt
	// or Intel graphics driver versions will fix this.
	QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
	fmt.setMajorVersion( 3 );
	fmt.setMinorVersion( 3 );

	// QPainter doesn't work with Core profiles, and we need to use QPainter to draw text. Once the bug in Qt is fixed,
	// or we switch to another text rendering method for 3D views, this should be changed to CoreProfile (which is faster
	// as well as being required by GPU profiling/debugging tools like nSight/renderdoc/etc).
	//fmt.setProfile( QSurfaceFormat::OpenGLContextProfile::CompatibilityProfile );
	fmt.setProfile( QSurfaceFormat::OpenGLContextProfile::CoreProfile );

	fmt.setDepthBufferSize( 24 );
	fmt.setSwapBehavior( QSurfaceFormat::SwapBehavior::DoubleBuffer );
	fmt.setSamples( 4 );
/*	fmt.setOption( QSurfaceFormat::StereoBuffers );
	fmt.setStereo( true );*/
	QSurfaceFormat::setDefaultFormat( fmt );
    
	SynGlyphX::GlyphBuilderApplication::Setup("Glyph Builder - Glyph Viewer Release Candidate 5", "0.8.00rc5");
	SynGlyphX::GlyphBuilderApplication a(argc, argv);
	if (SynGlyphX::GlyphBuilderApplication::IsGlyphEd()) {

		SynGlyphX::GlyphBuilderApplication::setApplicationName("GlyphEd");
	}

	//qInstallMessageHandler(myMessageHandler);

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

	qRegisterMetaType<SynGlyphX::GlyphGeometryInfo::Surface>("GlyphGeometryInfo::Surface");
	qRegisterMetaType<SynGlyphX::NonMappableGeometryProperties>("NonMappableGeometryProperties");
	qRegisterMetaType<SynGlyphX::ColorMinDiff>("ColorMinDiff");
	qRegisterMetaType<SynGlyphX::IntMinDiff>("IntMinDiff");
	qRegisterMetaType<SynGlyphX::DoubleMinDiff>("DoubleMinDiff");
	qRegisterMetaType<SynGlyphX::InputField>("InputField");

#ifdef USE_LICENSING
	if (!SynGlyphX::LicensingDialog::CheckLicense()) {

		return 0;
	}
#endif

	//Setup and show the splash screen
	QPixmap pixmap(SynGlyphX::GlyphBuilderApplication::GetSplashScreenLocation());
	QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);
	splash.show();

	splash.showMessage("Loading Glyph Viewer", Qt::AlignHCenter | Qt::AlignBottom);

	a.processEvents();

	try {
		//Need to figure out better way to not have the splash screen disappear before the user sees it
		QTimer::singleShot(1500, &splash, SLOT(close()));

		GlyphViewerWindow w;
		QFile file(":GlyphViewer/Resources/glyphed_stylesheet.qss"); //For building app 
		//QFile file("../../../GUI/GlyphViewer/Resources/glyphed_stylesheet.qss"); //For faster style testing
		file.open(QFile::ReadOnly);
		QString styleSheet = QLatin1String(file.readAll());
		w.setStyleSheet(styleSheet);
		w.move(50, 50);
		w.resize(1200, 700);

		GVGlobal::Init(new GVServices(&w));

		w.show();

		QStringList commandLineArguments = SynGlyphX::Application::arguments();
		if (commandLineArguments.size() > 1) {

			QDir visualizationToLoad(commandLineArguments[1]);

			if (!w.LoadNewVisualization(QDir::toNativeSeparators(visualizationToLoad.canonicalPath()))) {

				w.closeJVM();
				return 2;
			}
		}

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
