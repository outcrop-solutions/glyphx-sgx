#include "datamappingloadingfiltermodel.h"

DataMappingLoadingFilterModel::DataMappingLoadingFilterModel(QObject *parent)
	: SynGlyphX::DataMappingModel(parent)
{

}

DataMappingLoadingFilterModel::~DataMappingLoadingFilterModel()
{

}

void DataMappingLoadingFilterModel::LoadDataTransformFile(const QString& filename, const Table2LoadingFiltersMap& loadingFiltersMap) {

	SynGlyphX::DataMappingModel::LoadDataTransformFile(filename);

	m_loadingFiltersMap = loadingFiltersMap;
}

const DataMappingLoadingFilterModel::Table2LoadingFiltersMap& DataMappingLoadingFilterModel::GetLoadingFilters() const {

	return m_loadingFiltersMap;
}