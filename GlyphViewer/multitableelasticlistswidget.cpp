#include "multitableelasticlistswidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QAbstractItemView>
#include "groupboxsinglewidget.h"
#include "elasticlistwidget.h"

MultiTableElasticListsWidget::MultiTableElasticListsWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, SourceDataSelectionModel* selectionModel, QWidget *parent)
	: QWidget(parent),
	m_selectionModel(selectionModel),
	m_sourceDataCache(sourceDataCache)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	m_tableComboBox = new QComboBox(this);
	m_tableComboBox->setEnabled(false);
	m_tableComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_tableComboBox->setMinimumContentsLength(15);
	m_tableComboBox->blockSignals(true);

	SynGlyphX::GroupBoxSingleWidget* tableComboGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Source Data Table:"), m_tableComboBox, this);
	mainLayout->addWidget(tableComboGroupBox);
	
	m_elasticListsStackLayout = new QStackedLayout(this);
	ClearElasticLists();
	mainLayout->addLayout(m_elasticListsStackLayout, 1);
	QObject::connect(m_tableComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MultiTableElasticListsWidget::OnComboBoxChanged);

	setLayout(mainLayout);

	QObject::connect(m_selectionModel, &SourceDataSelectionModel::SelectionChanged, this, &MultiTableElasticListsWidget::OnSelectionChanged);
	QObject::connect(m_selectionModel->GetSceneSelectionModel()->model(), &QAbstractItemModel::modelReset, this, &MultiTableElasticListsWidget::OnModelReset);
}

MultiTableElasticListsWidget::~MultiTableElasticListsWidget()
{

}

void MultiTableElasticListsWidget::OnSelectionChanged() {

	UpdateElasticListsAndSourceDataWidget();
}

void MultiTableElasticListsWidget::OnModelReset() {

	ClearElasticLists();
	if (m_sourceDataCache->IsValid()) {

		const SynGlyphX::SourceDataCache::TableNameMap& tableNameMap = m_sourceDataCache->GetFormattedNames();
		for (auto formattedName : tableNameMap) {

			m_tableComboBox->addItem(formattedName.second, formattedName.first);
			SingleTableElasticListsWidget* elasticListsWidgetForTable = new SingleTableElasticListsWidget(m_sourceDataCache, formattedName.first, this);
			m_elasticListsStackLayout->addWidget(elasticListsWidgetForTable);
			m_elasticListWidgetsForEachTable[formattedName.first.toStdString()] = elasticListsWidgetForTable;
			QObject::connect(elasticListsWidgetForTable, &SingleTableElasticListsWidget::SelectionChanged, this, &MultiTableElasticListsWidget::OnElasticListsSelectionChanged);
		}
		
		m_tableComboBox->view()->setMinimumWidth(m_tableComboBox->view()->sizeHintForColumn(0));
		m_tableComboBox->setEnabled(true);
		m_tableComboBox->blockSignals(false);

		UpdateElasticListsAndSourceDataWidget();
	}
}

void MultiTableElasticListsWidget::OnComboBoxChanged(int current) {

	m_elasticListsStackLayout->setCurrentWidget(m_elasticListWidgetsForEachTable[m_tableComboBox->currentData().toString().toStdString()]);
}

void MultiTableElasticListsWidget::UpdateElasticListsAndSourceDataWidget() {

	const SourceDataSelectionModel::IndexSetMap& sourceDataSelectionSets = m_selectionModel->GetSourceDataSelection();
	bool isSelectionNotEmpty = !sourceDataSelectionSets.empty();
	if (isSelectionNotEmpty) {

		//m_sourceDataWindow->UpdateTables(sourceDataSelectionSets);
		UpdateElasticLists(sourceDataSelectionSets);

		//Only change the table shown if it is not in the selection at all
		if (sourceDataSelectionSets.count(m_tableComboBox->currentData().toString()) == 0) {

			int newComboBoxIndex = m_tableComboBox->findData(sourceDataSelectionSets.rbegin()->first);
			if ((newComboBoxIndex != -1) && (newComboBoxIndex != m_tableComboBox->currentIndex())) {

				m_tableComboBox->setCurrentIndex(newComboBoxIndex);
			}
		}
	}
	else {

		UpdateElasticLists();
	}
}

void MultiTableElasticListsWidget::UpdateElasticLists(const SourceDataSelectionModel::IndexSetMap& dataIndexes) {

	for (auto table : m_sourceDataCache->GetFormattedNames()) {

		int c = m_sourceDataCache->GetNumberOfRowsInTable(table.first);
		SingleTableElasticListsWidget* SingleTableElasticListsWidget = m_elasticListWidgetsForEachTable[table.first.toStdString()];
		SourceDataSelectionModel::IndexSetMap::const_iterator dataIndexesForTable = dataIndexes.find(table.first);
		if (dataIndexesForTable == dataIndexes.end()) {

			SingleTableElasticListsWidget->PopulateElasticLists();
		}
		else {

			SingleTableElasticListsWidget->PopulateElasticLists(dataIndexesForTable->second);
		}
	}
}

void MultiTableElasticListsWidget::ClearElasticLists() {

	for (auto widget : m_elasticListWidgetsForEachTable) {

		m_elasticListsStackLayout->removeWidget(widget.second);
		delete widget.second;
	}
	m_elasticListWidgetsForEachTable.clear();

	m_tableComboBox->blockSignals(true);
	m_tableComboBox->clear();
	m_tableComboBox->setEnabled(false);
}

void MultiTableElasticListsWidget::OnElasticListsSelectionChanged(const QString& table, const SynGlyphX::SourceDataCache::ColumnValueData& selection) {

	if (selection.empty()) {

		m_selectionModel->ClearSourceDataSelectionForTable(table);
	}
	else {

		const SourceDataSelectionModel::IndexSetMap& indexSets = m_selectionModel->GetSourceDataSelection();
		SourceDataSelectionModel::IndexSetMap::const_iterator previousSelection = indexSets.find(table);
		SynGlyphX::IndexSet indexSet;
		
		if (previousSelection != indexSets.end()) {

			indexSet = m_sourceDataCache->GetIndexesFromTableWithSelectedValues(table, selection, previousSelection->second);
		}
		else {

			indexSet = m_sourceDataCache->GetIndexesFromTableWithSelectedValues(table, selection);
		}
		
		m_selectionModel->SetSourceDataSelectionForTable(table, indexSet);
	}
}