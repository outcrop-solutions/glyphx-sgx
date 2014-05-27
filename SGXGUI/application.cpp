#include "application.h"
#include <QtCore/QDir>
#include <QtGui/QIcon>

namespace SynGlyphX {

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
    }

    void Application::SetupIcons(const QIcon& windowIcon) {

		//Set the default application icon
        setWindowIcon(windowIcon);
    }

} //namespace SynGlyphX