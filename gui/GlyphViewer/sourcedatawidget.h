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

#ifndef SOURCEDATAWIDGET_H
#define SOURCEDATAWIDGET_H

#include <QtWidgets/QTabWidget>
#include "sourcedatacache.h"
#include <QtWidgets/QStatusBar>
#include "datatransformmapping.h"
#include <QtWidgets/QLabel>

class QTableView;

class SourceDataWidget : public QWidget
{
	Q_OBJECT

public:
	SourceDataWidget(SourceDataCache::ConstSharedPtr sourceDataCache, SynGlyphX::DataTransformMapping::ConstSharedPtr dataTransformMapping, QWidget *parent = nullptr);
	~SourceDataWidget();

	void SetLoadSubsetVisualization(bool loadSubsetVisualization);
	bool GetLoadSubsetVisualization() const;
	void SetLoadSubsetVisualizationInNewInstance(bool loadSubsetVisualizationInNewInstance);
	bool GetLoadSubsetVisualizationInNewInstance() const;

	virtual void OnNewVisualization();

signals:
	void WindowHidden();
	void SubsetVisualizationCreated(const QString& newSubsetVisualizationFilename);

public slots:
	void CreateSubsetVisualization();
	void UpdateTables();
	void SetCount();

protected slots:
	void SaveCurrentTabToFile();

protected:
	void closeEvent(QCloseEvent* event) override;
	virtual SynGlyphX::IndexSet GetSourceIndexesForTable(const QString& table) = 0;
	virtual bool DoesEmptyFilterShowAll() const = 0;

	void DeleteTabs();
	void ReadSettings();
	void WriteSettings();

	void WriteToFile(QTableView* tableView, const QString& filename);

	QTabWidget* m_sourceDataTabs;
	QStatusBar* m_statusBar;
	std::vector<QTableView*> m_tableViews;
	QMap<QString, SynGlyphX::InputTable> m_tableInfoMap;
	QMap<QString, SourceDataCache::TableColumns> m_tableColumns;
	QMap<QString, QSqlQueryModel*> m_sqlModels;
	SourceDataCache::ConstSharedPtr m_sourceDataCache;
	SynGlyphX::DataTransformMapping::ConstSharedPtr m_dataTransformMapping;
	QLabel* countLabel;

	virtual const char* getDefaultExportedDataFileName() const { return "selectedsourcedata.csv"; }
};

#endif // SOURCEDATAWIDGET_H
