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

#ifndef DATAMAPPINGLOADINGFILTERMODEL_H
#define DATAMAPPINGLOADINGFILTERMODEL_H

#include "datamappingmodel.h"
#include "filteringparameters.h"
#include "inputtable.h"

class DataMappingLoadingFilterModel : public SynGlyphX::DataMappingModel
{
	Q_OBJECT

public:
	typedef std::unordered_map<SynGlyphX::InputTable, FilteringParameters> Table2LoadingFiltersMap;

	DataMappingLoadingFilterModel(QObject *parent);
	~DataMappingLoadingFilterModel();

	void LoadDataTransformFile(const QString& filename, const Table2LoadingFiltersMap& loadingFiltersMap);
	const Table2LoadingFiltersMap& GetLoadingFilters() const;

private:
	Table2LoadingFiltersMap m_loadingFiltersMap;
};

#endif // DATAMAPPINGLOADINGFILTERMODEL_H
