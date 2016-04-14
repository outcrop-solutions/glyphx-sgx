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

#ifndef FILTERINGMANAGER_H
#define FILTERINGMANAGER_H

#include <QtCore/QObject>
#include "datamappingloadingfiltermodel.h"
#include "sourcedatacache.h"
#include "inputtable.h"
#include "itemfocusselectionmodel.h"
#include "interval.h"
#include <unordered_map>
#include <map>
#include "filteringparameters.h"

class FilteringManager : public QObject
{
	Q_OBJECT

public:
	typedef std::map<QString, SynGlyphX::IndexSet> IndexSetMap;
	typedef QMap<QString, FilteringParameters> Table2FiltersMap;

	FilteringManager(DataMappingLoadingFilterModel* dataMappingModel, SourceDataCache::SharedPtr sourceDataCache, SynGlyphX::ItemFocusSelectionModel* sceneSelectionModel, QObject *parent);
	~FilteringManager();

	void GenerateFilterResultsForTable(const QString& table, const FilteringParameters& filters, bool updateFocus = false);
	void GenerateLoadingFilterResultsForTable(const QString& table, const FilteringParameters::ColumnDistinctValuesFilterMap& filters);
	void SetFilterIndexesForTable(const QString& table, const SynGlyphX::IndexSet& filterSet, bool updateFocus = false);
	
	void ClearAllFilters();
	void ClearFiltersForTable(const QString& table, bool updateFocus = false);
	const IndexSetMap& GetFilterResultsByTable() const;
	const SynGlyphX::IndexSet& GetGlyphIndexedFilterResults() const;
	const Table2FiltersMap& GetTable2FiltersMap() const;

	const SynGlyphX::ItemFocusSelectionModel* GetSceneSelectionModel() const;
	SourceDataCache::ConstSharedPtr GetSourceDataCache() const;
	const DataMappingLoadingFilterModel* GetDataMappingModel() const;

signals:
	void FilterResultsChanged(const SynGlyphX::IndexSet& glyphIndexedFilteredResults);

private slots:
	//void OnSceneSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void OnSceneModelReset();

private:
	typedef std::map<SynGlyphX::ProperInterval, QString> GlyphTemplateRangeToTableMap;
	typedef QMultiMap<QString, SynGlyphX::ProperInterval> TableToGlyphTemplateRangesMap;

	void Clear();
	void UpdateGlyphIndexedFilterResults();

	void AddSceneIndexesFromTableToSelection(QItemSelection& selection, const QString& table);
	void AddSceneIndexesToSelection(QItemSelection& selection, const QString& table, const SynGlyphX::IndexSet& indexSet);
	void ClearSourceDataSelectionForTable(QItemSelection& selection, bool updateFocus);

	DataMappingLoadingFilterModel* m_dataMappingModel;
	SourceDataCache::SharedPtr m_sourceDataCache;
	SynGlyphX::ItemFocusSelectionModel* m_sceneSelectionModel;

	//Results from loading screen filter
	IndexSetMap m_filterResultsPerTableFromLoadingFilter;

	TableToGlyphTemplateRangesMap m_tableToGlyphTreeRangesMap;
	GlyphTemplateRangeToTableMap m_glyphTemplateRangeToTableMap;

	IndexSetMap m_filterResultsByTable;
	SynGlyphX::IndexSet m_filterResultsIndexedToGlyphs;

	Table2FiltersMap m_filtersForEachTable;
};

#endif // SOURCEDATASELECTIONMODEL_H
