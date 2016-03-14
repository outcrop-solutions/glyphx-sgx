#include "rangefilterlistwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListView>
#include "singlewidgetdialog.h"
#include "roledatafilterproxymodel.h"
#include "datasource.h"
#include <boost/uuid/uuid_io.hpp>

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

	m_updateButton = new QPushButton(tr("Update 3D View"), this);
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
	m_removeAllButton->setEnabled(!m_numericFilters.find(m_currentTable)->empty());
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

		QStringList headerLabels;
		headerLabels << tr("Field") << tr("Range");

		const SynGlyphX::SourceDataCache::TableNameMap& tableNameMap = m_sourceDataCache->GetFormattedNames();
		for (auto formattedName : tableNameMap) {

			SynGlyphX::VScrollGridWidget* newScrollArea = new SynGlyphX::VScrollGridWidget(headerLabels, this);
			newScrollArea->setFrameShape(QFrame::Shape::NoFrame);
			m_filtersLayout->addWidget(newScrollArea);
			m_table2WidgetMap.insert(formattedName.first, newScrollArea);
			m_numericFilters.insert(formattedName.first, QMultiMap<QString, SynGlyphX::SingleNumericRangeFilterWidget*>());
		}
		m_currentTable = tableNameMap.begin()->first;
	}
}

void RangeFilterListWidget::OnAddFilter() {

	QStringList datasourceTable = Separate(m_currentTable);

	SynGlyphX::RoleDataFilterProxyModel* proxyModel = new SynGlyphX::RoleDataFilterProxyModel(this);
	proxyModel->setSourceModel(m_columnsModel);
	proxyModel->setFilterRole(SourceDataInfoModel::TypeRole);
	proxyModel->SetFilterData(SynGlyphX::SourceDataFieldType::Numeric);

	QListView* fieldSelectorWidget = new QListView(this);
	fieldSelectorWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	fieldSelectorWidget->setModel(proxyModel);
	fieldSelectorWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	fieldSelectorWidget->setRootIndex(proxyModel->mapFromSource(m_columnsModel->GetIndexOfTable(datasourceTable[0], datasourceTable[1])));
	
	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, fieldSelectorWidget, this);
	dialog.setWindowTitle(tr("Select Field(s) For Range Filter(s)"));

	if (dialog.exec() == QDialog::Accepted) {

		const QModelIndexList& selected = fieldSelectorWidget->selectionModel()->selectedIndexes();
		if (selected.isEmpty()) {

			return;
		}
		boost::uuids::string_generator gen;
		const Column2FilterWidgetMap& column2FilterWidgets = m_numericFilters.find(m_currentTable).value();
		SynGlyphX::SourceDataCache::ColumnMinMaxMap columnMinMaxMap;
		Column2FilterWidgetMap::const_iterator columnFilter;
		for (columnFilter = column2FilterWidgets.begin(); columnFilter != column2FilterWidgets.end(); ++columnFilter) {

			columnMinMaxMap.insert(std::pair<QString, std::pair<double, double>>(columnFilter.key(), columnFilter.value()->GetRange()));
		}

		for (const auto& modelIndex : selected) {

			QString field = proxyModel->data(modelIndex).toString();
			QLabel* fieldLabel = new QLabel(field, this);
			fieldLabel->setAlignment(Qt::AlignCenter);

			SynGlyphX::SingleNumericRangeFilterWidget* filter = new SynGlyphX::SingleNumericRangeFilterWidget(Qt::Horizontal, this);
			SynGlyphX::InputField inputField(gen(datasourceTable[0].toStdWString()), datasourceTable[1].toStdWString(), field.toStdWString(), SynGlyphX::InputField::Real);
			
			std::pair<double, double> minMax = m_sourceDataCache->GetMinMax(inputField, columnMinMaxMap);
			filter->SetMinMax(minMax.first, minMax.second);

			SynGlyphX::VScrollGridWidget* gridWidget = m_table2WidgetMap[m_currentTable];
			QList<QWidget*> widgetsToAdd;
			widgetsToAdd.push_back(fieldLabel);
			widgetsToAdd.push_back(filter);
			gridWidget->AddRow(widgetsToAdd);

			m_numericFilters[m_currentTable].insert(field, filter);
			m_removeAllButton->setEnabled(true);
			QObject::connect(filter, &SynGlyphX::SingleNumericRangeFilterWidget::RangeUpdated, this, &RangeFilterListWidget::OnRangesChanged);
		}

		m_updateButton->setEnabled(true);
	}
}

