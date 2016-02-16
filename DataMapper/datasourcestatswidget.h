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

#ifndef DATASOURCESTATSWIDGET_H
#define DATASOURCESTATSWIDGET_H

#include <QtWidgets/QTabWidget>
#include <QtSql/QSqlDatabase>
#include <QtWidgets/QTableView>
#include <unordered_map>
#include "datatransformmodel.h"
#include "roledatafilterproxymodel.h"
#include "dataengineconnection.h"
#include "uuid.h"

class DataStatsModel;

class DataSourceStatsWidget : public QTabWidget
{
	Q_OBJECT

public:
	DataSourceStatsWidget(DataTransformModel* dataTransformModel, QWidget *parent = 0);
	~DataSourceStatsWidget();

	virtual QSize sizeHint() const;

	void SetDataEngineConnection(DataEngine::DataEngineConnection::SharedPtr dataEngineConnection);
	void AddNewStatsViews();
	void ClearTabs();

public slots:
	void RebuildStatsViews();

private slots:
	void OnRowsRemovedFromModel(const QModelIndex& parent, int start, int end);

private:
	void CreateTablesFromDatasource(const boost::uuids::uuid& id, int place, QString file, SynGlyphX::FileDatasource::SourceType type);
	void CreateTableView(DataStatsModel* model, const QString& tabName, const QString& id);
	void RemoveTableViews(const QString& name = QString());

	DataTransformModel* m_model;
	DataEngine::DataEngineConnection::SharedPtr m_dataEngineConnection;
};

#endif // DATASOURCESTATSWIDGET_H
