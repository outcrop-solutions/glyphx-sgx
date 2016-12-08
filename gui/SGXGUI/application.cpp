#include "application.h"
#include <stdexcept>
#include <QtCore/QDir>
#include <QtGui/QIcon>
#include <QtCore/QStandardPaths>
#include <QtWidgets/QDesktopWidget>

namespace SynGlyphX {

	QString Application::s_commonDataLocation;	
	QString Application::s_tempDirectory = QDir::tempPath();
	QString Application::s_writeableLocation;
    
	Application::Application(int& argc, char** argv)
        : QApplication(argc, argv)
    {
		//The current working directory should always be where the executable is
		QDir::setCurrent(applicationDirPath());
    }

    Application::~Application()
    {

    }

    void Application::Setup(const QString& appName, const QString& appVersion) {

		//One we are on Qt 5.3 or later uncomment the following line
		//setAttribute(Qt::AA_UseDesktopOpenGL, true);
		setAttribute( Qt::AA_ShareOpenGLContexts, true );

        setOrganizationName("SynGlyphX");
        setOrganizationDomain("www.synglyphx.com");

        setApplicationName(appName);
        setApplicationVersion(appVersion);
		s_tempDirectory = QDir::tempPath() + QDir::separator() + organizationName() + QDir::separator() + applicationName();
		QDir dir;
		if (!dir.mkpath(s_tempDirectory)) {

			throw std::runtime_error("Failed to create temp directory");
		}

		QString baseCommonDataPath;

#ifdef WIN32
		QStringList paths = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
		Q_FOREACH(QString path, paths) {

			if (path.contains("ProgramData")) {

				baseCommonDataPath = path;
				break;
			}
		}

		if (baseCommonDataPath.isEmpty()) {

			baseCommonDataPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
		}
		
#else
		baseCommonDataPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
#endif

		s_commonDataLocation = QDir::toNativeSeparators(baseCommonDataPath + '/' + organizationName());
		s_writeableLocation = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + '/' + organizationName() + '/' + applicationName());

		QDir commonDataDir(s_commonDataLocation);
		if (!commonDataDir.exists()) {

			commonDataDir.mkpath(s_commonDataLocation);
		}
    }

    void Application::SetupIcons(const QIcon& windowIcon) {

		//Set the default application icon
        setWindowIcon(windowIcon);
    }

	QString Application::GetApplicationVersionMajorNumber() {

		QString version = applicationVersion();
		return version.left(version.indexOf('.'));
	}

	const QString& Application::GetCommonDataLocation() {

		return s_commonDataLocation;
	}

	const QString& Application::GetAppTempDirectory() {

		return s_tempDirectory;
	}

	const QString& Application::GetWriteableLocation() {

		return s_writeableLocation;
	}

	void Application::SetOverrideCursorAndProcessEvents(const QCursor& cursor, QEventLoop::ProcessEventsFlags flags) {

		setOverrideCursor(cursor);
		processEvents(flags);
	}

	QSize Application::DynamicQSize(int x, int y) {

		QRect screen = QApplication::desktop()->availableGeometry(1);
		float w = (float)x*(screen.width() / 1920.0);
		float h = (float)y*(screen.height() / 1040.0);

		//return QSize((int)w, (int)h);
		return QSize(x, y);
	}

} //namespace SynGlyphX
