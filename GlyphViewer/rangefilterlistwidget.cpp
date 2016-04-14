#include "rangefilterlistwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListView>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QAction>
#include "singlewidgetdialog.h"
#include "roledatafilterproxymodel.h"
#include "datasource.h"
#include <boost/uuid/uuid_io.hpp>
#include "glyphbuilderapplication.h"
#include "sharedactionlist.h"

RangeFilterListWidget::RangeFilterListWidget(SourceDataInfoModel* columnsModel, FilteringManager* filteringManager, QWidget *parent)
	: QWidget(parent),
	m_filteringManager(filteringManager),
	m_columnsModel(columnsModel),
	m_currentTable("")
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->setContentsMargins(8, 8, 8, 8);

	m_addButton = new QPushButton(tr("Add"), this);
	QObject::connect(m_addButton, &QPushButton::clicked, this, &RangeFilterListWidget::OnAddFilter);
	buttonLayout->addWidget(m_addButton, 1);
	m_addButton->setEnabled(false);

	m_removeAllButton = new QPushButton(tr("Remove All"), this);
	QObject::connect(m_removeAllButton, &QPushButton::clicked, this, &RangeFilterListWidget::OnRemoveAllFilters);
	buttonLayout->addWidget(m_removeAllButton, 1);
	m_removeAllButton->setEnabled(false);

	m_updateButton = new QPushButton(tr("Update 3D View"), this);
	QObject::connect(m_updateButton, &QPushButton::clicked, this, &RangeFilterListWidget::OnUpdateFilters);
	buttonLayout->addWidget(m_updateButton, 1);
	m_updateButton->setEnabled(false);

	m_moveUpButton = new QPushButton(this);
	m_moveUpButton->setIcon(QIcon(":SGXGUI/Resources/up_arrow.png"));
	QObject::connect(m_moveUpButton, &QPushButton::clicked, this, &RangeFilterListWidget::OnMoveUpRow);
	buttonLayout->addWidget(m_moveUpButton);
	m_moveUpButton->setEnabled(false);

	m_moveDownButton = new QPushButton(this);
	m_moveDownButton->setIcon(QIcon(":SGXGUI/Resources/down_arrow.png"));
	QObject::connect(m_moveDownButton, &QPushButton::clicked, this, &RangeFilterListWidget::OnMoveDownRow);
	buttonLayout->addWidget(m_moveDownButton);
	m_moveDownButton->setEnabled(false);

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
	m_rangeFiltersTableWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
	//m_rangeFiltersTableWidget->setDragDropMode(QAbstractItemView::InternalMove);
	//m_rangeFiltersTableWidget->setDropIndicatorShown(true);

	m_removeSelectedContextMenuAction = new QAction(tr("Remove"), m_rangeFiltersTableWidget);
	QObject::connect(m_removeSelectedContextMenuAction, &QAction::triggered, this, &RangeFilterListWidget::OnRemoveSelectedFilters);
	m_rangeFiltersTableWidget->addAction(m_removeSelectedContextMenuAction);

	m_rangeFiltersTableWidget->addAction(SynGlyphX::SharedActionList::CreateSeparator(this));

	m_moveRowUpContextMenuAction = new QAction(tr("Move Up"), m_rangeFiltersTableWidget);
	QObject::connect(m_moveRowUpContextMenuAction, &QAction::triggered, this, &RangeFilterListWidget::OnMoveUpRow);
	QObject::connect(m_moveRowUpContextMenuAction, &QAction::changed, this, [&, this](){ UpdatedEnableStateForButton(m_moveRowUpContextMenuAction, m_moveUpButton); });
	m_rangeFiltersTableWidget->addAction(m_moveRowUpContextMenuAction);

	m_moveRowDownContextMenuAction = new QAction(tr("Move Down"), m_rangeFiltersTableWidget);
	QObject::connect(m_moveRowDownContextMenuAction, &QAction::triggered, this, &RangeFilterListWidget::OnMoveDownRow);
	QObject::connect(m_moveRowDownContextMenuAction, &QAction::changed, this, [&, this](){ UpdatedEnableStateForButton(m_moveRowDownContextMenuAction, m_moveDownButton); });
	m_rangeFiltersTableWidget->addAction(m_moveRowDownContextMenuAction);

	mainLayout->addWidget(m_rangeFiltersTableWidget, 1);

	setLayout(mainLayout);

	QObject::connect(m_filteringManager->GetSceneSelectionModel()->model(), &QAbstractItemModel::modelReset, this, &RangeFilterListWidget::OnModelReset);
	QObject::connect(m_filteringManager, &FilteringManager::FilterResultsChanged, this, &RangeFilterListWidget::OnFilterResultsChanged);
	QObject::connect(m_rangeFiltersTableWidget->selectionModel(), &QItemSelectionModel::selectionChanged, this, &RangeFilterListWidget::OnFilterSelectionChanged);
}

