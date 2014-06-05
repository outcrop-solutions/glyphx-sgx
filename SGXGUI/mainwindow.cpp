#include "mainwindow.h"
#include <QtCore/QSettings>
#include <QtWidgets/QAction>
#include <QtCore/QFileInfo>
#include <QtWidgets/QMenu>
#include "application.h"

namespace SynGlyphX {

    MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
    {
        //Make sure Status Bar gets created for all applications
        statusBar();

        //Add an additional 
        for (int i = 0; i < MaxRecentFiles + 1; ++i) {
            QAction* recentFileAction = new QAction(this);
            recentFileAction->setVisible(false);
            if (i != 0) {
                QObject::connect(recentFileAction, &QAction::triggered, this, &MainWindow::OnRecentFileSelected);
            }
            m_recentFileActions.push_back(recentFileAction);
        }
        m_recentFileActions[0]->setSeparator(true);

        UpdateFilenameWindowTitle("Untitled");
    }

    MainWindow::~MainWindow()
    {

    }

    void MainWindow::ReadSettings() {

        QSettings settings;
        settings.beginGroup("Window");
        resize(settings.value("size", QSize(900, 720)).toSize());
        //restoreGeometry(settings.value("geometry").toByteArray());
        restoreState(settings.value("state").toByteArray());
        settings.endGroup();

        UpdateRecentFileList();
    }

    void MainWindow::WriteSettings() {

        QSettings settings;
        settings.beginGroup("Window");
        settings.setValue("size", size());
        //settings.setValue("geometry", saveGeometry());
        settings.setValue("state", saveState());
        settings.endGroup();
    }

    void MainWindow::closeEvent(QCloseEvent* event) {

        WriteSettings();
        QMainWindow::closeEvent(event);
    }

    void MainWindow::OnRecentFileSelected() {

        QAction *action = qobject_cast<QAction *>(sender());
        if (action != NULL) {
            LoadRecentFile(action->data().toString());
        }
    }

    void MainWindow::UpdateRecentFileList() {

        QSettings settings;
        settings.beginGroup("RecentFiles");
        QStringList files = settings.value("recentFileList").toStringList();
        settings.endGroup();

        int numRecentFiles = qMin(files.size(), static_cast<int>(MaxRecentFiles));

        for (int i = 1; i < numRecentFiles + 1; ++i) {
            QString text = tr("&%1 %2").arg(i).arg(QFileInfo(files[i - 1]).fileName());
            m_recentFileActions[i]->setText(text);
            m_recentFileActions[i]->setData(files[i - 1]);
            m_recentFileActions[i]->setVisible(true);
        }
        for (int j = numRecentFiles + 1; j < MaxRecentFiles + 1; ++j)
            m_recentFileActions[j]->setVisible(false);

        //If there are any files in the recent file list make the separator visible
        m_recentFileActions[0]->setVisible(numRecentFiles > 0);
    }

    void MainWindow::UpdateFilenameWindowTitle(const QString& title) {

        setWindowTitle(title + "[*] - " + SynGlyphX::Application::organizationName() + " " + SynGlyphX::Application::applicationName());
    }

    void MainWindow::SetCurrentFile(const QString& filename) {

        m_currentFilename = filename;

        QSettings settings;
        settings.beginGroup("RecentFiles");
        QStringList files = settings.value("recentFileList").toStringList();
        settings.endGroup();

        files.removeAll(filename);
        files.prepend(filename);
        while (files.size() > MaxRecentFiles) {
            files.removeLast();
        }

        settings.beginGroup("RecentFiles");
        settings.setValue("recentFileList", files);
        settings.endGroup();

        UpdateRecentFileList();
        UpdateFilenameWindowTitle(QFileInfo(filename).fileName());
        setWindowModified(false);
    }

    QAction* MainWindow::CreateMenuAction(QMenu* menu, const QString& title, QKeySequence shortcut) {

        QString titleWithShortcut = title;
#ifdef WIN32
        //ALT+F4 exits on windows but does not show up in the menu even though the key sequence exits the program so add the sequence to the title manually
        if (title == "Exit") {
            titleWithShortcut += "\tAlt+F4";
        } 
#endif
        QAction* action = menu->addAction(titleWithShortcut);
        action->setShortcut(shortcut);
        return action;
    }

} //namespace SynGlyphX