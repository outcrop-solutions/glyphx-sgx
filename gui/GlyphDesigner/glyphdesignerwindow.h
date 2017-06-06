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

#ifndef GLYPHDESIGNERWINDOW_H
#define GLYPHDESIGNERWINDOW_H

#include "mainwindow.h"
#include "glyphtreeview.h"
#include "antzsingleglyphtreewidget.h"
#include "minmaxglyphtreemodel.h"
#include "dataengineconnection.h"
#include "userlogindialog.h"

class GDServicesImpl;
class GlyphDesignerWindow : public SynGlyphX::MainWindow
{
	Q_OBJECT
		friend class GDServicesImpl;
public:
    GlyphDesignerWindow(QWidget *parent = 0);
    ~GlyphDesignerWindow();
	void closeJVM();

protected:
    virtual bool LoadRecentFile(const QString& filename);
    virtual void closeEvent(QCloseEvent* event);

	virtual void ReadSettings();
	virtual void WriteSettings();

private slots:
    void CreateNewGlyphTree();
    void ExportToCSV();
    bool SaveTemplate();
    bool SaveAsTemplate();
    void OpenTemplate();
	void OpenTemplateFromLibrary();
    void EditingModeChanged(QAction* action);
	void OnModelChanged();
	void ChangeGlobalOptions();

private:
    void CreateMenus();
    void CreateDockWidgets();
    bool LoadTemplate(const QString& filename);
    bool SaveTemplateFile(const QString& filename);
    bool AskUserToSave();
	void SelectRootGlyphInModel();
	bool IsUserLoggedIn();
	void Login();
	void UpdateUserMenu();
	void Logout();

	QDialog* loginDialog;
	DataEngine::UserLoginDialog* loginWidget;
	DataEngine::DataEngineConnection::SharedPtr m_dataEngineConnection;

	SynGlyphXANTz::ANTzSingleGlyphTreeWidget* m_3dView;
    GlyphTreeView* m_treeView;

    QMenu* m_fileMenu;
    QMenu* m_glyphMenu;
	QMenu* m_toolsMenu;

	QAction* m_showAnimation;

	SynGlyphXANTz::MinMaxGlyphTreeModel* m_glyphTreeModel;
	QItemSelectionModel* m_sharedSelectionModel;

	bool m_isFileLoadingOrDefaultGlyphSet;
};

#endif // GLYPHDESIGNERWINDOW_H