void RangeFilterListWidget::OnRemoveAllFilters() {

	SynGlyphX::VScrollGridWidget* gridWidget = m_table2WidgetMap[m_currentTable];
	gridWidget->RemoveAllWidgets();
	m_numericFilters.find(m_currentTable)->clear();
	m_removeAllButton->setEnabled(false);
	m_updateButton->setEnabled(true);
}

void RangeFilterListWidget::OnUpdateFilters() {

	SourceDataSelectionModel::IndexSetMap indexSets;
	for (Table2FilterWidgetsMap::const_iterator tableFilters = m_numericFilters.begin(); tableFilters != m_numericFilters.end(); ++tableFilters) {

		SynGlyphX::SourceDataCache::ColumnMinMaxMap columnMinMaxMap;
		for (Column2FilterWidgetMap::const_iterator columnFilter = tableFilters->begin(); columnFilter != tableFilters->end(); ++columnFilter) {

			columnMinMaxMap.insert(std::pair<QString, std::pair<double, double>>(columnFilter.key(), columnFilter.value()->GetRange()));
		}

		if (columnMinMaxMap.empty()) {

			m_selectionModel->ClearSourceDataSelectionForTable(tableFilters.key());
		}
		else {

			m_selectionModel->SetSourceDataSelectionForTable(tableFilters.key(), m_sourceDataCache->GetIndexesFromTableInRanges(tableFilters.key(), columnMinMaxMap));
		}
	}
	m_updateButton->setEnabled(false);
}

void RangeFilterListWidget::OnRangesChanged() {

	boost::uuids::string_generator gen;
	QStringList datasourceTable = Separate(m_currentTable);
	SynGlyphX::SingleNumericRangeFilterWidget* updatedFilter = dynamic_cast<SynGlyphX::SingleNumericRangeFilterWidget*>(sender());
	
	const Column2FilterWidgetMap& column2FilterWidgets = m_numericFilters.find(m_currentTable).value();
	SynGlyphX::SourceDataCache::ColumnMinMaxMap columnMinMaxMap;
	Column2FilterWidgetMap::const_iterator columnFilter;
	for (columnFilter = column2FilterWidgets.begin(); columnFilter != column2FilterWidgets.end(); ++columnFilter) {

		columnMinMaxMap.insert(std::pair<QString, std::pair<double, double>>(columnFilter.key(), columnFilter.value()->GetRange()));
		if (columnFilter.value() == updatedFilter) {

			break;
		}
	}
	++columnFilter;
	for (; columnFilter != column2FilterWidgets.end(); ++columnFilter) {

		SynGlyphX::SingleNumericRangeFilterWidget* filter = columnFilter.value();
		SynGlyphX::InputField inputField(gen(datasourceTable[0].toStdWString()), datasourceTable[1].toStdWString(), columnFilter.key().toStdWString(), SynGlyphX::InputField::Real);
		std::pair<double, double> minMax = m_sourceDataCache->GetMinMax(inputField, columnMinMaxMap);
		filter->blockSignals(true);
		filter->SetMinMax(minMax.first, minMax.second);
		filter->blockSignals(false);
		columnMinMaxMap.insert(std::pair<QString, std::pair<double, double>>(columnFilter.key(), minMax));
	}

	m_updateButton->setEnabled(true);
}

QStringList RangeFilterListWidget::Separate(const QString& datasourceTable) const {

	QStringList splitDatasourceTable = m_currentTable.split(':');
	if (splitDatasourceTable.count() < 2) {

		splitDatasourceTable.push_back(QString::fromStdWString(SynGlyphX::Datasource::SingleTableName));
	}

	return splitDatasourceTable;
}