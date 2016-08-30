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
#ifndef TABLECHOICEWIDGET_H
#define TABLECHOICEWIDGET_H

#include <QtWidgets/QWidget>
#include "radiobuttongroupwidget.h"
#include <QtWidgets/QListWidget>
#include <QtWidgets/QComboBox>
#include <unordered_map>

class TableChoiceWidget : public QWidget
{
	Q_OBJECT

public:
	typedef QMap<QString, QStringList> Table2ForiegnKeyTablesMap;

	TableChoiceWidget(QWidget *parent);
	~TableChoiceWidget();

	void SetTables(const QStringList& mainTables, Table2ForiegnKeyTablesMap foriegnKeyTablesMap = Table2ForiegnKeyTablesMap());
	QStringList GetChosenMainTables() const;
	QStringList GetChosenForiegnTables() const;
	bool IsInJoinedTableMode() const;

private slots:
	void OnNewMainTableInJoinedModeSelected(const QString& mainTable);

private:
	QStringList GetSelectedTables(const QListWidget* const listWidget) const;

	SynGlyphX::RadioButtonGroupWidget* m_choiceTypeRadioButtonWidget;

	//Multi base table choice widgets
	QListWidget* m_baseTableListWidget;

	//Joined table widgets
	QComboBox* m_mainTableComboBox;
	QListWidget* m_linkedTablesListWidget;

	QStringList m_mainTables;
	Table2ForiegnKeyTablesMap m_foriegnKeyTablesMap;
};

#endif // TABLECHOICEWIDGET_H
