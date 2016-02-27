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

#ifndef TABLECHOICEMODEL_H
#define TABLECHOICEMODEL_H

#include "abstracttreemodel.h"
#include <map>
#include <vector>

class TableChoiceModel : public SynGlyphX::AbstractTreeModel
{
	Q_OBJECT

public:
	typedef std::pair<QString, bool> TableEntry;
	typedef std::vector<TableEntry> TableEntries;
	typedef std::map<unsigned int, TableEntries> ForiegnKeyLinkedTableEntries;

	TableChoiceModel(bool includeCheckboxes, QObject *parent);
	~TableChoiceModel();

	void SetTables(const QStringList& tables, const ForiegnKeyLinkedTableEntries& foriegnKeyTables = ForiegnKeyLinkedTableEntries());
	QStringList GetChosenTables() const;

	const TableEntries& GetChosenLinkedTables(unsigned int tableIndex) const;

	virtual unsigned int GetMaxDepth() const;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& index) const;

private:
	const TableEntry& GetTableEntry(const QModelIndex& index) const;

	bool m_includeCheckboxes;
	TableEntries m_tables;
	ForiegnKeyLinkedTableEntries m_tableToTablesLinkedByForiegnKeys;

	static const quintptr s_rootID;
};

#endif // TABLECHOICEMODEL_H
