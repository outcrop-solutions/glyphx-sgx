#include "multitableelasticlistswidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QAbstractItemView>
#include "groupboxsinglewidget.h"
#include "elasticlistwidget.h"

MultiTableElasticListsWidget::MultiTableElasticListsWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, SynGlyphXANTz::GlyphForestModel* model, SynGlyphX::ItemFocusSelectionModel* selectionModel, QWidget *parent)
	: QWidget(parent),
	m_model(model),
	m_selectionModel(selectionModel),
	m_sourceDataCache(sourceDataCache)
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	QHBoxLayout* topLayout = new QHBoxLayout;

	m_hideUnselectedTreesCheckbox = new QCheckBox(tr("Filter View"), this);
	topLayout->addWidget(m_hideUnselectedTreesCheckbox);

	m_clearButton = new QPushButton(tr("Clear"), this);
	topLayout->addWidget(m_clearButton);
	QObject::connect(m_clearButton, &QPushButton::clicked, this, &MultiTableElasticListsWidget::Clear);

	topLayout->addStretch(1);
	
	layout->addLayout(topLayout);

	m_tableComboBox = new QComboBox(this);
	m_tableComboBox->setEnabled(false);
	m_tableComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_tableComboBox->setMinimumContentsLength(15);
	m_tableComboBox->blockSignals(true);

	SynGlyphX::GroupBoxSingleWidget* tableComboGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Source Data Table:"), m_tableComboBox, this);
	layout->addWidget(tableComboGroupBox);
	
	m_elasticListsStackLayout = new QStackedLayout(this);
	ClearElasticLists();
	layout->addLayout(m_elasticListsStackLayout, 1);
	QObject::connect(m_tableComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &MultiTableElasticListsWidget::OnComboBoxChanged);

	QHBoxLayout* buttonsLayout = new QHBoxLayout(this);

	m_sourceWidgetButton = new QPushButton(tr("Show Selected Source Data"), this);
	m_sourceWidgetButton->setCheckable(true);
	buttonsLayout->addWidget(m_sourceWidgetButton);

	buttonsLayout->addStretch(1);
	layout->addLayout(buttonsLayout);

	setLayout(layout);

	EnableButtons(!m_selectionModel->selection().empty());
	QObject::connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &MultiTableElasticListsWidget::OnSelectionChanged);

	m_sourceDataWindow.reset(new SourceDataWidget(m_sourceDataCache));
	QObject::connect(m_sourceWidgetButton, &QPushButton::toggled, m_sourceDataWindow.data(), &SourceDataWidget::setVisible);
	m_sourceDataWindow->setVisible(false);

	QObject::connect(m_sourceDataWindow.data(), &SourceDataWidget::WindowHidden, this, &MultiTableElasticListsWidget::OnSourceWidgetWindowHidden);
	QObject::connect(model, &SynGlyphXANTz::GlyphForestModel::modelReset, this, &MultiTableElasticListsWidget::OnModelReset);
}

MultiTableElasticListsWidget::~MultiTableElasticListsWidget()
{

}

void MultiTableElasticListsWidget::SetupLinkedWidgets(LinkedWidgetsManager& linkedWidgets) {

	linkedWidgets.AddFilterViewCheckbox(m_hideUnselectedTreesCheckbox);
}

void MultiTableElasticListsWidget::OnSourceWidgetWindowHidden() {

	m_sourceWidgetButton->setChecked(false);
}

void MultiTableElasticListsWidget::OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	UpdateElasticListsAndSourceDataWidget(m_selectionModel->selection().indexes());
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

		UpdateElasticListsAndSourceDataWidget(m_selectionModel->selection().indexes());
	}
	else {

		EnableButtons(false);
	}
}

void MultiTableElasticListsWidget::OnComboBoxChanged(int current) {

	m_elasticListsStackLayout->setCurrentWidget(m_elasticListWidgetsForEachTable[m_tableComboBox->currentData().toString().toStdString()]);
}

void MultiTableElasticListsWidget::UpdateElasticListsAndSourceDataWidget(const QModelIndexList& selectedIndexes) {

	bool isSelectionNotEmpty = !selectedIndexes.empty();
	EnableButtons(isSelectionNotEmpty);
	if (isSelectionNotEmpty) {

		SynGlyphX::SourceDataCache::IndexSetMap indexSets = m_sourceDataCache->SplitIndexSet(SynGlyphX::ItemFocusSelectionModel::GetRootRows(m_selectionModel->selection().indexes()));

		m_sourceDataWindow->UpdateTables(indexSets);
		UpdateElasticLists(indexSets);

		//Only change the table shown if it is not in the selection at all
		if (indexSets.count(m_tableComboBox->currentData().toString()) == 0) {

			int newComboBoxIndex = m_tableComboBox->findData(m_sourceDataCache->GetTableNameForIndex(SynGlyphX::ItemFocusSelectionModel::GetRootRow(m_selectionModel->selection().indexes().back())));
			if ((newComboBoxIndex != -1) && (newComboBoxIndex != m_tableComboBox->currentIndex())) {

				m_tableComboBox->setCurrentIndex(newComboBoxIndex);
			}
		}
	}
	else {

		UpdateElasticLists();
		m_sourceDataWindow->setVisible(false);
		OnSourceWidgetWindowHidden();
	}
}

void MultiTableElasticListsWidget::UpdateElasticLists(const SynGlyphX::SourceDataCache::IndexSetMap& dataIndexes) {

	for (auto table : m_sourceDataCache->GetFormattedNames()) {

		SingleTableElasticListsWidget* SingleTableElasticListsWidget = m_elasticListWidgetsForEachTable[table.first.toStdString()];
		SynGlyphX::SourceDataCache::IndexSetMap::const_iterator dataIndexesForTable = dataIndexes.find(table.first);
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

		m_selectionModel->clearSelection();
	}
	else {

		SynGlyphX::SourceDataCache::IndexSetMap indexSets = m_sourceDataCache->SplitIndexSet(SynGlyphX::ItemFocusSelectionModel::GetRootRows(m_selectionModel->selection().indexes()));
		SynGlyphX::SourceDataCache::IndexSetMap::const_iterator previousSelection = indexSets.find(table);
		SynGlyphX::IndexSet indexSet;
		
		if (previousSelection != indexSets.end()) {

			indexSet = m_sourceDataCache->GetIndexesFromTableWithSelectedValues(table, selection, previousSelection->second);
		}
		else {

			indexSet = m_sourceDataCache->GetIndexesFromTableWithSelectedValues(table, selection);
		}
		
		QItemSelection itemSelection;
		for (auto row : indexSet) {

			QModelIndex index = m_model->index(row);
			itemSelection.select(index, index);
		}

		m_selectionModel->select(itemSelection, QItemSelectionModel::ClearAndSelect);
		m_selectionModel->SetFocus(itemSelection.indexes(), SynGlyphX::ItemFocusSelectionModel::FocusFlag::ClearAndFocus);
	}
}

void MultiTableElasticListsWidget::EnableButtons(bool enable) {

	m_sourceWidgetButton->setEnabled(enable);
	m_clearButton->setEnabled(enable);
}

void MultiTableElasticListsWidget::Clear() {

	m_selectionModel->clearSelection();
	m_selectionModel->ClearFocus();
}