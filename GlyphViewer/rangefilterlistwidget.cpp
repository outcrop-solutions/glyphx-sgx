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
#include "glyphbuilderapplication.h"

RangeFilterListWidget::RangeFilterListWidget(SourceDataInfoModel* columnsModel, SourceDataCache::SharedPtr sourceDataCache, SourceDataSelectionModel* selectionModel, QWidget *parent)
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

	QStringList headerLabels;
	headerLabels << tr("Field") << tr("Range");

	m_rangeFiltersTableWidget = new QTableWidget(0, 2, this);
	m_rangeFiltersTableWidget->setHorizontalHeaderLabels(headerLabels);
	m_rangeFiltersTableWidget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	m_rangeFiltersTableWidget->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	m_rangeFiltersTableWidget->setFrameShape(QFrame::Shape::NoFrame);
	m_rangeFiltersTableWidget->horizontalHeader()->setStretchLastSection(true);
	m_rangeFiltersTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	m_rangeFiltersTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	m_rangeFiltersTableWidget->verticalHeader()->hide();

	mainLayout->addWidget(m_rangeFiltersTableWidget, 1);

	setLayout(mainLayout);

	QObject::connect(m_selectionModel->GetSceneSelectionModel()->model(), &QAbstractItemModel::modelReset, this, &RangeFilterListWidget::OnModelReset);
	QObject::connect(m_selectionModel, &SourceDataSelectionModel::SelectionChanged, this, &RangeFilterListWidget::OnSelectionChanged);
}

RangeFilterListWidget::~RangeFilterListWidget()
{

}

void RangeFilterListWidget::SwitchTable(const QString& table) {

	//Save ranges and extents for old table
	SaveRangesFromFiltersInTableWidget();

	//Insert ranges and extents for new table
	const Field2RangeAndExtentVector& newField2RangeAndExtentList = m_table2RangesAndExtentsMap[table];
	unsigned int newSize = newField2RangeAndExtentList.size();
	m_currentTable = table;
	m_removeAllButton->setEnabled(newSize > 0);

	unsigned int oldRowCount = m_rangeFiltersTableWidget->rowCount();
	m_rangeFiltersTableWidget->setRowCount(newSize);
	for (unsigned int i = oldRowCount; i < newSize; ++i) {

		SynGlyphX::SingleNumericRangeFilterWidget* filter = new SynGlyphX::SingleNumericRangeFilterWidget(Qt::Horizontal, this);
		m_rangeFiltersTableWidget->setCellWidget(i, 1, filter);
	}
	for (unsigned int j = 0; j < newSize; ++j) {

		m_rangeFiltersTableWidget->setItem(j, 0, CreateItem(newField2RangeAndExtentList[j].first));
		SynGlyphX::SingleNumericRangeFilterWidget* filterWidget = GetRangeFilterWidgetFromCell(j);
		filterWidget->SetMaxRangeExtents(newField2RangeAndExtentList[j].second.second);
		filterWidget->SetRange(newField2RangeAndExtentList[j].second.first);
	}
}

void RangeFilterListWidget::OnModelReset() {

	m_addButton->setEnabled(m_sourceDataCache->IsValid());
	m_removeAllButton->setEnabled(false);
	m_updateButton->setEnabled(false);
	ClearFiltersFromTableWidget();
	m_table2RangesAndExtentsMap.clear();
	m_currentTable.clear();

	if (m_sourceDataCache->IsValid()) {

		QStringList headerLabels;
		headerLabels << tr("Field") << tr("Range");

		QTableWidget* newTableWidget = new QTableWidget(0, 2, this);
		newTableWidget->setHorizontalHeaderLabels(headerLabels);
		newTableWidget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
		newTableWidget->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
		newTableWidget->setFrameShape(QFrame::Shape::NoFrame);
		newTableWidget->horizontalHeader()->setStretchLastSection(true);
		newTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
		newTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
		newTableWidget->verticalHeader()->hide();

		m_currentTable = m_sourceDataCache->GetFormattedNames().begin()->first;
	}
}

