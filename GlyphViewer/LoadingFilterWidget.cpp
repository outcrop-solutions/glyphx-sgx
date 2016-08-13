#include "LoadingFilterWidget.h"
#include "TitleListWidget.h"
#include <QtWidgets/QLayout>
#include "glyphengine.h"
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include <boost/uuid/uuid_io.hpp>
#include "datasource.h"
#include "sourcedatacache.h"
#include "stringconvert.h"

LoadingFilterMissingChoice::LoadingFilterMissingChoice(const std::wstring& field) :
	std::runtime_error(""),
	m_field(field) {


}

SingleLoadingFilterWidget::SingleLoadingFilterWidget(bool isRequired, QWidget* parent) :
	SynGlyphX::TitleListWidget(parent),
	m_isRequired(isRequired) {


}


LoadingFilterWidget::LoadingFilterWidget(QWidget *parent)
	: QWidget(parent)
{
	
}

LoadingFilterWidget::~LoadingFilterWidget()
{

}

void LoadingFilterWidget::SetFilters(DataEngine::GlyphEngine& glyphEngine, const SynGlyphX::MultiTableFrontEndFilters& filters) {

	//QWidget* innerWidget = new QWidget(this);
	//setWidget(innerWidget);

	//QVBoxLayout* innerWidgetLayout = new QVBoxLayout(innerWidget);
	QVBoxLayout* innerWidgetLayout = new QVBoxLayout(this);

	for (const auto& filtersForTable : filters) {

		QSplitter* splitter = AddFiltersForTable(glyphEngine, filtersForTable.second, filtersForTable.first);
		innerWidgetLayout->addWidget(splitter, 1);
	}

	//int stretchSize = 3 - filters.size();
	//if (stretchSize > 0) {

	//	innerWidgetLayout->addStretch(stretchSize);
	//}

	//innerWidget->setLayout(innerWidgetLayout);
	setLayout(innerWidgetLayout);
}

QSplitter* LoadingFilterWidget::AddFiltersForTable(DataEngine::GlyphEngine& glyphEngine, const SynGlyphX::SingleTableFrontEndFilters& filters, const SynGlyphX::InputTable& table) {

	QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

	splitter->setChildrenCollapsible(false);
	splitter->setHandleWidth(2);
	splitter->setStyleSheet("QSplitter::handle:horizontal { background: qlineargradient(x1 : 0, y1 : 0, x2 : 1, y2 : 1, stop : 0 #eee, stop:1 #ccc);"
		"border: 1px solid #777; width: 0px; margin - top: 0px; margin - bottom: 0px; border - radius: 2px; }");

	QString id = QString::fromStdWString(boost::uuids::to_wstring(table.GetDatasourceID()));
	QString tableName = QString::fromStdWString(table.GetTable());

	FieldToWidgetMap fieldToWidgetMap;
	for (const auto& filter : filters) {

		QString qField = QString::fromStdWString(filter.first);

		SingleLoadingFilterWidget* filterWidget = new SingleLoadingFilterWidget(filter.second.IsRequired(), this);
		filterWidget->layout()->setContentsMargins(0, 0, 0, 0);
		filterWidget->SetAllowMultiselect(filter.second.IsMultiselectAllowed());
		filterWidget->ShowSelectAllButton(true);
		filterWidget->SetTitle(qField);
		filterWidget->SetItems(glyphEngine.DistinctValuesForField(id, tableName, qField));

		splitter->addWidget(filterWidget);
		fieldToWidgetMap[filter.first] = filterWidget;
	}

	m_filterListWidgets[table] = fieldToWidgetMap;

	return splitter;
}

bool LoadingFilterWidget::AreSelectionsValid() const {

	for (const auto& tableFilterWidgets : m_filterListWidgets) {

		for (const auto& filterWidget : tableFilterWidgets.second) {

			if (filterWidget.second->IsRequired() && !filterWidget.second->AreAnyItemsSelected()) {

				return false;
			}
		}
	}

	return true;
}

bool LoadingFilterWidget::IsQueryNeeded(const SynGlyphX::InputTable& table) const {

	for (const auto& filterWidget : m_filterListWidgets.find(table)->second) {

		if (filterWidget.second->AreAnyItemsSelected()) {

			return true;
		}
	}

	return false;
}

MultiTableDistinctValueFilteringParameters LoadingFilterWidget::GetFilterValues() const {

	MultiTableDistinctValueFilteringParameters filteringParameters;

	for (const auto& tableFilterWidgets : m_filterListWidgets) {

		DistinctValueFilteringParameters filteringParametersForTable;
		for (const auto& filterWidget : tableFilterWidgets.second) {

			QSet<QString> filterData = filterWidget.second->GetSelectedLabels().toSet();
			if ((!filterData.isEmpty()) && (!filterWidget.second->AreAllItemsSelected())) {

				filteringParametersForTable.SetDistinctValueFilter(QString::fromStdWString(filterWidget.first), filterData);
			}
		}

		if (filteringParametersForTable.HasFilters()) {

			filteringParameters[tableFilterWidgets.first] = filteringParametersForTable;
		}
	}

	return filteringParameters;
}
/*
QString LoadingFilterWidget::GenerateQuery(const SynGlyphX::InputTable& table) const {

	for (const auto& filterWidget : m_filterListWidgets.find(table)->second) {

		if (filterWidget.second->IsRequired() && !filterWidget.second->AreAnyItemsSelected()) {

			throw LoadingFilterMissingChoice(filterWidget.first);
		}
	}

	QString query = "SELECT * FROM \"" + QString::fromStdWString(boost::uuids::to_wstring(table.GetDatasourceID()));
	if (table.GetTable() != SynGlyphX::Datasource::SingleTableName) {

		query += ":" + QString::fromStdWString(table.GetTable());
	}

	query += "\" WHERE ";
	
	const FieldToWidgetMap& singleTableFilters = m_filterListWidgets.find(table)->second;
	for (const auto& filter : singleTableFilters) {

		if (!filter.second->AreAllItemsSelected()) {

			if (filter != *singleTableFilters.cbegin()) {

				query += " AND ";
			}

			query += SourceDataCache::CreateInString(QString::fromStdWString(filter.first), filter.second->GetSelectedLabels().toSet());
		}
	}
	
	return query;
}
*/