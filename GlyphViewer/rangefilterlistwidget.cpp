#include "rangefilterlistwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTableView>
#include <QtWidgets/QHeaderView>
#include "singlewidgetdialog.h"

RangeFilterListWidget::RangeFilterListWidget(SourceDataInfoModel* columnsModel, SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, SourceDataSelectionModel* selectionModel, QWidget *parent)
	: QWidget(parent),
	m_selectionModel(selectionModel),
	m_sourceDataCache(sourceDataCache),
	m_columnsModel(columnsModel),
	m_currentTable("")
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);

	m_filtersLayout = new QStackedLayout(this);
	m_filtersLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->addLayout(m_filtersLayout, 1);

	QFrame* border = new QFrame(this);
	border->setFrameStyle(QFrame::Shape::HLine | QFrame::Shadow::Sunken);
	mainLayout->addWidget(border);

	QWidget* buttonsWidget = new QWidget(this);

	QHBoxLayout* buttonLayout = new QHBoxLayout(this);
	m_addButton = new QPushButton(tr("Add"), this);
	QObject::connect(m_addButton, &QPushButton::clicked, this, &RangeFilterListWidget::OnAddFilter);
	buttonLayout->addWidget(m_addButton);
	m_addButton->setEnabled(false);

	m_removeAllButton = new QPushButton(tr("Remove All"), this);
	QObject::connect(m_removeAllButton, &QPushButton::clicked, this, &RangeFilterListWidget::OnRemoveAllFilters);
	buttonLayout->addWidget(m_removeAllButton);
	m_removeAllButton->setEnabled(false);

	m_updateButton = new QPushButton(tr("Update"), this);
	QObject::connect(m_updateButton, &QPushButton::clicked, this, &RangeFilterListWidget::OnUpdateFilters);
	buttonLayout->addWidget(m_updateButton);
	m_updateButton->setEnabled(false);

	buttonsWidget->setLayout(buttonLayout);

	mainLayout->addWidget(buttonsWidget);

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
	m_updateButton->setEnabled(false);

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
			newScrollArea->setFrameShape(QFrame::Shape::NoFrame);
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

	/*QTableView* fieldSelectorWidget = new QTableView(this);
	fieldSelectorWidget->horizontalHeader()->hide();
	fieldSelectorWidget->verticalHeader()->hide();
	fieldSelectorWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	fieldSelectorWidget->setModel(m_columnsModel);
	//fieldSelectorWidget->selectionModel()->select(m_selectedField, QItemSelectionModel::ClearAndSelect);
	//fieldSelectorWidget->scrollTo(m_selectedField);
	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, fieldSelectorWidget, this);
	dialog.setWindowTitle(tr("Select Field For Time Filter"));

	if (dialog.exec() == QDialog::Accepted) {

		
	}*/

	SynGlyphX::SingleNumericRangeFilterWidget* filter = new SynGlyphX::SingleNumericRangeFilterWidget(Qt::Horizontal, this);
	filter->SetMinMax(25.0, 150.0);
	QVBoxLayout* mainLayout = dynamic_cast<QVBoxLayout*>(m_table2WidgetMap[m_currentTable]->widget()->layout());
	mainLayout->insertWidget(m_numericFilters[m_currentTable].size(), filter);
	m_numericFilters[m_currentTable].push_back(filter);
	m_removeAllButton->setEnabled(true);
	QObject::connect(filter, &SynGlyphX::SingleNumericRangeFilterWidget::RangeUpdated, this, &RangeFilterListWidget::OnRangesChanged);
}

void RangeFilterListWidget::OnRemoveAllFilters() {

	for (SynGlyphX::SingleNumericRangeFilterWidget* filter : m_numericFilters[m_currentTable]) {

		m_table2WidgetMap[m_currentTable]->widget()->layout()->removeWidget(filter);
		delete filter;
	}
	m_numericFilters.erase(m_numericFilters.find(m_currentTable));
	m_removeAllButton->setEnabled(false);
}

void RangeFilterListWidget::OnUpdateFilters() {

	m_updateButton->setEnabled(false);
}

void RangeFilterListWidget::OnRangesChanged() {

	m_updateButton->setEnabled(true);
}
