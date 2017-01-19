#include "FilteringTable.h"
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QAction>
#include <set>
#include <functional>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListView>
#include "sharedactionlist.h"
#include "filteringmanager.h"
#include "glyphbuilderapplication.h"
#include "roledatafilterproxymodel.h"
#include "sourcedatainfomodel.h"
#include "singlewidgetdialog.h"

FilteringTable::FilteringTable(SourceDataInfoModel* columnsModel, FilteringManager* filteringManager, SynGlyphX::InputField::Type fieldType, const QString& label, bool includeMoveUpDown, QWidget *parent)
	: QWidget(parent),
    m_removeSelectedContextMenuAction(nullptr),
    m_moveRowUpContextMenuAction(nullptr),
    m_moveRowDownContextMenuAction(nullptr),
	m_columnsModel(columnsModel),
	m_filteringManager(filteringManager),
    m_currentTable(""),
	m_fieldType(fieldType)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->setContentsMargins(8, 8, 8, 8);

	m_addButton = new QPushButton(tr("Add"), this);
	QObject::connect(m_addButton, &QPushButton::clicked, this, &FilteringTable::OnAddFilters);
	buttonLayout->addWidget(m_addButton, 1);
	m_addButton->setEnabled(false);

	m_removeAllButton = new QPushButton(tr("Remove All"), this);
	QObject::connect(m_removeAllButton, &QPushButton::clicked, this, &FilteringTable::OnRemoveAllFilters);
	buttonLayout->addWidget(m_removeAllButton, 1);
	m_removeAllButton->setEnabled(false);

	m_updateButton = new QPushButton(tr("Update 3D View"), this);
	QObject::connect(m_updateButton, &QPushButton::clicked, this, &FilteringTable::OnUpdateFilters);
	buttonLayout->addWidget(m_updateButton, 1);
	m_updateButton->setEnabled(false);

	if (includeMoveUpDown) {

		m_moveUpButton = new QPushButton(this);
		m_moveUpButton->setIcon(QIcon(":SGXGUI/Resources/up_arrow.png"));
		QObject::connect(m_moveUpButton, &QPushButton::clicked, this, &FilteringTable::OnMoveUpRow);
		buttonLayout->addWidget(m_moveUpButton);
		m_moveUpButton->setEnabled(false);

		m_moveDownButton = new QPushButton(this);
		m_moveDownButton->setIcon(QIcon(":SGXGUI/Resources/down_arrow.png"));
		QObject::connect(m_moveDownButton, &QPushButton::clicked, this, &FilteringTable::OnMoveDownRow);
		buttonLayout->addWidget(m_moveDownButton);
		m_moveDownButton->setEnabled(false);
	}

	mainLayout->addLayout(buttonLayout);

	QFrame* border = new QFrame(this);
	border->setFrameStyle(QFrame::Shape::HLine | QFrame::Shadow::Sunken);
	mainLayout->addWidget(border);

	m_filterListTableWidget = new QTableWidget(0, 2, this);
	m_filterListTableWidget->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	m_filterListTableWidget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
	m_filterListTableWidget->setFrameShape(QFrame::Shape::NoFrame);
	m_filterListTableWidget->horizontalHeader()->setStretchLastSection(true);
	m_filterListTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	m_filterListTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	m_filterListTableWidget->verticalHeader()->hide();
	m_filterListTableWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

	QStringList headerLabels;
	headerLabels << tr("Field") << label;
	m_filterListTableWidget->setHorizontalHeaderLabels(headerLabels);

	m_removeSelectedContextMenuAction = new QAction(tr("Remove"), m_filterListTableWidget);
	m_removeSelectedContextMenuAction->setEnabled(false);
	QObject::connect(m_removeSelectedContextMenuAction, &QAction::triggered, this, &FilteringTable::OnRemoveSelectedFilters);
	m_filterListTableWidget->addAction(m_removeSelectedContextMenuAction);

	if (includeMoveUpDown) {

		m_filterListTableWidget->addAction(SynGlyphX::SharedActionList::CreateSeparator(this));

		m_moveRowUpContextMenuAction = new QAction(tr("Move Up"), m_filterListTableWidget);
		m_moveRowUpContextMenuAction->setEnabled(false);
		QObject::connect(m_moveRowUpContextMenuAction, &QAction::triggered, this, &FilteringTable::OnMoveUpRow);
		QObject::connect(m_moveRowUpContextMenuAction, &QAction::changed, this, [&, this](){ UpdatedEnableStateForButton(m_moveRowUpContextMenuAction, m_moveUpButton); });
		m_filterListTableWidget->addAction(m_moveRowUpContextMenuAction);

		m_moveRowDownContextMenuAction = new QAction(tr("Move Down"), m_filterListTableWidget);
		m_moveRowDownContextMenuAction->setEnabled(false);
		QObject::connect(m_moveRowDownContextMenuAction, &QAction::triggered, this, &FilteringTable::OnMoveDownRow);
		QObject::connect(m_moveRowDownContextMenuAction, &QAction::changed, this, [&, this](){ UpdatedEnableStateForButton(m_moveRowDownContextMenuAction, m_moveDownButton); });
		m_filterListTableWidget->addAction(m_moveRowDownContextMenuAction);
	}

	mainLayout->addWidget(m_filterListTableWidget);

	setLayout(mainLayout);

	QObject::connect(m_filteringManager, &FilteringManager::FilterResultsChanged, this, &FilteringTable::OnFilterResultsChanged);
	QObject::connect(m_filterListTableWidget->selectionModel(), &QItemSelectionModel::selectionChanged, this, &FilteringTable::OnFilterSelectionChanged);
}

