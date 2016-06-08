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
#ifndef DATAMAPPING3DWIDGET_H
#define DATAMAPPING3DWIDGET_H

#include "minmaxglyph3dwidget.h"
#include <stack>

namespace SynGlyphX {
	class DataTransformModel;
	class RoleDataFilterProxyModel;
}

class DataMapping3DWidget : public SynGlyphXANTz::MinMaxGlyph3DWidget
{
	Q_OBJECT

public:
	DataMapping3DWidget(SynGlyphX::DataTransformModel* model, QWidget *parent = 0);
	~DataMapping3DWidget();

	virtual void SetModel(SynGlyphX::RoleDataFilterProxyModel* model, QItemSelectionModel* selectionModel);

private slots:
	void OnInternalSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void OnExternalSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void OnExternalDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
	void OnExternalModelReset();
	void OnExternalRowsRemoved(const QModelIndex& parent, int first, int last);
	void OnExternalRowsInserted(const QModelIndex& parent, int first, int last);

private:
	void Clear();
	void ConnectInternalSelection();
	void ConnectExternalSelection();
	std::stack<unsigned int> GetRowIndiciesFromStack(const QModelIndex& index) const;
	QModelIndex GetModelIndexFromStack(std::stack<unsigned int> rows, const QAbstractItemModel* const model) const;
	QModelIndex GetInternalModelIndex(const QModelIndex& externalIndex) const;
	QModelIndexList GetValidExternalIndexes(const QModelIndexList& indexList) const;

	SynGlyphX::DataTransformModel* m_dataTransformModel;

	SynGlyphXANTz::MinMaxGlyphTreeModel* m_internalModel;
	QItemSelectionModel* m_internalSelectionModel;

	SynGlyphX::RoleDataFilterProxyModel* m_externalModel;
	QItemSelectionModel* m_externalSelectionModel;

	QMetaObject::Connection m_internalSelectionConnection;
	QMetaObject::Connection m_externalSelectionConnection;

	int m_glyphTreeIndex;
};

#endif // DATAMAPPING3DWIDGET_H
