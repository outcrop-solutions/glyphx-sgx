#include "application.h"
#include <QtCore/QDir>
#include <QtGui/QIcon>

namespace SynGlyphX {

	QString Application::s_tempDirectory = QDir::tempPath();

    Application::Application(int& argc, char** argv)
        : QApplication(argc, argv)
    {

    }

    Application::~Application()
    {

    }

    void Application::Setup(const QString& appName, const QString& appVersion) {

        setOrganizationName("SynGlyphX");
        setOrganizationDomain("www.synglyphx.com");

        setApplicationName(appName);
        setApplicationVersion(appVersion);

		s_tempDirectory = QDir::tempPath() + QDir::separator() + organizationName() + QDir::separator() + applicationName();
		QDir dir;
		if (!dir.mkpath(s_tempDirectory)) {

			throw std::exception("Failed to create temp directory");
		}
    }

    void Application::SetupIcons(const QIcon& windowIcon) {

		//Set the default application icon
        setWindowIcon(windowIcon);
    }

	const QString& Application::GetAppTempDirectory() {

		return s_tempDirectory;
	}

} //namespace SynGlyphX