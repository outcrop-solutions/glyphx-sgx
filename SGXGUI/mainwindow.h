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

    protected slots:
        void SwitchBetweenFullAndNormalScreen();

    protected:
		virtual void showEvent(QShowEvent* event);
        virtual void closeEvent(QCloseEvent* event);

        virtual void ReadSettings();
        virtual void WriteSettings();

        virtual void LoadRecentFile(const QString& filename) = 0;

        void UpdateRecentFileList();
        void UpdateFilenameWindowTitle(const QString& title);
		void ClearCurrentFile();
        void SetCurrentFile(const QString& filename);
        QAction* CreateMenuAction(QMenu* menu, const QString& title, QKeySequence shortcut = QKeySequence());
        void CreateHelpMenu();
        void CreateFullScreenAction(QMenu* menu);

        QList<QAction*> m_recentFileActions;
        QString m_currentFilename;
        QMenu* m_helpMenu;
        QAction* m_fullScreenAction;

    private slots:
        void OnRecentFileSelected();
        void ShowAboutBox();

	private:
		bool m_needToReadSettings;
    };

} //namespace SynGlyphX

#endif // MAINWINDOW_H
