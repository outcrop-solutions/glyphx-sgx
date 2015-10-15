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

#ifndef SOURCEDATASELECTIONMODEL_H
#define SOURCEDATASELECTIONMODEL_H

#include <QtCore/QObject>
#include "datamappingmodel.h"
#include "sourcedatacache.h"
#include "inputtable.h"
#include "itemfocusselectionmodel.h"
#include "range.h"
#include <unordered_map>
#include <map>

class SourceDataSelectionModel : public QObject
{
	Q_OBJECT

public:
	SourceDataSelectionModel(SynGlyphX::DataMappingModel* dataMappingModel, SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, SynGlyphX::ItemFocusSelectionModel* sceneSelectionModel, QObject *parent);
	~SourceDataSelectionModel();

	void SetNewSourceDataSelection(const SynGlyphX::SourceDataCache::IndexSetMap& selectedIndexMap);
	void SetSourceDataSelectionForTable(const QString& table, const SynGlyphX::IndexSet& newSelectionSet);
	void ClearSourceDataSelection();
	void ClearSourceDataSelectionForTable(const QString& table);
	const SynGlyphX::SourceDataCache::IndexSetMap& GetSourceDataSelection() const;

signals:
	void SelectionChanged();

private slots:
	void OnSceneSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void OnSceneModelReset();

private:
	void AddSceneIndexesFromTableToSelection(QItemSelection& selection, const QString& table);

	SynGlyphX::DataMappingModel* m_dataMappingModel;
	SynGlyphX::SourceDataCache::SharedPtr m_sourceDataCache;
	SynGlyphX::ItemFocusSelectionModel* m_sceneSelectionModel;

	std::unordered_multimap<QString, SynGlyphX::Range> m_tableToGlyphTreeRangesMap;
	std::map<SynGlyphX::Range, QString> m_glyphTemplateRangeToTableMap;

	SynGlyphX::SourceDataCache::IndexSetMap m_selectedSourceDataSets;
};

#endif // SOURCEDATASELECTIONMODEL_H
