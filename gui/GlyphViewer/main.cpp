#include "glyphviewerwindow.h"
#include "glyphbuilderapplication.h"
#include <QtWidgets/QSplashScreen>
#include <QtCore/QTimer>
#include <QtCore/QDir>
#include "application.h"
#include <QtCore/QStandardPaths>
#include <QtWidgets/QMessageBox>
#include <QtCore/QTextStream>
#include <QtCore/QString>
#include "nonmappablegeometryproperties.h"
#include "GVGlobal.h"
#include "version.h"

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

	QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
	fmt.setMajorVersion(3);
	fmt.setMinorVersion(3);

	// Now that we have our own font rendering instead of relying on QPainter, we can use a core profile!
	fmt.setProfile(QSurfaceFormat::OpenGLContextProfile::CompatibilityProfile);
	
	fmt.setDepthBufferSize(24);
	fmt.setSwapBehavior(QSurfaceFormat::SwapBehavior::DoubleBuffer);
	fmt.setSamples(4);
	QSurfaceFormat::setDefaultFormat(fmt);
	
	SynGlyphX::GlyphBuilderApplication::Setup("GlyphX", SynGlyphX::getFullVersionString().c_str());
	SynGlyphX::GlyphBuilderApplication a(argc, argv);
	QDir commonDataDir(SynGlyphX::GlyphBuilderApplication::GetCommonDataLocation());
	commonDataDir.mkdir("GlyphX");

	SynGlyphX::GlyphBuilderApplication::SetupIconsAndLogos();

	qRegisterMetaType<SynGlyphX::GlyphGeometryInfo::Surface>("GlyphGeometryInfo::Surface");
	qRegisterMetaType<SynGlyphX::NonMappableGeometryProperties>("NonMappableGeometryProperties");
	qRegisterMetaType<SynGlyphX::ColorMinDiff>("ColorMinDiff");
	qRegisterMetaType<SynGlyphX::IntMinDiff>("IntMinDiff");
	qRegisterMetaType<SynGlyphX::DoubleMinDiff>("DoubleMinDiff");
	qRegisterMetaType<SynGlyphX::InputField>("InputField");

	//Setup and show the splash screen
	QPixmap pixmap(SynGlyphX::GlyphBuilderApplication::GetSplashScreenLocation());
	QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);
	splash.show();

	splash.showMessage("Loading GlyphX", Qt::AlignHCenter | Qt::AlignBottom);

	a.processEvents();

	try {
		//Need to figure out better way to not have the splash screen disappear before the user sees it

		QTimer::singleShot(1500, &splash, SLOT(close()));

		QString model = nullptr;
		QString address = "https://app.glyphx.co/";
		QStringList commandLineArguments = SynGlyphX::Application::arguments();
		for (QString arg : commandLineArguments) {
			if (arg.contains("url")) {
				address = arg.split("=")[1];
			}
			else if (arg.contains("model")) {
				QStringList splt = arg.split("=");
				if (splt.size() > 2) {
					model = splt[2];
				}
				else {
					model = splt[1];
				}
				address += "?model=" + model;
			}
		}

		GlyphViewerWindow w(address, model);
		QFile file(":GlyphViewer/Resources/glyphed_stylesheet.qss"); //For building app 
		//QFile file("../../../GUI/GlyphViewer/Resources/glyphed_stylesheet.qss"); //For faster style testing
		file.open(QFile::ReadOnly);
		QString styleSheet = QLatin1String(file.readAll());
		w.setStyleSheet(styleSheet);
		w.move(50, 50);
		w.resize(1200, 700);

		GVGlobal::Init(new GVServices(&w));

		w.show();

		if (commandLineArguments.size() > 1) {

			QDir visualizationToLoad(commandLineArguments[1]);

			if (!w.LoadNewVisualization(QDir::toNativeSeparators(visualizationToLoad.canonicalPath()))) {

				return 2;
			}
		}

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