void RangeFilterListWidget::OnAddFilter() {

	QStringList datasourceTable = Separate(m_currentTable);

	SynGlyphX::RoleDataFilterProxyModel* fieldTypeProxyModel = new SynGlyphX::RoleDataFilterProxyModel(this);
	fieldTypeProxyModel->setSourceModel(m_columnsModel);
	fieldTypeProxyModel->setFilterRole(SourceDataInfoModel::TypeRole);
	QList<int> filterData;
	filterData.push_back(SourceDataInfoModel::NoTypeData);
	filterData.push_back(SynGlyphX::InputField::Type::Real);
	fieldTypeProxyModel->SetFilterData(filterData);

	SynGlyphX::RoleDataFilterProxyModel* filterOutFieldsInUseModel = new SynGlyphX::RoleDataFilterProxyModel(this);
	filterOutFieldsInUseModel->setSourceModel(fieldTypeProxyModel);
	filterOutFieldsInUseModel->setFilterRole(Qt::DisplayRole);
	
	QStringList fieldsInUse;
	for (unsigned int row = 0; row < m_rangeFiltersTableWidget->rowCount(); ++row) {

		fieldsInUse.push_back(GetTextFromCell(row));
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

		unsigned int nextRow = m_rangeFiltersTableWidget->rowCount();
		m_rangeFiltersTableWidget->setRowCount(nextRow + selected.count());
		boost::uuids::string_generator gen;
		SourceDataCache::ColumnMinMaxMap columnMinMaxMap;
		for (unsigned int row = 0; row < nextRow; ++row) {

			columnMinMaxMap.push_back(SourceDataCache::ColumnMinMaxPair(GetTextFromCell(row), GetRangeFilterWidgetFromCell(row)->GetRange()));
		}

		for (const auto& modelIndex : selected) {

			QString field = filterOutFieldsInUseModel->data(modelIndex).toString();

			SynGlyphX::SingleNumericRangeFilterWidget* filter = new SynGlyphX::SingleNumericRangeFilterWidget(Qt::Horizontal, this);
			SynGlyphX::InputField inputField(gen(datasourceTable[0].toStdWString()), datasourceTable[1].toStdWString(), field.toStdWString(), SynGlyphX::InputField::Real);
			
			SynGlyphX::DegenerateInterval minMax = m_sourceDataCache->GetMinMax(inputField, columnMinMaxMap);
			filter->SetMaxRangeExtents(minMax);
			filter->SetRange(minMax);

			m_rangeFiltersTableWidget->setItem(nextRow, 0, CreateItem(field));
			m_rangeFiltersTableWidget->setCellWidget(nextRow++, 1, filter);

			m_removeAllButton->setEnabled(true);
			QObject::connect(filter, &SynGlyphX::SingleNumericRangeFilterWidget::RangeUpdated, this, &RangeFilterListWidget::OnRangesChanged);
		}

		m_updateButton->setEnabled(true);
	}
}

void RangeFilterListWidget::OnRemoveSelectedFilters() {

	SaveRangesFromFiltersInTableWidget();

	m_removeAllButton->setEnabled(m_rangeFiltersTableWidget->rowCount() > 0);
	m_updateButton->setEnabled(DoAnyTablesHaveFilters());
}

void RangeFilterListWidget::OnRemoveAllFilters() {

	ClearFiltersFromTableWidget();
	SaveRangesFromFiltersInTableWidget();
	
	m_removeAllButton->setEnabled(false);
	m_updateButton->setEnabled(DoAnyTablesHaveFilters());
}

void RangeFilterListWidget::OnUpdateFilters() {
	
	try {

		SynGlyphX::GlyphBuilderApplication::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
		SaveRangesFromFiltersInTableWidget();
		for (Table2RangesAndExtentsMap::const_iterator tableIterator = m_table2RangesAndExtentsMap.begin(); tableIterator != m_table2RangesAndExtentsMap.end(); ++tableIterator) {

			SourceDataCache::ColumnMinMaxMap columnMinMaxMap;
			for (const auto& field2RangeAndExtent : tableIterator.value()) {

				columnMinMaxMap.push_back(SourceDataCache::ColumnMinMaxPair(field2RangeAndExtent.first, field2RangeAndExtent.second.first));
			}

			if (columnMinMaxMap.empty()) {

				m_selectionModel->ClearSourceDataSelectionForTable(tableIterator.key());
			}
			else {

				m_selectionModel->SetSourceDataSelectionForTable(tableIterator.key(), m_sourceDataCache->GetIndexesFromTableInRanges(tableIterator.key(), columnMinMaxMap));
			}
		}
		m_updateButton->setEnabled(false);
		SynGlyphX::GlyphBuilderApplication::restoreOverrideCursor();
		
	}
	catch (const std::exception& e) {

		SynGlyphX::GlyphBuilderApplication::restoreOverrideCursor();
		QMessageBox::warning(this, tr("Filtering Error"), tr("Filtering Error: ") + e.what());
	}
}

