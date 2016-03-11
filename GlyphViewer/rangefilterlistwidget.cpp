#include "rangefilterlistwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>

RangeFilterListWidget::RangeFilterListWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, SourceDataSelectionModel* selectionModel, QWidget *parent)
	: QWidget(parent),
	m_selectionModel(selectionModel),
	m_sourceDataCache(sourceDataCache),
	m_currentTable("")
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QHBoxLayout* buttonLayout = new QHBoxLayout(this);
	m_addButton = new QPushButton(tr("Add"), this);
	QObject::connect(m_addButton, &QPushButton::clicked, this, &RangeFilterListWidget::OnAddFilter);
	buttonLayout->addWidget(m_addButton);
	m_addButton->setEnabled(false);
	
	m_removeAllButton = new QPushButton(tr("Remove All"), this);
	QObject::connect(m_removeAllButton, &QPushButton::clicked, this, &RangeFilterListWidget::OnRemoveAllFilters);
	buttonLayout->addWidget(m_removeAllButton);
	m_removeAllButton->setEnabled(false);

	mainLayout->addLayout(buttonLayout);

	m_filtersLayout = new QStackedLayout(this);
	mainLayout->addLayout(m_filtersLayout, 1);

	setLayout(mainLayout);

	QObject::connect(m_selectionModel->GetSceneSelectionModel()->model(), &QAbstractItemModel::modelReset, this, &RangeFilterListWidget::OnModelReset);
}

RangeFilterListWidget::~RangeFilterListWidget()
{

}

void RangeFilterListWidget::SwitchTable(const QString& table) {

	m_currentTable = table;
	m_filtersLayout->setCurrentWidget(m_table2WidgetMap[table]);
}

void RangeFilterListWidget::OnModelReset() {

	m_addButton->setEnabled(m_sourceDataCache->IsValid());
	m_removeAllButton->setEnabled(false);

	for (auto tableWidget : m_table2WidgetMap) {

		m_filtersLayout->removeWidget(tableWidget);
		delete tableWidget;
	}
	m_table2WidgetMap.clear();
	m_currentTable.clear();
	m_numericFilters.clear();

	if (m_sourceDataCache->IsValid()) {

		const SynGlyphX::SourceDataCache::TableNameMap& tableNameMap = m_sourceDataCache->GetFormattedNames();
		for (auto formattedName : tableNameMap) {

			SynGlyphX::VerticalScrollArea* newScrollArea = new SynGlyphX::VerticalScrollArea(this);
			QWidget* newInnerWidget = new QWidget(this);
			QVBoxLayout* newLayout = new QVBoxLayout(newInnerWidget);
			newLayout->addStretch(1);
			newInnerWidget->setLayout(newLayout);
			newScrollArea->setWidget(newInnerWidget);
			m_filtersLayout->addWidget(newScrollArea);
			m_table2WidgetMap.insert(formattedName.first, newScrollArea);
			m_numericFilters.insert(formattedName.first, QVector<SynGlyphX::SingleNumericRangeFilterWidget*>());
		}
		m_currentTable = tableNameMap.begin()->first;
	}
}

void RangeFilterListWidget::OnAddFilter() {

	SynGlyphX::SingleNumericRangeFilterWidget* filter = new SynGlyphX::SingleNumericRangeFilterWidget(Qt::Horizontal, this);
	QVBoxLayout* mainLayout = dynamic_cast<QVBoxLayout*>(m_table2WidgetMap[m_currentTable]->widget()->layout());
	mainLayout->insertWidget(m_numericFilters[m_currentTable].size(), filter);
	m_numericFilters[m_currentTable].push_back(filter);
	m_removeAllButton->setEnabled(true);
}

void RangeFilterListWidget::OnRemoveAllFilters() {

	for (SynGlyphX::SingleNumericRangeFilterWidget* filter : m_numericFilters[m_currentTable]) {

		m_table2WidgetMap[m_currentTable]->widget()->layout()->removeWidget(filter);
		delete filter;
	}
	m_numericFilters.erase(m_numericFilters.find(m_currentTable));
	m_removeAllButton->setEnabled(false);
}
