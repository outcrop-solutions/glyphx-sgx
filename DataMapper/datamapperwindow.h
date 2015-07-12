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
#include "databindingwidget.h"
#include "minmaxglyphmodel.h"
#include "datatransformmodel.h"
#include "antzwidget.h"
#include "datamapping3dwidget.h"
#include "glyphtreesview.h"
#include "roledatafilterproxymodel.h"
#include "baseobjectlistview.h"

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
    void ExportToANTz(const QString& templateDir);
	void AddBaseObject();
	void AddGlyphTemplate();
	void CreateNewGlyphTree();
	void ChangeMapDownloadSettings();
	void ChangeGlyphDefaults();
	void ChangeNewMappingDefaults();
	void OnGlyphTreesViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void ChangeSceneProperties();
	void ChangeOptions();

private:
    void CreateMenus();
    void CreateDockWidgets();
	void CreateCenterWidget();
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
    QMenu* m_glyphMenu;
	QMenu* m_baseObjectMenu;
	QMenu* m_datasourceMenu;
    QMenu* m_viewMenu;
	QMenu* m_toolsMenu;

	QAction* m_showAnimation;
    
	SynGlyphX::RoleDataFilterProxyModel* m_baseObjectsModel;
	BaseObjectListView* m_baseObjectsView;
	GlyphTreesView* m_glyphTreesView;
	DataSourceStatsWidget* m_dataSourceStats;

	QList<QAction*> m_projectDependentActions;

	DataBindingWidget* m_dataBindingWidget;
	MinMaxGlyphModel* m_minMaxGlyphModel;

	DataTransformModel* m_dataTransformModel;
	
	DataMapping3DWidget* m_minMaxGlyph3DWidget;

	SynGlyphX::DataMappingDefaults m_newMappingDefaults;
	SynGlyphX::SceneProperties m_newMappingSceneProperties;

	QString m_antzExportDirectory;
	QString m_antzzSpaceExportDirectory;
};

#endif // DATAMAPPERWINDOW_H