FilteringTable::~FilteringTable()
{

}

void FilteringTable::OnRemoveSelectedFilters() {

	if (DoAnySubTablesHaveAllItemsSelected()) {

		if (QMessageBox::question(this, tr("Remove fields"), tr("One or more fields has all of their filters selected.  That will cause those fields to be removed."
			"  Do you wish to continue?"), QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No) == QMessageBox::No) {

			return;
		}
	}

	//sort into highest row number first
	RowSet rowsWithSelection = GetRowSelectionMap();
	for (const auto& row : rowsWithSelection) {

		m_filterGroups[m_currentTable].RemoveWidget(row);
		m_filterListTableWidget->removeRow(row);
	}

	m_removeAllButton->setEnabled(m_filterListTableWidget->rowCount() > 0);
	m_updateButton->setEnabled(DoAnyTablesHaveFilters());

	UpdateRowSpansInWidget();
	ResetFiltersAfterAddOrRemove();
}

void FilteringTable::OnFilterSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	GroupedRowMap rowsWithSelection = GetGroupedRowSelectionMap();
	m_removeSelectedContextMenuAction->setEnabled(!rowsWithSelection.empty());

	if (m_moveRowUpContextMenuAction != nullptr) {

		if (rowsWithSelection.size() == 1) {

			unsigned int span = rowsWithSelection.begin()->first;
			m_moveRowUpContextMenuAction->setEnabled(span != 0);
			m_moveRowDownContextMenuAction->setEnabled(span != (m_filterGroups[m_currentTable].GetNumberOfGroups() - 1));
		}
		else {

			m_moveRowUpContextMenuAction->setEnabled(false);
			m_moveRowDownContextMenuAction->setEnabled(false);
		}
	}
}

void FilteringTable::UpdatedEnableStateForButton(QAction* action, QPushButton* button) {

	if (action->isEnabled() != button->isEnabled()) {

		button->setEnabled(action->isEnabled());
	}
}

void FilteringTable::OnFilterResultsChanged() {

	if (m_filteringManager->GetFilterResultsByTable().empty() && (m_filterListTableWidget->rowCount() > 0)) {

		m_updateButton->setEnabled(true);
	}
}

void FilteringTable::OnNewVisualization() {

	SourceDataCache::ConstSharedPtr sourceDataCache = m_filteringManager->GetSourceDataCache();
	bool isSourceDataCacheValid = sourceDataCache->IsValid();

	m_addButton->setEnabled(isSourceDataCacheValid);
	m_removeAllButton->setEnabled(false);
	m_updateButton->setEnabled(false);
	ClearFiltersFromTableWidget();
	m_currentTable.clear();
	ClearData();
	m_filterGroups.clear();

	if (isSourceDataCacheValid) {

		m_currentTable = sourceDataCache->GetFormattedNames().begin()->first;
	}
}

void FilteringTable::SwitchTable(const QString& table) {

	SaveFiltersInTableWidget();
	ClearFiltersFromTableWidget();
	RebuildRows(table);
	m_removeAllButton->setEnabled(m_filterGroups[m_currentTable].GetNumberOfGroups() != 0);
}

void FilteringTable::ClearFiltersFromTableWidget() {

	while (m_filterListTableWidget->rowCount() > 0) {

		m_filterListTableWidget->removeRow(0);
	}

	m_filterGroups[m_currentTable].Clear();
}

void FilteringTable::OnRemoveAllFilters() {

	ClearFiltersFromTableWidget();

	m_removeAllButton->setEnabled(false);
	m_updateButton->setEnabled(DoAnyTablesHaveFilters());
}

