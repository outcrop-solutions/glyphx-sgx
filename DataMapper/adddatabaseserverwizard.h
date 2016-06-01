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
#pragma once
#ifndef ADDDATABASESERVERWIDGET_H
#define ADDDATABASESERVERWIDGET_H

#include <QtWidgets/QWizard>
#include "dataengineconnection.h"
#include <QtWidgets/QListWidget>
#include "tablechoicewidget.h"
#include "databaseserverinfowidget.h"
#include "databaseserverdatasource.h"

class AddDatabaseServerWizard : public QWizard
{
	Q_OBJECT

public:
	enum PageType {
		DatabaseInfoPage = 0,
		SchemaSelectionPage,
		TableSelectionPage
	};

	AddDatabaseServerWizard(DataEngine::DataEngineConnection::SharedPtr dataEngineConnection, QWidget *parent);
	~AddDatabaseServerWizard();

	//void SetValues(const SynGlyphX::DatabaseServerDatasource& datasource);
	SynGlyphX::DatabaseServerDatasource GetValues() const;

	virtual int nextId() const;
	virtual bool validateCurrentPage();

protected:
	virtual void initializePage(int id);

private:
	void CreateDatabaseInfoPage();
	void CreateSchemaSelectionPage();
	void CreateTableSelectionPage();

	bool ValidateDatabaseInfo();
	bool DoesSchemaHaveTables(const QString& schema) const;

	DataEngine::DataEngineConnection::SharedPtr m_dataEngineConnection;
	QStringList m_schemas;
	//QMap<QString, 

	//Database Info Page Widgets
	SynGlyphX::DatabaseServerInfoWidget* m_databaseServerInfoWidget;

	//Schema selection page widgets
	QListWidget* m_schemaListWidget;

	//Table choice widget
	TableChoiceWidget* m_tableChoiceWidget;
};

#endif // ADDDATABASESERVERWIDGET_H
