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

#ifndef MODALGLYPHWIDGET_H
#define MODALGLYPHWIDGET_H

#include "glyphpropertieswidget.h"
#include "minmaxglyphtreemodel.h"
#include <QtCore/QItemSelectionModel>
#include "propertyupdate.h"

class ModalGlyphWidget : public SynGlyphX::GlyphPropertiesWidget
{
    Q_OBJECT

public:
	ModalGlyphWidget(SynGlyphXANTz::MinMaxGlyphTreeModel::GlyphType glyphTreeType, QWidget *parent = 0);
    ~ModalGlyphWidget();

	void SetModel(SynGlyphXANTz::MinMaxGlyphTreeModel* model, QItemSelectionModel* selectionModel);

private slots:
	void OnGlyphUpdated(const QModelIndex& index);
	void OnWidgetUpdated(SynGlyphX::PropertyUpdates updates);
    void SelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:
    void ConnectWidgetSignals();
    void DisconnectWidgetSignals();
	void UpdateWidget(const QModelIndex& index);
    
	SynGlyphXANTz::MinMaxGlyphTreeModel::GlyphType m_glyphTreeType;
	SynGlyphXANTz::MinMaxGlyphTreeModel* m_model;
    QItemSelectionModel* m_selectionModel;

    std::vector<QMetaObject::Connection> m_propertyConnections;
	QMetaObject::Connection m_glyphUpdateConnection;
	QMetaObject::Connection m_selectionConnection;
};

#endif // MODALGLYPHWIDGET_H
