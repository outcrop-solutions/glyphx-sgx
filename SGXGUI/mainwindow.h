///
/// SynGlyphX Holdings Incorporated ("COMPANY") CONFIDENTIAL
/// Copyright (c) 2013-2015 SynGlyphX Holdings Incorporated, All Rights Reserved.
///
/// NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
/// herein are proprietary to COMPANY and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
/// Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
/// from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed 
/// Confidentiality and Non-disclosure agreements explicitly covering such access.
///
/// The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes  
/// information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
/// OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE 
/// LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  
/// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
///

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
