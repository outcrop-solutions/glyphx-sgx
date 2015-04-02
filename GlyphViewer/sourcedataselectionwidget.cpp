#include "sourcedataselectionwidget.h"
#include <QtWidgets/QVBoxLayout>
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

	m_sourceWidgetButton = new QPushButton(tr("Show Selected Source Data"), this);
	m_sourceWidgetButton->setCheckable(true);
	m_sourceWidgetButton->setEnabled(false);
	layout->addWidget(m_sourceWidgetButton);

	setLayout(layout);

	m_sourceWidgetButton->setEnabled(!m_selectionModel->selection().empty());
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
}

void SourceDataSelectionWidget::OnComboBoxChanged(int current) {

	m_elasticListsStackLayout->setCurrentWidget(m_elasticListWidgetsForEachTable[m_tableComboBox->currentData().toString().toStdString()]);
}

void SourceDataSelectionWidget::UpdateElasticListsAndSourceDataWidget(const QModelIndexList& selectedIndexes) {

	bool isSelectionNotEmpty = !selectedIndexes.empty();
	m_sourceWidgetButton->setEnabled(isSelectionNotEmpty);
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

		SynGlyphX::IndexSet indexSet = m_sourceDataCache->GetIndexesFromTableWithSelectedValues(table, selection);
		QItemSelection itemSelection;
		for (auto row : indexSet) {

			QModelIndex index = m_model->index(row);
			itemSelection.select(index, index);
		}

		m_selectionModel->select(itemSelection, QItemSelectionModel::ClearAndSelect);
	}
}