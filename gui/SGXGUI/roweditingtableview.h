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
#ifndef SYNGLYPHX_ROWEDITINGTABLEVIEW_H
#define SYNGLYPHX_ROWEDITINGTABLEVIEW_H

#include "sgxgui_global.h"
#include <QtWidgets/QWidget>

class QPushButton;
class QTableView;
class QHBoxLayout;

namespace SynGlyphX {

	class SGXGUI_EXPORT RowEditingTableView : public QWidget
	{
		Q_OBJECT

	public:
		RowEditingTableView(QWidget *parent);
		~RowEditingTableView();

	protected slots:
		void OnAddRows();
		void OnRemoveSelectedRows();
		void OnRemoveAllRows();

		//virtual void OnMoveUpRow() = 0;
		//virtual void OnMoveDownRow() = 0;

	protected:
		virtual void UpdateEnabledStates();
		virtual void AddRows() = 0;

		QHBoxLayout* m_topButtonLayout;
		QPushButton* m_addButton;
		QPushButton* m_removeAllButton;
		//QPushButton* m_moveUpButton;
		//QPushButton* m_moveDownButton;

		QTableView* m_tableView;
		QAction* m_removeSelectedContextMenuAction;
		QAction* m_removeAllContextMenuAction;
		QAction* m_moveRowUpContextMenuAction;
		QAction* m_moveRowDownContextMenuAction;
	};

} //namespace SynGlyphX

#endif // SYNGLYPHX_ROWEDITINGTABLEVIEW_H
