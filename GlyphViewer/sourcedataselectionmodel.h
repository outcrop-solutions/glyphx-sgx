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
#include "interval.h"
#include <unordered_map>
#include <map>
#include "guihash.h"

class SourceDataSelectionModel : public QObject
{
	Q_OBJECT

public:
	typedef std::map<QString, SynGlyphX::IndexSet> IndexSetMap;

	SourceDataSelectionModel(SynGlyphX::DataMappingModel* dataMappingModel, SourceDataCache::SharedPtr sourceDataCache, SynGlyphX::ItemFocusSelectionModel* sceneSelectionModel, QObject *parent);
	~SourceDataSelectionModel();

	const SynGlyphX::ItemFocusSelectionModel* GetSceneSelectionModel() const;

	//void SetNewSourceDataSelection(const IndexSetMap& selectedIndexMap);
	void SetSourceDataSelectionForTable(const QString& table, const SynGlyphX::IndexSet& newSelectionSet, bool updateFocus = true);
	void ClearSourceDataSelection();
	void ClearSourceDataSelectionForTable(const QString& table, bool updateFocus = true);
	const IndexSetMap& GetSourceDataSelection() const;

	SourceDataCache::ConstSharedPtr GetSourceDataCache() const;
	const SynGlyphX::DataMappingModel* GetDataMappingModel() const;

signals:
	void SelectionChanged();

private slots:
	void OnSceneSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void OnSceneModelReset();

private:
	typedef std::map<SynGlyphX::ProperInterval, QString> GlyphTemplateRangeToTableMap;
	typedef std::unordered_multimap<QString, SynGlyphX::ProperInterval, SynGlyphX::QStringHash> TableToGlyphTemplateRangesMap;

	void AddSceneIndexesFromTableToSelection(QItemSelection& selection, const QString& table);
	void AddSceneIndexesToSelection(QItemSelection& selection, const QString& table, const SynGlyphX::IndexSet& indexSet);

	void ClearSourceDataSelectionForTable(QItemSelection& selection, bool updateFocus);

	SynGlyphX::DataMappingModel* m_dataMappingModel;
	SourceDataCache::SharedPtr m_sourceDataCache;
	SynGlyphX::ItemFocusSelectionModel* m_sceneSelectionModel;

	TableToGlyphTemplateRangesMap m_tableToGlyphTreeRangesMap;
	GlyphTemplateRangeToTableMap m_glyphTemplateRangeToTableMap;

	IndexSetMap m_selectedSourceDataSets;
};

#endif // SOURCEDATASELECTIONMODEL_H
