#include "rangefilterlistwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListView>
#include <QtWidgets/QMessageBox>
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

	QHBoxLayout* buttonLayout = new QHBoxLayout(this);
	buttonLayout->setContentsMargins(8, 8, 8, 8);

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

	mainLayout->addLayout(buttonLayout);

	QFrame* border = new QFrame(this);
	border->setFrameStyle(QFrame::Shape::HLine | QFrame::Shadow::Sunken);
	mainLayout->addWidget(border);

	m_filtersLayout = new QStackedLayout(this);
	m_filtersLayout->setContentsMargins(0, 0, 0, 0);
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
	m_removeAllButton->setEnabled(m_table2WidgetMap[table]->rowCount() > 0);
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

	if (m_sourceDataCache->IsValid()) {

		QStringList headerLabels;
		headerLabels << tr("Field") << tr("Range");

		const SynGlyphX::SourceDataCache::TableNameMap& tableNameMap = m_sourceDataCache->GetFormattedNames();
		for (auto formattedName : tableNameMap) {

			QTableWidget* newTableWidget = new QTableWidget(0, 2, this);
			newTableWidget->setHorizontalHeaderLabels(headerLabels);
			newTableWidget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
			newTableWidget->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
			newTableWidget->setFrameShape(QFrame::Shape::NoFrame);
			newTableWidget->horizontalHeader()->setStretchLastSection(true);
			newTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
			newTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
			newTableWidget->verticalHeader()->hide();
			m_filtersLayout->addWidget(newTableWidget);
			m_table2WidgetMap.insert(formattedName.first, newTableWidget);
		}
		m_currentTable = tableNameMap.begin()->first;
	}
}

void RangeFilterListWidget::OnAddFilter() {

	QStringList datasourceTable = Separate(m_currentTable);

	SynGlyphX::RoleDataFilterProxyModel* fieldTypeProxyModel = new SynGlyphX::RoleDataFilterProxyModel(this);
	fieldTypeProxyModel->setSourceModel(m_columnsModel);
	fieldTypeProxyModel->setFilterRole(SourceDataInfoModel::TypeRole);
	fieldTypeProxyModel->SetFilterData(SynGlyphX::SourceDataFieldType::Numeric);

	SynGlyphX::RoleDataFilterProxyModel* filterOutFieldsInUseModel = new SynGlyphX::RoleDataFilterProxyModel(this);
	filterOutFieldsInUseModel->setSourceModel(fieldTypeProxyModel);
	filterOutFieldsInUseModel->setFilterRole(Qt::DisplayRole);
	QStringList fieldsInUse;
	for (unsigned int row = 0; row < m_table2WidgetMap[m_currentTable]->rowCount(); ++row) {

		fieldsInUse.push_back(m_table2WidgetMap[m_currentTable]->item(row, 0)->text());
	}
	filterOutFieldsInUseModel->SetFilterData(fieldsInUse);
	filterOutFieldsInUseModel->SetNot(true);

	QModelIndex rootIndex = filterOutFieldsInUseModel->mapFromSource(fieldTypeProxyModel->mapFromSource(m_columnsModel->GetIndexOfTable(datasourceTable[0], datasourceTable[1])));
	if (filterOutFieldsInUseModel->rowCount(rootIndex) == 0) {

		QMessageBox::information(this, tr("All fields in use"), tr("All fields for this table are already in use."), QMessageBox::StandardButton::Ok);
		return;
	}

	QListView* fieldSelectorWidget = new QListView(this);
	fieldSelectorWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	fieldSelectorWidget->setModel(filterOutFieldsInUseModel);
	fieldSelectorWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	fieldSelectorWidget->setRootIndex(rootIndex);
	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, fieldSelectorWidget, this);
	dialog.setWindowTitle(tr("Select Field(s) For Range Filter(s)"));

	if (dialog.exec() == QDialog::Accepted) {

		const QModelIndexList& selected = fieldSelectorWidget->selectionModel()->selectedIndexes();
		if (selected.isEmpty()) {

			return;
		}

		QTableWidget* currentTableWidget = m_table2WidgetMap[m_currentTable];
		unsigned int nextRow = currentTableWidget->rowCount();
		currentTableWidget->setRowCount(nextRow + selected.count());
		boost::uuids::string_generator gen;
		SynGlyphX::SourceDataCache::ColumnMinMaxMap columnMinMaxMap;
		for (unsigned int row = 0; row < nextRow; ++row) {

			columnMinMaxMap.push_back(SynGlyphX::SourceDataCache::ColumnMinMaxPair(GetTextFromCell(currentTableWidget, row), GetRangeFromCell(currentTableWidget, row)));
		}

		for (const auto& modelIndex : selected) {

			QString field = filterOutFieldsInUseModel->data(modelIndex).toString();

			SynGlyphX::SingleNumericRangeFilterWidget* filter = new SynGlyphX::SingleNumericRangeFilterWidget(Qt::Horizontal, this);
			SynGlyphX::InputField inputField(gen(datasourceTable[0].toStdWString()), datasourceTable[1].toStdWString(), field.toStdWString(), SynGlyphX::InputField::Real);
			
			std::pair<double, double> minMax = m_sourceDataCache->GetMinMax(inputField, columnMinMaxMap);
			filter->SetMaxRangeExtents(minMax.first, minMax.second);
			filter->SetRange(minMax);

			QTableWidgetItem* item = new QTableWidgetItem(field);
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren);
			currentTableWidget->setItem(nextRow, 0, item);
			currentTableWidget->setCellWidget(nextRow++, 1, filter);

			m_removeAllButton->setEnabled(true);
			QObject::connect(filter, &SynGlyphX::SingleNumericRangeFilterWidget::RangeUpdated, this, &RangeFilterListWidget::OnRangesChanged);
		}

		m_updateButton->setEnabled(true);
	}
}

