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

#ifndef FIELDGROUPMODEL_H
#define FIELDGROUPMODEL_H

#include <QtCore/QAbstractTableModel>
#include "inputfield.h"
#include "sourcedatamanager.h"
#include "datatransformmodel.h"
#include <vector>
#include "utilitytypes.h"

class FieldGroupModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	FieldGroupModel(QObject *parent);
	~FieldGroupModel();

	virtual Qt::ItemFlags flags(const QModelIndex& index) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	void SetCheckedItems(const SynGlyphX::FieldGroup& fieldGroup);
	void UncheckAllItems();

	void ResetTable(DataTransformModel* model);
	const SynGlyphX::FieldGroup& GetCheckedItems() const;

	SynGlyphX::AllSomeNone AreFieldsChecked() const;

private:
	SynGlyphX::InputField GetInputFieldForRow(int row) const;
	const SynGlyphX::InputTable& GetTableForRow(int row) const;

	SynGlyphX::FieldGroup m_checkedItems;
	DataTransformModel* m_dataTransformModel;
	std::vector<unsigned int> m_countOfFieldsPerTable;
};

#endif // FIELDGROUPMODEL_H
