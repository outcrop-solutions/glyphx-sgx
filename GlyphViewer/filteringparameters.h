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

#ifndef FILTERINGPARAMETERS_H
#define FILTERINGPARAMETERS_H

#include "DistinctValueFilteringParameters.h"
#include <vector>
#include "interval.h"
#include "keywordfilter.h"

class FilteringParameters : public DistinctValueFilteringParameters
{
public:
	typedef std::pair<QString, SynGlyphX::DegenerateInterval> ColumnRangeFilter;
	typedef std::vector<ColumnRangeFilter> ColumnRangeFilterMap;

	typedef std::pair<QString, KeywordFilter> ColumnKeywordFilter;
	typedef std::vector<ColumnKeywordFilter> ColumnKeywordFilterMap;

	FilteringParameters();
	FilteringParameters(const FilteringParameters& filters);
	virtual ~FilteringParameters();

	FilteringParameters& operator=(const FilteringParameters& filters);
	bool operator==(const FilteringParameters& filters);
	bool operator!=(const FilteringParameters& filters);

	void Clear() override;
	bool HasFilters() const override;

	void SetRangeFilters(const ColumnRangeFilterMap& filterMap);
	void SetRangeFilter(const QString& column, const SynGlyphX::DegenerateInterval& rangeFilter);
	void RemoveRangeFilter(const QString& column);
	const ColumnRangeFilterMap& GetRangeFilters() const;

	void SetKeywordFilters(const ColumnKeywordFilterMap& filterMap);
	void SetKeywordFilter(const QString& column, const KeywordFilter& keywordFilter);
	void RemoveKeywordFilter(const QString& column);
	const ColumnKeywordFilterMap& GetKeywordFilters() const;

private:
	//Filters from Keyword tab in Filtering widget
	ColumnKeywordFilterMap m_keywordFilters;

	//Filters from Range tab in Filtering widget
	ColumnRangeFilterMap m_rangeFilters;
};

#endif //FILTERINGPARAMETERS_H