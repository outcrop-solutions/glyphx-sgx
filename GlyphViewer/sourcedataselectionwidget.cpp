#include "sourcedataselectionwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QAbstractItemView>
#include "verticalscrollarea.h"
#include "groupboxsinglewidget.h"
#include "elasticlistwidget.h"

SourceDataSelectionWidget::SourceDataSelectionWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, GlyphForestModel* model, QItemSelectionModel* selectionModel, QWidget *parent)
	: QWidget(parent),
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

	SynGlyphX::VerticalScrollArea* scrollArea = new SynGlyphX::VerticalScrollArea(this);

	m_elasticListsLayout = new QStackedLayout(layout);
	layout->addLayout(m_elasticListsLayout, 1);
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

	bool isSelectionNotEmpty = !m_selectionModel->selection().empty();
	m_sourceWidgetButton->setEnabled(isSelectionNotEmpty);
	if (isSelectionNotEmpty) {

		SynGlyphX::SourceDataCache::IndexSet selectedDataRows;
		Q_FOREACH(const QModelIndex& index, m_selectionModel->selection().indexes()) {

			selectedDataRows.insert(GetRootRow(index));
		}

		SynGlyphX::SourceDataCache::IndexSetMap indexSets = m_sourceDataCache->SplitIndexSet(selectedDataRows);

		m_sourceDataWindow->UpdateTables(indexSets);
		UpdateElasticLists(indexSets);
	}
	else {

		m_sourceDataWindow->setVisible(false);
		OnSourceWidgetWindowHidden();
	}
}

void SourceDataSelectionWidget::OnModelReset() {

	if (m_sourceDataCache->IsValid()) {

		const SynGlyphX::SourceDataCache::TableNameMap& tableNameMap = m_sourceDataCache->GetFormattedNames();
		for (auto formattedName : tableNameMap) {

			m_tableComboBox->addItem(formattedName.second, formattedName.first);
			
			SynGlyphX::VerticalScrollArea* scrollArea = new SynGlyphX::VerticalScrollArea(this);
			ElasticListsWidget* elasticListsWidget = new ElasticListsWidget(m_sourceDataCache, formattedName.first, scrollArea);

			scrollArea->setWidget(elasticListsWidget);
			m_elasticListsLayout->addWidget(scrollArea);
			m_elasticListWidgetsForEachTable[formattedName.first.toStdString()] = elasticListsWidget;
		}
		m_tableComboBox->view()->setMinimumWidth(m_tableComboBox->view()->sizeHintForColumn(0));
		m_tableComboBox->setEnabled(true);
		m_tableComboBox->blockSignals(false);
	}
	else {

		m_tableComboBox->blockSignals(true);
		m_tableComboBox->clear();
		m_tableComboBox->setEnabled(false);
		m_elasticListWidgetsForEachTable.clear();
		while (!m_elasticListsLayout->isEmpty()) {

			m_elasticListsLayout->removeWidget(m_elasticListsLayout->widget(0));
		}
	}
}

unsigned long SourceDataSelectionWidget::GetRootRow(const QModelIndex& index) const {

	QModelIndex ancestor = index;
	while (ancestor.parent().isValid()) {

		ancestor = ancestor.parent();
	}

	return ancestor.row();
}

void SourceDataSelectionWidget::OnComboBoxChanged(int current) {

	m_elasticListsLayout->setCurrentWidget(m_elasticListWidgetsForEachTable[m_tableComboBox->currentData().toString().toStdString()]->parentWidget());
}

void SourceDataSelectionWidget::UpdateElasticLists(const SynGlyphX::SourceDataCache::IndexSetMap& dataIndexes) {

	for (auto table : m_sourceDataCache->GetTablesIndexMap()) {

		SynGlyphX::SourceDataCache::IndexSetMap::const_iterator dataIndexesForTable = dataIndexes.find(table.second);
	}
}

void SourceDataSelectionWidget::RemoveElasticLists() {


}