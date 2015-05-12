#include "sourcedataselectionwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QAbstractItemView>
#include "groupboxsinglewidget.h"
#include "elasticlistwidget.h"
#include "sourcedataselectionmodel.h"

SourceDataSelectionWidget::SourceDataSelectionWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, GlyphForestModel* model, QItemSelectionModel* selectionModel, QWidget *parent)
	: QWidget(parent),
	m_model(model),
	m_selectionModel(selectionModel),
	m_sourceDataCache(sourceDataCache)
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	QHBoxLayout* topLayout = new QHBoxLayout;

	m_hideUnselectedTreesCheckbox = new QCheckBox(tr("Filter View"), this);
	QObject::connect(m_hideUnselectedTreesCheckbox, &QCheckBox::clicked, this, &SourceDataSelectionWidget::OptionsChanged);
	topLayout->addWidget(m_hideUnselectedTreesCheckbox);

	m_clearButton = new QPushButton(tr("Clear"), this);
	topLayout->addWidget(m_clearButton);
	QObject::connect(m_clearButton, &QPushButton::clicked, m_selectionModel, &QItemSelectionModel::clear);

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
	QObject::connect(m_tableComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &SourceDataSelectionWidget::OnComboBoxChanged);

	QHBoxLayout* buttonsLayout = new QHBoxLayout(this);

	m_sourceWidgetButton = new QPushButton(tr("Show Selected Source Data"), this);
	m_sourceWidgetButton->setCheckable(true);
	buttonsLayout->addWidget(m_sourceWidgetButton);

	buttonsLayout->addStretch(1);
	layout->addLayout(buttonsLayout);

	setLayout(layout);

	EnableButtons(!m_selectionModel->selection().empty());
	QObject::connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &SourceDataSelectionWidget::OnSelectionChanged);

	m_sourceDataWindow.reset(new SourceDataWidget(m_sourceDataCache));
	QObject::connect(m_sourceWidgetButton, &QPushButton::toggled, m_sourceDataWindow.data(), &SourceDataWidget::setVisible);
	m_sourceDataWindow->setVisible(false);

	QObject::connect(m_sourceDataWindow.data(), &SourceDataWidget::WindowHidden, this, &SourceDataSelectionWidget::OnSourceWidgetWindowHidden);
	QObject::connect(model, &GlyphForestModel::modelReset, this, &SourceDataSelectionWidget::OnModelReset);
}

SourceDataSelectionWidget::~SourceDataSelectionWidget()
{

}

void SourceDataSelectionWidget::SetHideUnselectedTreesCheckbox(bool checked) {

	bool areSignalsBlocked = m_hideUnselectedTreesCheckbox->signalsBlocked();
	m_hideUnselectedTreesCheckbox->blockSignals(true);
	m_hideUnselectedTreesCheckbox->setChecked(checked);
	m_hideUnselectedTreesCheckbox->blockSignals(areSignalsBlocked);
}

bool SourceDataSelectionWidget::GetHideUnselectedTreesCheckbox() const {

	return m_hideUnselectedTreesCheckbox->isChecked();
}

void SourceDataSelectionWidget::OnSourceWidgetWindowHidden() {

	m_sourceWidgetButton->setChecked(false);
}

void SourceDataSelectionWidget::OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	UpdateElasticListsAndSourceDataWidget(m_selectionModel->selection().indexes());
}

void SourceDataSelectionWidget::OnModelReset() {

	ClearElasticLists();
	if (m_sourceDataCache->IsValid()) {

		const SynGlyphX::SourceDataCache::TableNameMap& tableNameMap = m_sourceDataCache->GetFormattedNames();
		for (auto formattedName : tableNameMap) {

			m_tableComboBox->addItem(formattedName.second, formattedName.first);
			ElasticListsWidget* elasticListsWidget = new ElasticListsWidget(m_sourceDataCache, formattedName.first, this);
			m_elasticListsStackLayout->addWidget(elasticListsWidget);
			m_elasticListWidgetsForEachTable[formattedName.first.toStdString()] = elasticListsWidget;
			QObject::connect(elasticListsWidget, &ElasticListsWidget::SelectionChanged, this, &SourceDataSelectionWidget::OnElasticListsSelectionChanged);
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

void SourceDataSelectionWidget::OnComboBoxChanged(int current) {

	m_elasticListsStackLayout->setCurrentWidget(m_elasticListWidgetsForEachTable[m_tableComboBox->currentData().toString().toStdString()]);
}

void SourceDataSelectionWidget::UpdateElasticListsAndSourceDataWidget(const QModelIndexList& selectedIndexes) {

	bool isSelectionNotEmpty = !selectedIndexes.empty();
	EnableButtons(isSelectionNotEmpty);
	if (isSelectionNotEmpty) {

		SynGlyphX::SourceDataCache::IndexSetMap indexSets = m_sourceDataCache->SplitIndexSet(SourceDataSelectionModel::GetRootRows(m_selectionModel->selection().indexes()));

		m_sourceDataWindow->UpdateTables(indexSets);
		UpdateElasticLists(indexSets);
	}
	else {

		UpdateElasticLists();
		m_sourceDataWindow->setVisible(false);
		OnSourceWidgetWindowHidden();
	}
}

void SourceDataSelectionWidget::UpdateElasticLists(const SynGlyphX::SourceDataCache::IndexSetMap& dataIndexes) {

	for (auto table : m_sourceDataCache->GetFormattedNames()) {

		ElasticListsWidget* elasticListsWidget = m_elasticListWidgetsForEachTable[table.first.toStdString()];
		SynGlyphX::SourceDataCache::IndexSetMap::const_iterator dataIndexesForTable = dataIndexes.find(table.first);
		if (dataIndexesForTable == dataIndexes.end()) {

			elasticListsWidget->PopulateElasticLists();
		}
		else {

			elasticListsWidget->PopulateElasticLists(dataIndexesForTable->second);
		}
	}
}

void SourceDataSelectionWidget::ClearElasticLists() {

	for (auto widget : m_elasticListWidgetsForEachTable) {

		m_elasticListsStackLayout->removeWidget(widget.second);
		delete widget.second;
	}
	m_elasticListWidgetsForEachTable.clear();

	m_tableComboBox->blockSignals(true);
	m_tableComboBox->clear();
	m_tableComboBox->setEnabled(false);
}

void SourceDataSelectionWidget::OnElasticListsSelectionChanged(const QString& table, const SynGlyphX::SourceDataCache::ColumnValueData& selection) {

	if (selection.empty()) {

		m_selectionModel->clearSelection();
	}
	else {

		SynGlyphX::SourceDataCache::IndexSetMap indexSets = m_sourceDataCache->SplitIndexSet(SourceDataSelectionModel::GetRootRows(m_selectionModel->selection().indexes()));
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
	}
}

void SourceDataSelectionWidget::EnableButtons(bool enable) {

	m_sourceWidgetButton->setEnabled(enable);
	m_clearButton->setEnabled(enable);
}