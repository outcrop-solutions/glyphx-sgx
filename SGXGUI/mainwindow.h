#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sgxgui_global.h"
#include <QtWidgets/QMainWindow>

namespace SynGlyphX {

    const unsigned int MaxRecentFiles = 4;

    class SGXGUI_EXPORT MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow(QWidget *parent = 0);
        ~MainWindow();

    protected:
        virtual void closeEvent(QCloseEvent* event);

        virtual void ReadSettings();
        virtual void WriteSettings();

        virtual void LoadRecentFile(const QString& filename) = 0;

        void UpdateRecentFileList();
        void UpdateFilenameWindowTitle(const QString& title);
        void SetCurrentFile(const QString& filename);

        QList<QAction*> m_recentFileActions;
        QString m_currentFilename;

    private slots:
        void OnRecentFileSelected();
    };

} //namespace SynGlyphX

#endif // MAINWINDOW_H