RangeFilterListWidget::~RangeFilterListWidget()
{

}

void RangeFilterListWidget::SwitchTable(const QString& table) {

	//Save ranges and extents for old table
	SaveRangesFromFiltersInTableWidget();

	//Insert ranges and extents for new table
	const Field2RangeAndDistinctValuesVector& newField2RangeAndExtentList = m_table2RangesAndDistinctValuesMap[table];
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
		filterWidget->SetSliderPositionValuesAndMaxExtents(newField2RangeAndExtentList[j].second.second);
		filterWidget->SetRange(newField2RangeAndExtentList[j].second.first);
	}
}

void RangeFilterListWidget::OnModelReset() {

	SourceDataCache::ConstSharedPtr sourceDataCache = m_filteringManager->GetSourceDataCache();
	bool isSourceDataCacheValid = sourceDataCache->IsValid();

	m_addButton->setEnabled(isSourceDataCacheValid);
	m_removeAllButton->setEnabled(false);
	m_updateButton->setEnabled(false);
	ClearFiltersFromTableWidget();
	m_table2RangesAndDistinctValuesMap.clear();
	m_currentTable.clear();

	if (isSourceDataCacheValid) {

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

		m_currentTable = sourceDataCache->GetFormattedNames().begin()->first;
	}
}

void RangeFilterListWidget::OnAddFilter() {

	try {

		QStringList datasourceTable = Separate(m_currentTable);

		SynGlyphX::IntRoleDataFilterProxyModel* fieldTypeProxyModel = new SynGlyphX::IntRoleDataFilterProxyModel(this);
		fieldTypeProxyModel->setSourceModel(m_columnsModel);
		fieldTypeProxyModel->setFilterRole(SourceDataInfoModel::TypeRole);
		QSet<int> filterData;
		filterData.insert(SourceDataInfoModel::NoTypeData);
		filterData.insert(SynGlyphX::InputField::Type::Real);
		fieldTypeProxyModel->SetFilterData(filterData);

		SynGlyphX::StringRoleDataFilterProxyModel* filterOutFieldsInUseModel = new SynGlyphX::StringRoleDataFilterProxyModel(this);
		filterOutFieldsInUseModel->setSourceModel(fieldTypeProxyModel);
		filterOutFieldsInUseModel->setFilterRole(Qt::DisplayRole);

		QSet<QString> fieldsInUse;
		for (unsigned int row = 0; row < m_rangeFiltersTableWidget->rowCount(); ++row) {

			fieldsInUse.insert(GetTextFromCell(row));
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
			FilteringParameters::ColumnRangeFilterMap rangeFilterMap;
			for (unsigned int row = 0; row < nextRow; ++row) {

				rangeFilterMap.push_back(FilteringParameters::ColumnRangeFilter(GetTextFromCell(row), GetRangeFilterWidgetFromCell(row)->GetRange()));
			}

			for (const auto& modelIndex : selected) {

				QString field = filterOutFieldsInUseModel->data(modelIndex).toString();

				SynGlyphX::SingleNumericRangeFilterWidget* filter = new SynGlyphX::SingleNumericRangeFilterWidget(Qt::Horizontal, this);
				SynGlyphX::InputField inputField(gen(datasourceTable[0].toStdWString()), datasourceTable[1].toStdWString(), field.toStdWString(), SynGlyphX::InputField::Real);

				SynGlyphX::SingleNumericRangeFilterWidget::SliderPositionValues sliderPositionValues = m_filteringManager->GetSourceDataCache()->GetSortedNumericDistictValues(inputField, rangeFilterMap);
				filter->SetSliderPositionValuesAndMaxExtents(sliderPositionValues);
				filter->SetRange(SynGlyphX::DegenerateInterval(*sliderPositionValues.begin(), *sliderPositionValues.rbegin()));

				m_rangeFiltersTableWidget->setItem(nextRow, 0, CreateItem(field));
				m_rangeFiltersTableWidget->setCellWidget(nextRow++, 1, filter);

				m_removeAllButton->setEnabled(true);
				QObject::connect(filter, &SynGlyphX::SingleNumericRangeFilterWidget::RangeUpdated, this, &RangeFilterListWidget::OnRangesChanged);
			}

			m_updateButton->setEnabled(true);
		}
	}
	catch (const std::exception& e) {

		QMessageBox::warning(this, tr("Add Filter Error"), tr("Filtering Error: ") + e.what());
	}
}

