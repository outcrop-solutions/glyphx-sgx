#ifndef APPLICATION_H
#define APPLICATION_H

#include "sgxgui_global.h"
#include <QtWidgets/QApplication>

namespace SynGlyphX {

    class SGXGUI_EXPORT Application : public QApplication
    {
        Q_OBJECT

    public:
        Application(int& argc, char** argv);
        ~Application();

        static void Setup(const QString& appName, const QString& appVersion);
        static void SetupIcons(const QIcon& windowIcon);

    private:

    };

} //namespace SynGlyphX

#endif // APPLICATION_H