void FilteringTable::OnMoveUpRow() {

	GroupedRowMap rowsWithSelection = GetGroupedRowSelectionMap();
	unsigned int span = rowsWithSelection.begin()->first;

	MoveRow(span, span - 1);
}

void FilteringTable::OnMoveDownRow() {

	GroupedRowMap rowsWithSelection = GetGroupedRowSelectionMap();
	unsigned int span = rowsWithSelection.begin()->first;

	MoveRow(span, span + 1);
}

void FilteringTable::MoveRow(unsigned int sourceSpan, unsigned int destinationSpan) {

	SaveFiltersInTableWidget();
	MoveRowData(sourceSpan, destinationSpan);

	RebuildRows(m_currentTable);

	m_updateButton->setEnabled(true);
}

void FilteringTable::RebuildRows(const QString& newTable) {

	ClearFiltersFromTableWidget();
	m_currentTable = newTable;
	ResetForNewTable();

	unsigned int row = 0;
	for (unsigned int i = 0; i < m_filterGroups[m_currentTable].GetNumberOfGroups(); ++i) {

		for (unsigned int j = 0; j < m_filterGroups[m_currentTable].GetCountForGroup(i); ++j) {

			m_filterListTableWidget->insertRow(row);

			QTableWidgetItem* fieldItem = new QTableWidgetItem(m_filterGroups[m_currentTable].GetFields().at(i));
			fieldItem->setFlags(Qt::ItemIsEnabled | Qt::ItemNeverHasChildren);
			m_filterListTableWidget->setItem(row, 0, fieldItem);

			m_filterListTableWidget->setCellWidget(row, 1, m_filterGroups[m_currentTable].GetWidget(FilterWidgetGroupsManager::GroupedIndex(i, j)));

			++row;
		}
	}

	UpdateRowSpansInWidget();
	ResetFiltersAfterAddOrRemove();
}

/*QString FilteringTable::GetTextFromCell(int row) const {

	return m_filterListTableWidget->item(row, 0)->text();
}*/