void RangeFilterListWidget::OnRemoveSelectedFilters() {

	//SaveRangesFromFiltersInTableWidget();
	
	const QModelIndexList& modelIndexList = m_rangeFiltersTableWidget->selectionModel()->selectedRows();

	//sort into highest row number first
	std::set<unsigned int, std::greater<unsigned int>> rowsToRemove;
	for (const auto& modelIndex : modelIndexList) {

		rowsToRemove.insert(modelIndex.row());
	}
	unsigned int startingResetRow = *rowsToRemove.rbegin();
	for (unsigned int row : rowsToRemove) {

		m_rangeFiltersTableWidget->removeRow(row);
	}
	
	m_removeAllButton->setEnabled(m_rangeFiltersTableWidget->rowCount() > 0);
	m_updateButton->setEnabled(DoAnyTablesHaveFilters());

	ResetMinMaxExtentsForFilters(startingResetRow);
}

void RangeFilterListWidget::OnRemoveAllFilters() {

	ClearFiltersFromTableWidget();
	//SaveRangesFromFiltersInTableWidget();
	
	m_removeAllButton->setEnabled(false);
	m_updateButton->setEnabled(DoAnyTablesHaveFilters());
}

void RangeFilterListWidget::OnUpdateFilters() {
	
	try {

		SynGlyphX::GlyphBuilderApplication::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
		SaveRangesFromFiltersInTableWidget();
		for (Table2RangesAndDistinctValuesMap::const_iterator tableIterator = m_table2RangesAndDistinctValuesMap.begin(); tableIterator != m_table2RangesAndDistinctValuesMap.end(); ++tableIterator) {

			FilteringParameters filteringParameters;
			const FilteringManager::Table2FiltersMap& table2FiltersMap = m_filteringManager->GetTable2FiltersMap();
			if (table2FiltersMap.count(tableIterator.key())) {

				filteringParameters.SetDistinctValueFilters(table2FiltersMap[tableIterator.key()].GetDistinctValueFilters());
			}
			
			FilteringParameters::ColumnRangeFilterMap rangeFilterMap;
			for (const auto& field2RangeAndExtent : tableIterator.value()) {

				rangeFilterMap.push_back(FilteringParameters::ColumnRangeFilter(field2RangeAndExtent.first, field2RangeAndExtent.second.first));
			}
			filteringParameters.SetRangeFilters(rangeFilterMap);

			m_filteringManager->GenerateFilterResultsForTable(tableIterator.key(), filteringParameters);
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

	SynGlyphX::SingleNumericRangeFilterWidget* updatedFilter = dynamic_cast<SynGlyphX::SingleNumericRangeFilterWidget*>(sender());
	
	unsigned int row = 0;
	for (; row < m_rangeFiltersTableWidget->rowCount(); ++row) {

		if (m_rangeFiltersTableWidget->cellWidget(row, 1) == updatedFilter) {

			break;
		}
	}
	ResetMinMaxExtentsForFilters(row + 1);

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

	Field2RangeAndDistinctValuesVector field2RangeAndExtentList;
	for (unsigned int k = 0; k < m_rangeFiltersTableWidget->rowCount(); ++k) {

		Field2RangeAndDistinctValues field2RangeAndExtent;
		field2RangeAndExtent.first = GetTextFromCell(k);
		SynGlyphX::SingleNumericRangeFilterWidget* filterWidget = GetRangeFilterWidgetFromCell(k);
		field2RangeAndExtent.second.first = filterWidget->GetRange();
		field2RangeAndExtent.second.second = filterWidget->GetSliderPositionValues();

		field2RangeAndExtentList.push_back(field2RangeAndExtent);
	}
	m_table2RangesAndDistinctValuesMap[m_currentTable] = field2RangeAndExtentList;
}

void RangeFilterListWidget::OnFilterResultsChanged() {

	if (m_filteringManager->GetFilterResultsByTable().empty() && (m_rangeFiltersTableWidget->rowCount() > 0)) {

		m_updateButton->setEnabled(true);
	}
}

bool RangeFilterListWidget::DoAnyTablesHaveFilters() const {

	for (const auto& rangesInTable : m_table2RangesAndDistinctValuesMap) {

		if (!rangesInTable.empty()) {

			return true;
		}
	}

	return false;
}

void RangeFilterListWidget::OnFilterSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {

	const QModelIndexList& modelIndexList = m_rangeFiltersTableWidget->selectionModel()->selectedRows();
	m_removeSelectedContextMenuAction->setEnabled(!modelIndexList.empty());

	if (modelIndexList.count() == 1) {

		int row = modelIndexList[0].row();
		m_moveRowUpContextMenuAction->setEnabled(row > 0);
		m_moveRowDownContextMenuAction->setEnabled(row < (m_rangeFiltersTableWidget->rowCount() - 1));
	}
	else {

		m_moveRowUpContextMenuAction->setEnabled(false);
		m_moveRowDownContextMenuAction->setEnabled(false);
	}
}

void RangeFilterListWidget::UpdatedEnableStateForButton(QAction* action, QPushButton* button) {

	if (action->isEnabled() != button->isEnabled()) {

		button->setEnabled(action->isEnabled());
	}
}

void RangeFilterListWidget::ResetMinMaxExtentsForFilters(unsigned int startingRow) {

	FilteringParameters::ColumnRangeFilterMap columnIntervalMap;
	QStringList datasourceTable = Separate(m_currentTable);
	boost::uuids::string_generator gen;

	for (unsigned int i = 0; i < startingRow; ++i) {

		columnIntervalMap.push_back(FilteringParameters::ColumnRangeFilter(GetTextFromCell(i), GetRangeFilterWidgetFromCell(i)->GetRange()));
	}

	for (unsigned int j = startingRow; j < m_rangeFiltersTableWidget->rowCount(); ++j) {

		QString field = GetTextFromCell(j);
		SynGlyphX::SingleNumericRangeFilterWidget* filter = GetRangeFilterWidgetFromCell(j);
		SynGlyphX::InputField inputField(gen(datasourceTable[0].toStdWString()), datasourceTable[1].toStdWString(), field.toStdWString(), SynGlyphX::InputField::Real);
		SynGlyphX::SingleNumericRangeFilterWidget::SliderPositionValues sliderPositionValues = m_filteringManager->GetSourceDataCache()->GetSortedNumericDistictValues(inputField, columnIntervalMap);
		filter->blockSignals(true);
		filter->SetSliderPositionValuesAndMaxExtents(sliderPositionValues);
		filter->blockSignals(false);
		columnIntervalMap.push_back(std::pair<QString, SynGlyphX::DegenerateInterval>(field, SynGlyphX::DegenerateInterval(*sliderPositionValues.begin(), *sliderPositionValues.rbegin())));
	}
}

void RangeFilterListWidget::OnMoveUpRow() {

	const QModelIndexList& modelIndexList = m_rangeFiltersTableWidget->selectionModel()->selectedRows();
	unsigned int sourceRow = modelIndexList[0].row();

	MoveRow(sourceRow, sourceRow - 1);
}

void RangeFilterListWidget::OnMoveDownRow() {

	const QModelIndexList& modelIndexList = m_rangeFiltersTableWidget->selectionModel()->selectedRows();
	unsigned int sourceRow = modelIndexList[0].row();

	MoveRow(sourceRow, sourceRow + 1);
}

void RangeFilterListWidget::MoveRow(unsigned int sourceRow, unsigned int destinationRow) {

	SaveRangesFromFiltersInTableWidget();

	Field2RangeAndDistinctValuesVector& currentTableRanges = m_table2RangesAndDistinctValuesMap[m_currentTable];
	Field2RangeAndDistinctValuesVector::iterator field2RangeMapIterator = currentTableRanges.begin();
	std::advance(field2RangeMapIterator, sourceRow);

	Field2RangeAndDistinctValues dataToMove = *field2RangeMapIterator;
	currentTableRanges.erase(field2RangeMapIterator);

	field2RangeMapIterator = currentTableRanges.begin();
	std::advance(field2RangeMapIterator, destinationRow);
	currentTableRanges.insert(field2RangeMapIterator, dataToMove);

	m_rangeFiltersTableWidget->removeRow(sourceRow);
	m_rangeFiltersTableWidget->insertRow(destinationRow);

	SynGlyphX::SingleNumericRangeFilterWidget* filter = new SynGlyphX::SingleNumericRangeFilterWidget(Qt::Horizontal, this);
	filter->SetSliderPositionValuesAndMaxExtents(dataToMove.second.second);
	filter->SetRange(dataToMove.second.first);

	m_rangeFiltersTableWidget->setItem(destinationRow, 0, CreateItem(dataToMove.first));
	m_rangeFiltersTableWidget->setCellWidget(destinationRow, 1, filter);

	ResetMinMaxExtentsForFilters(std::min(sourceRow, destinationRow));
}