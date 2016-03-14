#include "filteringwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTabWidget>
#include "groupboxsinglewidget.h"
#include "multitableelasticlistswidget.h"
#include "rangefilterlistwidget.h"

FilteringWidget::FilteringWidget(SourceDataInfoModel* columnsModel, SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, SourceDataSelectionModel* selectionModel, QWidget *parent)
	: QWidget(parent),
	m_selectionModel(selectionModel),
	m_sourceDataCache(sourceDataCache)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QHBoxLayout* topLayout = new QHBoxLayout;

	m_hideUnselectedTreesCheckbox = new QCheckBox(tr("Filter View"), this);
	topLayout->addWidget(m_hideUnselectedTreesCheckbox);

	m_clearButton = new QPushButton(tr("Clear"), this);
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

	RangeFilterListWidget* rangeListFilterWidget = new RangeFilterListWidget(columnsModel, sourceDataCache, selectionModel, filterMethodsWidget);
	filterMethodsWidget->addTab(rangeListFilterWidget, tr("Range"));
	QObject::connect(this, &FilteringWidget::TableChanged, rangeListFilterWidget, &RangeFilterListWidget::SwitchTable);
	MultiTableElasticListsWidget* elasticListsWidget = new MultiTableElasticListsWidget(sourceDataCache, selectionModel, filterMethodsWidget);
	filterMethodsWidget->addTab(elasticListsWidget, tr("Elastic"));
	QObject::connect(this, &FilteringWidget::TableChanged, elasticListsWidget, &MultiTableElasticListsWidget::SwitchTable);

	mainLayout->addWidget(filterMethodsWidget, 1);

	QHBoxLayout* buttonsLayout = new QHBoxLayout(this);

	m_sourceWidgetButton = new QPushButton(tr("Show Selected Source Data"), this);
	m_sourceWidgetButton->setCheckable(true);
	buttonsLayout->addWidget(m_sourceWidgetButton);

	buttonsLayout->addStretch(1);
	mainLayout->addLayout(buttonsLayout);

	setLayout(mainLayout);

	EnableButtons(!m_selectionModel->GetSourceDataSelection().empty());
	m_sourceDataWindow.reset(new SourceDataWidget(m_selectionModel));
	QObject::connect(m_sourceWidgetButton, &QPushButton::toggled, m_sourceDataWindow.data(), &SourceDataWidget::setVisible);
	m_sourceDataWindow->setVisible(false);

	QObject::connect(m_sourceDataWindow.data(), &SourceDataWidget::WindowHidden, this, &FilteringWidget::OnSourceWidgetWindowHidden);
	QObject::connect(m_selectionModel, &SourceDataSelectionModel::SelectionChanged, this, &FilteringWidget::OnSourceDataSelectionChanged);
	QObject::connect(m_selectionModel->GetSceneSelectionModel()->model(), &QAbstractItemModel::modelReset, this, &FilteringWidget::OnModelReset);

	QObject::connect(m_tableComboBox, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), this, &FilteringWidget::OnTableChanged);
}

FilteringWidget::~FilteringWidget()
{

}

void FilteringWidget::SetupLinkedWidgets(LinkedWidgetsManager& linkedWidgets) {

	linkedWidgets.AddFilterViewCheckbox(m_hideUnselectedTreesCheckbox);
}

void FilteringWidget::Clear() {

	m_selectionModel->ClearSourceDataSelection();
}

void FilteringWidget::OnSourceWidgetWindowHidden() {

	m_sourceWidgetButton->setChecked(false);
}

void FilteringWidget::OnSourceDataSelectionChanged() {

	const SourceDataSelectionModel::IndexSetMap& sourceDataSelectionSets = m_selectionModel->GetSourceDataSelection();
	EnableButtons(m_sourceDataCache->IsValid() && (!sourceDataSelectionSets.empty()));

	if (sourceDataSelectionSets.empty()) {

		m_sourceDataWindow->setVisible(false);
		OnSourceWidgetWindowHidden();
	}
	else {

		//Only change the table shown if it is not in the selection at all
		if (sourceDataSelectionSets.count(m_tableComboBox->currentData().toString()) == 0) {

			int newComboBoxIndex = m_tableComboBox->findData(sourceDataSelectionSets.rbegin()->first);
			if ((newComboBoxIndex != -1) && (newComboBoxIndex != m_tableComboBox->currentIndex())) {

				m_tableComboBox->setCurrentIndex(newComboBoxIndex);
			}
		}
	}
}

void FilteringWidget::OnModelReset() {

	m_tableComboBox->blockSignals(true);
	m_tableComboBox->clear();
	m_tableComboBox->setEnabled(m_sourceDataCache->IsValid());

	if (m_sourceDataCache->IsValid()) {

		const SynGlyphX::SourceDataCache::TableNameMap& tableNameMap = m_sourceDataCache->GetFormattedNames();
		for (auto formattedName : tableNameMap) {

			m_tableComboBox->addItem(formattedName.second, formattedName.first);
		}

		m_tableComboBox->view()->setMinimumWidth(m_tableComboBox->view()->sizeHintForColumn(0));
		m_tableComboBox->blockSignals(false);
	}

	OnSourceDataSelectionChanged();
}

void FilteringWidget::OnTableChanged(const QString& table) {

	emit TableChanged(m_tableComboBox->currentData().toString());
}

void FilteringWidget::EnableButtons(bool enable) {

	m_sourceWidgetButton->setEnabled(enable);
	m_clearButton->setEnabled(enable);
}