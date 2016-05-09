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

#ifndef SYNGLYPHX_ELASTICLISTWIDGET_H
#define SYNGLYPHX_ELASTICLISTWIDGET_H

#include "sgxgui_global.h"
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTableView>
#include <set>
#include "elasticlistmodel.h"
#include <QtWidgets/QCheckBox>

namespace SynGlyphX {

	class SGXGUI_EXPORT ElasticListWidget : public QFrame
	{
		Q_OBJECT

	public:
		static const unsigned int MaximumNumberOfRowsShown;

		ElasticListWidget(QWidget *parent);
		~ElasticListWidget();

		bool HasSelection() const;

		void SetTitle(const QString& title);
		QString GetTitle() const;
		void SetData(const ElasticListModel::Data& data);
		//void SetSelectedData(const ElasticListModel::Data& data);

		const QSet<QString>& GetSelectedRawData() const;

	signals:
		void SelectionChanged();

	protected:
		bool eventFilter(QObject *obj, QEvent *ev);

	private slots:
		void OnNewUserSelection();
		void ResizeTable();

	private:
		void ChangeSelection();

		bool m_hasUserChangedSelectedItemsInTable;
		QSet<QString> m_selectedRawData;
		
		QLabel* m_title;
		QCheckBox* m_showAllCheckBox;
		QTableView* m_dataAndCountView;
		ElasticListModel* m_model;
	};

} //namespace SynGlyphX

#endif //SYNGLYPHX_ELASTICLISTWIDGET_H
