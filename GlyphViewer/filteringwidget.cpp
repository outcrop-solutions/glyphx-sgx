#include "filteringwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTabWidget>
#include "groupboxsinglewidget.h"
#include "application.h"

FilteringWidget::FilteringWidget(SourceDataInfoModel* columnsModel, FilteringManager* filteringManager, QWidget *parent)
	: QWidget(parent),
	m_filteringManager(filteringManager)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QHBoxLayout* topLayout = new QHBoxLayout();

	m_hideUnselectedTreesCheckbox = new QCheckBox(tr("Filter View"), this);
	topLayout->addWidget(m_hideUnselectedTreesCheckbox);

	m_clearButton = new QPushButton(tr("Clear All Filters"), this);
	topLayout->addWidget(m_clearButton);
	QObject::connect(m_clearButton, &QPushButton::clicked, this, &FilteringWidget::Clear);

	topLayout->addStretch(1);
	mainLayout->addLayout(topLayout);

	m_tableComboBox = new QComboBox(this);
	m_tableComboBox->setEnabled(false);
	m_tableComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_tableComboBox->setMinimumContentsLength(15);
	m_tableComboBox->blockSignals(true);

	SynGlyphX::GroupBoxSingleWidget* tableComboGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Source Data Table:"), m_tableComboBox, this);
	mainLayout->addWidget(tableComboGroupBox);

	QTabWidget* filterMethodsWidget = new QTabWidget(this);

	m_rangeListFilterWidget = new RangeFilterListWidget(columnsModel, m_filteringManager, filterMethodsWidget);
	filterMethodsWidget->addTab(m_rangeListFilterWidget, tr("Range"));
	
	m_keywordFilterListWidget = new KeywordFilterListWidget(columnsModel, m_filteringManager, filterMethodsWidget);
	filterMethodsWidget->addTab(m_keywordFilterListWidget, tr("Keyword"));

	m_elasticListsWidget = new MultiTableElasticListsWidget(m_filteringManager, filterMethodsWidget);
	filterMethodsWidget->addTab(m_elasticListsWidget, tr("Elastic"));

	mainLayout->addWidget(filterMethodsWidget, 1);

	QHBoxLayout* buttonsLayout = new QHBoxLayout();

	m_sourceWidgetButton = new QPushButton(tr("Show Filtered Data"), this);
	m_sourceWidgetButton->setEnabled(false);
	m_sourceWidgetButton->setCheckable(true);
	buttonsLayout->addWidget(m_sourceWidgetButton);

	buttonsLayout->addStretch(1);

	m_createSubsetVizButton = new QPushButton(tr("Create Subset Visualization"));
	m_createSubsetVizButton->setEnabled(false);
	buttonsLayout->addWidget(m_createSubsetVizButton);

	mainLayout->addLayout(buttonsLayout);

	setLayout(mainLayout);

	EnableButtons(!m_filteringManager->GetFilterResultsByTable().empty());
	m_sourceDataWindow.reset(new SourceDataWidget(m_filteringManager));
	m_sourceDataWindow->setWindowTitle(tr("Source Data Of Filtered Glyphs"));
	QObject::connect(m_sourceWidgetButton, &QPushButton::toggled, m_sourceDataWindow.data(), &SourceDataWidget::setVisible);
	m_sourceDataWindow->setVisible(false);

	QObject::connect(m_sourceDataWindow.data(), &SourceDataWidget::WindowHidden, this, &FilteringWidget::OnSourceWidgetWindowHidden);
	QObject::connect(m_filteringManager, &FilteringManager::FilterResultsChanged, this, &FilteringWidget::OnFilterResultsChanged);
	
	QObject::connect(m_tableComboBox, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), this, &FilteringWidget::OnTableChanged);

	QObject::connect(m_createSubsetVizButton, &QPushButton::clicked, m_sourceDataWindow.data(), &SourceDataWidget::CreateSubsetVisualization);
}

FilteringWidget::~FilteringWidget()
{

}

void FilteringWidget::SetupLinkedWidgets(LinkedWidgetsManager& linkedWidgets) {

	linkedWidgets.AddFilterViewCheckbox(m_hideUnselectedTreesCheckbox);
}

void FilteringWidget::Clear() {

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
	m_filteringManager->ClearAllFilters();
	m_rangeListFilterWidget->OnRemoveAllFilters();
	m_keywordFilterListWidget->OnRemoveAllFilters();
	SynGlyphX::Application::restoreOverrideCursor();
}

void FilteringWidget::OnSourceWidgetWindowHidden() {

	m_sourceWidgetButton->setChecked(false);
}

void FilteringWidget::OnFilterResultsChanged() {

	const FilteringManager::Table2FiltersMap& filtersMap = m_filteringManager->GetTable2FiltersMap();
	EnableButtons(m_filteringManager->GetSourceDataCache()->IsValid() && (!filtersMap.empty()));

	m_createSubsetVizButton->setEnabled(!filtersMap.empty());

	//Only change the table shown if it is not in the selection at all
	if (!filtersMap.empty() && (filtersMap.count(m_tableComboBox->currentData().toString()) == 0)) {

		int newComboBoxIndex = m_tableComboBox->findData(filtersMap.lastKey());
		if ((newComboBoxIndex != -1) && (newComboBoxIndex != m_tableComboBox->currentIndex())) {

			m_tableComboBox->setCurrentIndex(newComboBoxIndex);
		}
	}
}

void FilteringWidget::OnNewVisualization() {

	SourceDataCache::ConstSharedPtr sourceDataCache = m_filteringManager->GetSourceDataCache();

	m_tableComboBox->blockSignals(true);
	m_tableComboBox->clear();
	m_tableComboBox->setEnabled(sourceDataCache->IsValid());

	m_sourceWidgetButton->setEnabled(sourceDataCache->IsValid());

	if (sourceDataCache->IsValid()) {

		const SourceDataCache::TableNameMap& tableNameMap = sourceDataCache->GetFormattedNames();
		for (auto formattedName : tableNameMap) {

			m_tableComboBox->addItem(formattedName.second, formattedName.first);
		}

		m_tableComboBox->view()->setMinimumWidth(m_tableComboBox->view()->sizeHintForColumn(0));
		m_tableComboBox->blockSignals(false);

		m_sourceDataWindow->UpdateTables();
	}
	else {

		OnSourceWidgetWindowHidden();
	}

	m_elasticListsWidget->OnNewVisualization();
	m_rangeListFilterWidget->OnNewVisualization();
	m_keywordFilterListWidget->OnNewVisualization();

	OnFilterResultsChanged();
}

void FilteringWidget::OnTableChanged(const QString& table) {

	QString tableId = m_tableComboBox->currentData().toString();
	m_rangeListFilterWidget->SwitchTable(tableId);
	m_elasticListsWidget->SwitchTable(tableId);
	m_keywordFilterListWidget->SwitchTable(tableId);
}

void FilteringWidget::EnableButtons(bool enable) {

	m_clearButton->setEnabled(enable);
}