void FilteringTable::OnUpdateFilters() {

	try {

		SynGlyphX::GlyphBuilderApplication::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
		SaveFiltersInTableWidget();
		const FilteringManager::Table2FiltersMap& table2FiltersMap = m_filteringManager->GetTable2FiltersMap();

		const FilteringManager::TableToGlyphTemplateRangesMap& allTables = m_filteringManager->GetTableToGlyphTemplateRangesMap();
		for (FilteringManager::TableToGlyphTemplateRangesMap::const_iterator tableIterator = allTables.begin(); tableIterator != allTables.end(); ++tableIterator) {

			FilteringParameters filteringParameters;
			
			if (table2FiltersMap.count(tableIterator.key())) {

				filteringParameters = table2FiltersMap[tableIterator.key()];
			}

			GetFilteringParametersForTable(tableIterator.key(), filteringParameters);

			if (!m_filteringManager->GenerateFilterResultsForTable(tableIterator.key(), filteringParameters)) {

				QMessageBox::warning(this, tr("Filtering"), tr("No glyphs match the current combination of filters in table: ") + tableIterator.key());
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

QStringList FilteringTable::Separate(const QString& datasourceTable) const {

	QStringList splitDatasourceTable = datasourceTable.split(':');
	if (splitDatasourceTable.count() < 2) {

		splitDatasourceTable.push_back(QString::fromStdWString(SynGlyphX::Datasource::SingleTableName));
	}

	return splitDatasourceTable;
}

void FilteringTable::AddRow(const QString& alias, const QString& field) {

	int span = 0;
	for (; span < int(m_filterGroups[m_currentTable].GetNumberOfGroups()); ++span) {

		if (m_filterGroups[m_currentTable].GetFields().at(span) == field) {

			break;
		}
	}

	QWidget* newFilterWidget = AddFilter(field, span);

	unsigned int row = 0;
	if (span == m_filterGroups[m_currentTable].GetNumberOfGroups()) {

		row = m_filterListTableWidget->rowCount();
		m_filterGroups[m_currentTable].AddWidgetToNewGroup(newFilterWidget, field);
	}
	else {

		for (int i = 0; i <= span; ++i) {

			row += m_filterGroups[m_currentTable].GetCountForGroup(i);
		}
		m_filterGroups[m_currentTable].AddWidgetToGroup(span, newFilterWidget);
	}

	m_filterListTableWidget->insertRow(row);

	QTableWidgetItem* fieldItem = new QTableWidgetItem(alias);
	fieldItem->setFlags(Qt::ItemIsEnabled | Qt::ItemNeverHasChildren);
	m_filterListTableWidget->setItem(row, 0, fieldItem);

	m_filterListTableWidget->setCellWidget(row, 1, newFilterWidget);
}

void FilteringTable::OnAddFilters() {

	try {

		QStringList datasourceTable = Separate(m_currentTable);

		SynGlyphX::IntRoleDataFilterProxyModel* fieldTypeProxyModel = new SynGlyphX::IntRoleDataFilterProxyModel(this);
		fieldTypeProxyModel->setSourceModel(m_columnsModel);
		fieldTypeProxyModel->setFilterRole(SourceDataInfoModel::TypeRole);
		QSet<int> filterData;
		filterData.insert(SourceDataInfoModel::NoTypeData);
		filterData.insert(m_fieldType);
		fieldTypeProxyModel->SetFilterData(filterData);

		QModelIndex rootIndex = fieldTypeProxyModel->mapFromSource(m_columnsModel->GetIndexOfTable(datasourceTable[0], datasourceTable[1]));

		QListView* fieldSelectorWidget = new QListView(this);
		fieldSelectorWidget->setModel(fieldTypeProxyModel);
		fieldSelectorWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
		fieldSelectorWidget->setRootIndex(rootIndex);
		SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, fieldSelectorWidget, this);
		dialog.setMinimumSize(320, 480);
		if (m_fieldType == SynGlyphX::InputField::Type::Text){
			dialog.setWindowTitle(tr("Select Field(s) For Keyword Filter(s)"));
		}
		else{
			dialog.setWindowTitle(tr("Select Field(s) For Range Filter(s)"));
		}

		if (dialog.exec() == QDialog::Accepted) {

			const QModelIndexList& selected = fieldSelectorWidget->selectionModel()->selectedIndexes();
			if (selected.isEmpty()) {

				return;
			}

			//QSet<QString> newFields;
			for (const auto& modelIndex : selected) {

				AddRow(fieldTypeProxyModel->data(modelIndex).toString(), fieldTypeProxyModel->data(modelIndex, SourceDataInfoModel::FieldRole).toString());
			}

			UpdateRowSpansInWidget();
			m_removeAllButton->setEnabled(true);
			m_updateButton->setEnabled(true);
			ResetFiltersAfterAddOrRemove();
		}
	}
	catch (const std::exception& e) {

		QMessageBox::warning(this, tr("Add Filter Error"), tr("Filtering Error: ") + e.what());
	}
}

void FilteringTable::OnFilterChanged() {

	m_updateButton->setEnabled(true);
}

bool FilteringTable::DoAnySubTablesHaveAllItemsSelected() const {

	/*RowSet rowSelectionMap = GetRowSelectionMap();
	std::vector<unsigned int, RowSet> field2RowsMap;
	QString previousField;
	int field2RowsMapIndex
	for (unsigned int i = 0; i < m_filterListTableWidget->rowCount(); ++i) {

		
	}*/

	return false;
}

FilteringTable::RowSet FilteringTable::GetRowSelectionMap() const {

	RowSet rows;
	for (const auto& modelIndex : m_filterListTableWidget->selectionModel()->selectedIndexes()) {

		rows.insert(modelIndex.row());
	}

	return rows;

	/*RowMap rowSelectionMap;
	unsigned int baseRow = 0;
	const std::vector<unsigned int>& rowSpans = m_table2RowSpanList[m_currentTable];
	std::set<unsigned int>::const_iterator row = rows.begin();
	for (unsigned int rowSpanIndex = 0; rowSpanIndex < rowSpans.size(); ++rowSpanIndex) {

		std::set<unsigned int, std::greater<unsigned int>> subRows;
		while ((row != rows.end()) && (*row < baseRow + rowSpans[rowSpanIndex])) {

			subRows.insert(*row - baseRow);
			++row;
		}

		if (!subRows.empty()) {

			rowSelectionMap[rowSpanIndex] = subRows;
		}

		baseRow += rowSpans[rowSpanIndex];
	}

	return rowSelectionMap;*/
}

FilteringTable::GroupedRowMap FilteringTable::GetGroupedRowSelectionMap() const {

	GroupedRowMap rowMap;
	for (const auto& modelIndex : m_filterListTableWidget->selectionModel()->selectedIndexes()) {

		FilterWidgetGroupsManager::GroupedIndex index = m_filterGroups[m_currentTable].GetGroupedIndex(modelIndex.row());
		rowMap[index.first].push_back(index.second);
	}
	return rowMap;
}

void FilteringTable::UpdateRowSpansInWidget() {

	unsigned int firstTableRow = 0;
	std::vector<unsigned int> rowSpans = m_filterGroups[m_currentTable].GetCountGorEachGroup();
	for (unsigned int i = 0; i < rowSpans.size(); ++i) {

		m_filterListTableWidget->setSpan(firstTableRow, 0, rowSpans[i], 1);
		firstTableRow += rowSpans[i];
	}
}

void FilteringTable::MoveRowData(unsigned int sourceSpan, unsigned int destinationSpan) {


}

FilteringTable::FilterWidgetGroupsManager::FilterWidgetGroupsManager() {


}

FilteringTable::FilterWidgetGroupsManager::~FilterWidgetGroupsManager() {


}

QWidget* FilteringTable::FilterWidgetGroupsManager::GetWidget(unsigned int index) const {

	try {

		return GetWidget(GetGroupedIndex(index));
	}
	catch (const std::exception&) {

		throw;
	}
}

QWidget* FilteringTable::FilterWidgetGroupsManager::GetWidget(const GroupedIndex index) const {

	return m_filterWidgets.at(index.first).at(index.second);
}

const QStringList& FilteringTable::FilterWidgetGroupsManager::GetFields() const {

	return m_fields;
}

void FilteringTable::FilterWidgetGroupsManager::AddWidgetToGroup(unsigned int group, QWidget* widget) {

	m_filterWidgets[group].push_back(widget);
}

void FilteringTable::FilterWidgetGroupsManager::AddWidgetToNewGroup(QWidget* widget, const QString& field) {

	FilterWidgetGroup group;
	group.push_back(widget);
	AddGroup(group, field);
}

void FilteringTable::FilterWidgetGroupsManager::AddGroup(const FilterWidgetGroup& group, const QString& field) {

	m_filterWidgets.push_back(group);
	m_fields.push_back(field);
}

void FilteringTable::FilterWidgetGroupsManager::RemoveWidget(unsigned int index) {

	try {

		RemoveWidget(GetGroupedIndex(index));
	}
	catch (const std::exception&) {

		throw;
	}
}

void FilteringTable::FilterWidgetGroupsManager::RemoveWidget(const GroupedIndex index) {

	if (m_filterWidgets[index.first].size() == 1) {

		std::vector<FilterWidgetGroup>::iterator iT = m_filterWidgets.begin();
		std::advance(iT, index.first);
		m_filterWidgets.erase(iT);
		m_fields.removeAt(index.first);
	}
	else {

		FilterWidgetGroup::iterator iT = m_filterWidgets[index.first].begin();
		std::advance(iT, index.second);
		m_filterWidgets[index.first].erase(iT);
	}
}

void FilteringTable::FilterWidgetGroupsManager::Clear() {

	m_filterWidgets.clear();
	m_fields.clear();
}

unsigned int FilteringTable::FilterWidgetGroupsManager::GetNumberOfGroups() const {

	return static_cast<unsigned int>(m_filterWidgets.size());
}

unsigned int FilteringTable::FilterWidgetGroupsManager::GetCountForGroup(unsigned int group) const {

	return static_cast<unsigned int>(m_filterWidgets[group].size());
}

std::vector<unsigned int> FilteringTable::FilterWidgetGroupsManager::GetCountGorEachGroup() const {

	std::vector<unsigned int> counts;

	for (const auto& group : m_filterWidgets) {

		counts.push_back(static_cast<unsigned int>(group.size()));
	}

	return counts;
}

FilteringTable::FilterWidgetGroupsManager::GroupedIndex FilteringTable::FilterWidgetGroupsManager::GetGroupedIndex(unsigned int index) const {

	unsigned int totalSize = 0;
	for (unsigned int i = 0; i < m_filterWidgets.size(); ++i) {

		if (index < totalSize + m_filterWidgets[i].size()) {

			return GroupedIndex(i, index - totalSize);
		}

		totalSize += static_cast<unsigned int>(m_filterWidgets[i].size());
	}

	throw std::invalid_argument("Index exceeds total number of widgets.");
}

void FilteringTable::FilterWidgetGroupsManager::SwapGroups(unsigned int sourceGroup, unsigned int destinationGroup) {

	std::vector<FilterWidgetGroup>::iterator iT = m_filterWidgets.begin();
	std::advance(iT, sourceGroup);
	FilterWidgetGroup groupToMove = *iT;
	m_filterWidgets.erase(iT);

	QString fieldToMove = m_fields[sourceGroup];
	m_fields.removeAt(sourceGroup);

	iT = m_filterWidgets.begin();
	std::advance(iT, destinationGroup);
	m_filterWidgets.insert(iT, groupToMove);

	m_fields.insert(destinationGroup, fieldToMove);
}
