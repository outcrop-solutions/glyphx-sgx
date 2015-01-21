#ifndef GLYPHFORESTMODEL_H
#define GLYPHFORESTMODEL_H

#include <QtCore/QAbstractItemModel>
#include <unordered_map>
#include "antzdata.h"
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

#include "glyph.h"

class GlyphForestModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	GlyphForestModel(QObject *parent);
	~GlyphForestModel();

	//Functions from QAbstractItemModel that need to be implemented
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	virtual QModelIndex	index(int row, int column = 0, const QModelIndex& parent = QModelIndex()) const;
	virtual QModelIndex	parent(const QModelIndex& index) const;
	virtual int	rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	ANTzPlus::ANTzData::SharedPtr GetANTzData() const;

	const QStringList& GetBaseImageFilenames() const;

	void Clear();
	void LoadANTzVisualization(const QStringList& antzCSVFilenames, const QStringList& baseImageFilenames);

	QModelIndex IndexFromANTzID(int id) const;

	void SetParentGridToDefaultBaseImage();

private:
	void Clear(bool resetModel);

	int GetChildIndexFromParent(pNPnode node) const;
	int FindRowForRootNode(pNPnode) const;
	std::unordered_map<std::wstring, int> m_textures;
	QString m_defaultBaseImage;
	ANTzPlus::ANTzData::SharedPtr m_antzData;
	QStringList m_baseImageFilenames;
};

#endif // GLYPHFORESTMODEL_H
