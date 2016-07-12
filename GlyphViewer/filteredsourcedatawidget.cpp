#include "filteredsourcedatawidget.h"
#include "datatransformmodel.h"

FilteredSourceDataWidget::FilteredSourceDataWidget(FilteringManager* filteringManager, QWidget *parent)
	: SourceDataWidget(filteringManager->GetSourceDataCache(), filteringManager->GetDataTransformModel()->GetDataMapping(), parent),
	m_filteringManager(filteringManager)
{
	setWindowTitle(tr("Source Data Of Filtered Glyphs"));
	QObject::connect(m_filteringManager, &FilteringManager::FilterResultsChanged, this, &SourceDataWidget::UpdateTables);
}

FilteredSourceDataWidget::~FilteredSourceDataWidget()
{

}

SynGlyphX::IndexSet FilteredSourceDataWidget::GetSourceIndexesForTable(const QString& table) {

	const FilteringManager::IndexSetMap& dataIndexes = m_filteringManager->GetFilterResultsByTable();
	if (dataIndexes.count(table) == 0) {

		return SynGlyphX::IndexSet();
	}
	else {

		return dataIndexes.at(table);
	}
}