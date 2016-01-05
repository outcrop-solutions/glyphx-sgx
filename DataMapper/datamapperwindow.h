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

#ifndef DATAMAPPERWINDOW_H
#define DATAMAPPERWINDOW_H

#include "mainwindow.h"
#include <QtWidgets/QListView>
#include <boost/shared_ptr.hpp>
#include "datasourcestatswidget.h"
#include "databindingtableswidget.h"
#include "glyphrolestablemodel.h"
#include "datatransformmodel.h"
#include "antzwidget.h"
#include "datamapping3dwidget.h"
#include "glyphtreesview.h"
#include "roledatafilterproxymodel.h"
#include "baseobjectlistview.h"
#include "datasourcesview.h"
#include "antzcsvwriter.h"
#include "dataengineconnection.h"
#include "glyphengine.h"

class DataMapperWindow : public SynGlyphX::MainWindow
{
    Q_OBJECT

public:
    DataMapperWindow(QWidget *parent = 0);
    ~DataMapperWindow();

protected:
    virtual bool LoadRecentFile(const QString& filename);
	virtual void closeEvent(QCloseEvent* event);

	virtual void ReadSettings();
	virtual void WriteSettings();

private slots:
    void CreateNewProject();
    void OpenProject();
    bool SaveProject();
    bool SaveAsProject();
    void AddDataSources();
	void ExportToANTz(SynGlyphXANTz::ANTzCSVWriter::OutputPlatform platform);
	void AddBaseObject();
	void AddGlyphTemplate();
	void CreateNewGlyphTree();
	void ChangeMapDownloadSettings();
	void ChangeGlyphDefaults();
	void ChangeNewMappingDefaults();
	void ChangeSceneProperties();
	void ChangeOptions();

private:
    void CreateMenus();
	void CreateExportToPortableVisualizationSubmenu();
    void CreateDockWidgets();
	void CreateCenterWidget();
	void UpdateMissingFiles(const QString& mappingFilename);
	bool LoadDataTransform(const QString& filename);
	bool SaveDataTransform(const QString& filename);
	bool AskUserToSave();
	void EnableProjectDependentActions(bool enable);
	bool ValidateNewDatasource(const QString& datasource);
	bool DoesANTzTemplateExist(const QString& templateDir) const;
	void ProcessCSVFile(const QString& csvFile);
	void ReadNewMappingDefaults();
	void WriteNewMappingDefaults();
	void ClearAndInitializeDataMapping();
	void SelectFirstBaseObject();

    QMenu* m_fileMenu;
	QMenu* m_editMenu;
    QMenu* m_glyphMenu;
	QMenu* m_baseObjectMenu;
	QMenu* m_datasourceMenu;
	QMenu* m_toolsMenu;

	QAction* m_showAnimation;
    
	SynGlyphX::RoleDataFilterProxyModel* m_baseObjectsModel;
	BaseObjectListView* m_baseObjectsView;
	GlyphTreesView* m_glyphTreesView;
	DataSourceStatsWidget* m_dataSourceStats;
	DataSourcesView* m_dataSourcesView;

	QList<QAction*> m_projectDependentActions;

	DataBindingTablesWidget* m_dataBindingWidget;
	GlyphRolesTableModel* m_glyphRolesTableModel;

	DataTransformModel* m_dataTransformModel;
	
	DataMapping3DWidget* m_minMaxGlyph3DWidget;

	SynGlyphX::DataMappingDefaults m_newMappingDefaults;
	SynGlyphX::SceneProperties m_newMappingSceneProperties;

	std::unordered_map<SynGlyphXANTz::ANTzCSVWriter::OutputPlatform, QString> m_antzExportDirectories;

	QMetaObject::Connection m_modelResetConnection;
	DataEngine::DataEngineConnection dec;
};

#endif // DATAMAPPERWINDOW_H
