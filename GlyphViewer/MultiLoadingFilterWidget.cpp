#include "MultiLoadingFilterWidget.h"
#include "TitleListWidget.h"
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QLayout>
#include "LoadingFilterWidget.h"

MultiLoadingFilterWidget::MultiLoadingFilterWidget(QWidget *parent)
	: QSplitter(Qt::Horizontal, parent),
	m_loadingFilterWidgetsStack(nullptr)
{
	setChildrenCollapsible(false);
	setHandleWidth(2);
	setStyleSheet("QSplitter::handle:horizontal { background: qlineargradient(x1 : 0, y1 : 0, x2 : 1, y2 : 1, stop : 0 #eee, stop:1 #ccc);"
		"border: 1px solid #777; width: 0px; margin - top: 0px; margin - bottom: 0px; border - radius: 2px; }");

	m_viewListWidget = new SynGlyphX::TitleListWidget(this);
	m_viewListWidget->SetAllowMultiselect(false);
	m_viewListWidget->ShowSelectAllButton(false);
	m_viewListWidget->SetTitle(tr("View(s)"));
	m_viewListWidget->layout()->setContentsMargins(0, 0, 0, 0);

	addWidget(m_viewListWidget);

	QObject::connect(m_viewListWidget, &SynGlyphX::TitleListWidget::CurrentRowChanged, this, &MultiLoadingFilterWidget::OnFileSelected);
}

MultiLoadingFilterWidget::~MultiLoadingFilterWidget()
{

}

void MultiLoadingFilterWidget::Reset(const std::vector<VisualizationData>& dataAndFilters) {

	m_viewListWidget->blockSignals(true);

	m_loadingFilterWidgetMap.clear();
	m_mustHaveFilterMap.clear();
	m_fieldNameMap.clear();
	if (m_loadingFilterWidgetsStack != nullptr) {

		delete m_loadingFilterWidgetsStack;
	}
	m_loadingFilterWidgetsStack = new QStackedWidget(this);
	addWidget(m_loadingFilterWidgetsStack);

	QStringList titles;
	QStringList files;
	for (unsigned int i = 0; i < dataAndFilters.size(); ++i) {

		titles.push_back(dataAndFilters[i].m_title);
		files.push_back(dataAndFilters[i].m_sdtPath);

		if (dataAndFilters[i].m_tableInGlyphEd.isEmpty()) {

			QWidget* widget = new QWidget(this);
			m_loadingFilterWidgetsStack->addWidget(widget);
		}
		else {

			LoadingFilterWidget* loadingFilterWidget = new LoadingFilterWidget(this);

			QStringList fieldNames;
			for (unsigned int j = 0; j < dataAndFilters[i].m_filterTitles.size(); ++j) {

				loadingFilterWidget->AddFilter(dataAndFilters[i].m_filterTitles[j],
					dataAndFilters[i].m_filterMultiselect[j],
					dataAndFilters[i].m_filterValues[j]);

				fieldNames.push_back(dataAndFilters[i].m_filterFieldNames[j]);
			}

			m_loadingFilterWidgetsStack->addWidget(loadingFilterWidget);
			m_loadingFilterWidgetMap[i] = loadingFilterWidget;

			m_mustHaveFilterMap[i] = dataAndFilters[i].m_mustHaveFilter;
			m_fieldNameMap[i] = fieldNames;
		}
	}

	m_viewListWidget->SetItems(titles, files);

	m_viewListWidget->blockSignals(false);

	if (dataAndFilters.empty()) {

		m_loadingFilterWidgetsStack->setVisible(false);
	}
	else {

		m_viewListWidget->SelectItem(0);
		OnFileSelected(0);
	}
}

bool MultiLoadingFilterWidget::DoCurrentNecessaryFiltersHaveSelection() const {

	if (CanCurrentHaveFilters()) {

		unsigned int current = m_loadingFilterWidgetsStack->currentIndex();
		for (unsigned int i = 0; i < m_mustHaveFilterMap.at(current).size(); ++i) {

			if (m_mustHaveFilterMap.at(current).at(i) && (!m_loadingFilterWidgetMap.at(current)->AreAnyValuesSelected(i))) {

				return false;
			}
		}
	}

	return true;
}

QString MultiLoadingFilterWidget::GetCurrentFilename() const {

	return m_viewListWidget->GetSelectedTooltips().first();
}

bool MultiLoadingFilterWidget::CanCurrentHaveFilters() const {

	return (m_loadingFilterWidgetMap.count(m_loadingFilterWidgetsStack->currentIndex()) != 0);
}

FilteringParameters MultiLoadingFilterWidget::GetCurrentFilterValues() const {

	unsigned int currentWidget = m_loadingFilterWidgetsStack->currentIndex();

	FilteringParameters filteringParameters;

	if (m_fieldNameMap.count(currentWidget) != 0) {

		for (unsigned int i = 0; i < m_fieldNameMap.at(currentWidget).size(); ++i) {

			QSet<QString> filterData = m_loadingFilterWidgetMap.at(currentWidget)->GetFilterData(i);

			if ((!filterData.isEmpty()) && (!m_loadingFilterWidgetMap.at(currentWidget)->AreAllValuesSelected(i))) {

				filteringParameters.SetDistinctValueFilter(m_fieldNameMap.at(currentWidget).at(i), filterData);
			}
		}
	}

	return filteringParameters;
}

void MultiLoadingFilterWidget::OnFileSelected(int row) {

	m_loadingFilterWidgetsStack->setCurrentIndex(row);
	m_loadingFilterWidgetsStack->setVisible(m_loadingFilterWidgetMap.count(row) != 0);
}