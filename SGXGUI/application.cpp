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

        addLibraryPath(QDir::currentPath() + QDir::separator() + "qt_plugins");
        //Set the default application icon
        
        setOrganizationName("SynGlyphX");
        setOrganizationDomain("www.synglyphx.com");

        setApplicationName(appName);
        setApplicationVersion(appVersion);
    }

    void Application::SetupIcons(const QIcon& windowIcon) {

        setWindowIcon(windowIcon);
    }

} //namespace SynGlyphX