void RangeFilterListWidget::OnRemoveSelectedFilters() {

}

void RangeFilterListWidget::OnRemoveAllFilters() {

	QTableWidget* currentTableWidget = m_table2WidgetMap[m_currentTable];
	while (currentTableWidget->rowCount() > 0) {

		currentTableWidget->removeRow(0);
	}
	
	m_removeAllButton->setEnabled(false);
	m_updateButton->setEnabled(true);
}

void RangeFilterListWidget::OnUpdateFilters() {
	
	for (QMap<QString, QTableWidget*>::const_iterator table2Widget = m_table2WidgetMap.begin(); table2Widget != m_table2WidgetMap.end(); ++table2Widget) {

		SynGlyphX::SourceDataCache::ColumnMinMaxMap columnMinMaxMap;
		for (unsigned int row = 0; row < table2Widget.value()->rowCount(); ++row) {

			columnMinMaxMap.push_back(SynGlyphX::SourceDataCache::ColumnMinMaxPair(GetTextFromCell(table2Widget.value(), row), GetRangeFromCell(table2Widget.value(), row)));
		}

		if (columnMinMaxMap.empty()) {

			m_selectionModel->ClearSourceDataSelectionForTable(table2Widget.key());
		}
		else {

			m_selectionModel->SetSourceDataSelectionForTable(table2Widget.key(), m_sourceDataCache->GetIndexesFromTableInRanges(table2Widget.key(), columnMinMaxMap));
		}
	}
	m_updateButton->setEnabled(false);
}

void RangeFilterListWidget::OnRangesChanged() {

	boost::uuids::string_generator gen;
	QStringList datasourceTable = Separate(m_currentTable);
	SynGlyphX::SingleNumericRangeFilterWidget* updatedFilter = dynamic_cast<SynGlyphX::SingleNumericRangeFilterWidget*>(sender());
	
	QTableWidget* currentTableWidget = m_table2WidgetMap[m_currentTable];
	SynGlyphX::SourceDataCache::ColumnMinMaxMap columnMinMaxMap;
	unsigned int row = 0;
	for (; row < currentTableWidget->rowCount(); ++row) {

		columnMinMaxMap.push_back(SynGlyphX::SourceDataCache::ColumnMinMaxPair(GetTextFromCell(currentTableWidget, row), GetRangeFromCell(currentTableWidget, row)));
		if (currentTableWidget->cellWidget(row, 1) == updatedFilter) {

			break;
		}
	}
	++row;
	for (; row < currentTableWidget->rowCount(); ++row) {

		QString field = GetTextFromCell(currentTableWidget, row);
		SynGlyphX::SingleNumericRangeFilterWidget* filter = dynamic_cast<SynGlyphX::SingleNumericRangeFilterWidget*>(currentTableWidget->cellWidget(row, 1));
		SynGlyphX::InputField inputField(gen(datasourceTable[0].toStdWString()), datasourceTable[1].toStdWString(), field.toStdWString(), SynGlyphX::InputField::Real);
		std::pair<double, double> minMax = m_sourceDataCache->GetMinMax(inputField, columnMinMaxMap);
		filter->blockSignals(true);
		filter->SetMaxRangeExtents(minMax.first, minMax.second);
		filter->blockSignals(false);
		columnMinMaxMap.push_back(std::pair<QString, std::pair<double, double>>(field, minMax));
	}

	m_updateButton->setEnabled(true);
}

std::pair<double, double> RangeFilterListWidget::GetRangeFromCell(QTableWidget* widget, int row, int column) const {

	SynGlyphX::SingleNumericRangeFilterWidget* filterWidget = dynamic_cast<SynGlyphX::SingleNumericRangeFilterWidget*>(widget->cellWidget(row, column));
	return filterWidget->GetRange();
}

QString RangeFilterListWidget::GetTextFromCell(QTableWidget* widget, int row, int column) const {

	return widget->item(row, column)->text();
}

QStringList RangeFilterListWidget::Separate(const QString& datasourceTable) const {

	QStringList splitDatasourceTable = m_currentTable.split(':');
	if (splitDatasourceTable.count() < 2) {

		splitDatasourceTable.push_back(QString::fromStdWString(SynGlyphX::Datasource::SingleTableName));
	}

	return splitDatasourceTable;
}