void RangeFilterListWidget::OnRangesChanged() {

	boost::uuids::string_generator gen;
	QStringList datasourceTable = Separate(m_currentTable);
	SynGlyphX::SingleNumericRangeFilterWidget* updatedFilter = dynamic_cast<SynGlyphX::SingleNumericRangeFilterWidget*>(sender());
	
	SourceDataCache::ColumnMinMaxMap columnMinMaxMap;
	unsigned int row = 0;
	for (; row < m_rangeFiltersTableWidget->rowCount(); ++row) {

		columnMinMaxMap.push_back(SourceDataCache::ColumnMinMaxPair(GetTextFromCell(row), GetRangeFilterWidgetFromCell(row)->GetRange()));
		if (m_rangeFiltersTableWidget->cellWidget(row, 1) == updatedFilter) {

			break;
		}
	}
	++row;
	for (; row < m_rangeFiltersTableWidget->rowCount(); ++row) {

		QString field = GetTextFromCell(row);
		SynGlyphX::SingleNumericRangeFilterWidget* filter = GetRangeFilterWidgetFromCell(row);
		SynGlyphX::InputField inputField(gen(datasourceTable[0].toStdWString()), datasourceTable[1].toStdWString(), field.toStdWString(), SynGlyphX::InputField::Real);
		SynGlyphX::DegenerateInterval minMax = m_sourceDataCache->GetMinMax(inputField, columnMinMaxMap);
		filter->blockSignals(true);
		filter->SetMaxRangeExtents(minMax);
		filter->blockSignals(false);
		columnMinMaxMap.push_back(std::pair<QString, SynGlyphX::DegenerateInterval>(field, minMax));
	}

	m_updateButton->setEnabled(true);
}

SynGlyphX::SingleNumericRangeFilterWidget* RangeFilterListWidget::GetRangeFilterWidgetFromCell(int row, int column) const {

	return dynamic_cast<SynGlyphX::SingleNumericRangeFilterWidget*>(m_rangeFiltersTableWidget->cellWidget(row, column));
}

QString RangeFilterListWidget::GetTextFromCell(int row, int column) const {

	return m_rangeFiltersTableWidget->item(row, column)->text();
}

QStringList RangeFilterListWidget::Separate(const QString& datasourceTable) const {

	QStringList splitDatasourceTable = m_currentTable.split(':');
	if (splitDatasourceTable.count() < 2) {

		splitDatasourceTable.push_back(QString::fromStdWString(SynGlyphX::Datasource::SingleTableName));
	}

	return splitDatasourceTable;
}

void RangeFilterListWidget::ClearFiltersFromTableWidget() {

	while (m_rangeFiltersTableWidget->rowCount() > 0) {

		m_rangeFiltersTableWidget->removeRow(0);
	}
}

QTableWidgetItem* RangeFilterListWidget::CreateItem(const QString& text) {

	QTableWidgetItem* item = new QTableWidgetItem(text);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren);

	return item;
}

void RangeFilterListWidget::SaveRangesFromFiltersInTableWidget() {

	Field2RangeAndExtentVector field2RangeAndExtentList;
	for (unsigned int k = 0; k < m_rangeFiltersTableWidget->rowCount(); ++k) {

		Field2RangeAndExtentPair field2RangeAndExtent;
		field2RangeAndExtent.first = GetTextFromCell(k);
		SynGlyphX::SingleNumericRangeFilterWidget* filterWidget = GetRangeFilterWidgetFromCell(k);
		field2RangeAndExtent.second.first = filterWidget->GetRange();
		field2RangeAndExtent.second.second = filterWidget->GetMaxRangeExtents();

		field2RangeAndExtentList.push_back(field2RangeAndExtent);
	}
	m_table2RangesAndExtentsMap[m_currentTable] = field2RangeAndExtentList;
}

void RangeFilterListWidget::OnSelectionChanged() {

	if (m_selectionModel->GetSourceDataSelection().empty() && (m_rangeFiltersTableWidget->rowCount() > 0)) {

		m_updateButton->setEnabled(true);
	}
}

bool RangeFilterListWidget::DoAnyTablesHaveFilters() const {

	for (const auto& rangesInTable : m_table2RangesAndExtentsMap) {

		if (!rangesInTable.empty()) {

			return true;
		}
	}

	return false;
}