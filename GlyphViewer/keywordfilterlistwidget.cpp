#include "keywordfilterlistwidget.h"
#include "sharedactionlist.h"
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QListView>
#include "glyphbuilderapplication.h"
#include "roledatafilterproxymodel.h"
#include "singlewidgetdialog.h"

KeywordFilterListWidget::KeywordFilterListWidget(SourceDataInfoModel* columnsModel, FilteringManager* filteringManager, QWidget *parent)
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
	QObject::connect(m_addButton, &QPushButton::clicked, this, &KeywordFilterListWidget::OnAddFilter);
	buttonLayout->addWidget(m_addButton, 1);
	m_addButton->setEnabled(false);

	m_removeAllButton = new QPushButton(tr("Remove All"), this);
	QObject::connect(m_removeAllButton, &QPushButton::clicked, this, &KeywordFilterListWidget::OnRemoveAllFilters);
	buttonLayout->addWidget(m_removeAllButton, 1);
	m_removeAllButton->setEnabled(false);

	m_updateButton = new QPushButton(tr("Update 3D View"), this);
	QObject::connect(m_updateButton, &QPushButton::clicked, this, &KeywordFilterListWidget::OnUpdateFilters);
	buttonLayout->addWidget(m_updateButton, 1);
	m_updateButton->setEnabled(false);

	mainLayout->addLayout(buttonLayout);

	QFrame* border = new QFrame(this);
	border->setFrameStyle(QFrame::Shape::HLine | QFrame::Shadow::Sunken);
	mainLayout->addWidget(border);

	QStringList headerLabels;
	headerLabels << tr("Field") << tr("Keyword & Options");

	m_keywordFiltersTableWidget = new QTableWidget(0, 2, this);
	m_keywordFiltersTableWidget->setHorizontalHeaderLabels(headerLabels);
	m_keywordFiltersTableWidget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	m_keywordFiltersTableWidget->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	m_keywordFiltersTableWidget->setFrameShape(QFrame::Shape::NoFrame);
	m_keywordFiltersTableWidget->horizontalHeader()->setStretchLastSection(true);
	m_keywordFiltersTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	m_keywordFiltersTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	m_keywordFiltersTableWidget->verticalHeader()->hide();
	m_keywordFiltersTableWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

	m_removeSelectedContextMenuAction = new QAction(tr("Remove"), m_keywordFiltersTableWidget);
	QObject::connect(m_removeSelectedContextMenuAction, &QAction::triggered, this, &KeywordFilterListWidget::OnRemoveSelectedFilters);
	m_keywordFiltersTableWidget->addAction(m_removeSelectedContextMenuAction);

	//m_keywordFiltersTableWidget->addAction(SynGlyphX::SharedActionList::CreateSeparator(this));

	mainLayout->addWidget(m_keywordFiltersTableWidget, 1);

	setLayout(mainLayout);

	QObject::connect(m_keywordFiltersTableWidget->selectionModel(), &QItemSelectionModel::selectionChanged, this, &KeywordFilterListWidget::OnFilterSelectionChanged);
}

KeywordFilterListWidget::~KeywordFilterListWidget()
{

}

void KeywordFilterListWidget::SwitchTable(const QString& table) {

	//Save ranges and extents for old table
	SaveFiltersInTableWidget();

	//Insert ranges and extents for new table
	const FilteringParameters::ColumnKeywordFilterMap& keywordFilterMap = m_table2FiltersMap[table];
	unsigned int newSize = keywordFilterMap.size();
	m_currentTable = table;
	m_removeAllButton->setEnabled(newSize > 0);

	unsigned int oldRowCount = m_keywordFiltersTableWidget->rowCount();
	m_keywordFiltersTableWidget->setRowCount(newSize);
	for (unsigned int i = oldRowCount; i < newSize; ++i) {

		KeywordFilterWidget* filter = new KeywordFilterWidget(this);
		m_keywordFiltersTableWidget->setCellWidget(i, 1, filter);
	}
	for (unsigned int j = 0; j < newSize; ++j) {

		m_keywordFiltersTableWidget->setItem(j, 0, CreateItem(keywordFilterMap[j].first));
		KeywordFilterWidget* filterWidget = GetKeywordFilterWidgetFromCell(j);
		filterWidget->Set(keywordFilterMap[j].second);
	}
}

void KeywordFilterListWidget::OnNewVisualization() {

	SourceDataCache::ConstSharedPtr sourceDataCache = m_filteringManager->GetSourceDataCache();
	bool isSourceDataCacheValid = sourceDataCache->IsValid();

	m_addButton->setEnabled(isSourceDataCacheValid);
	m_removeAllButton->setEnabled(false);
	m_updateButton->setEnabled(false);
	ClearFiltersFromTableWidget();
	m_table2FiltersMap.clear();
	m_currentTable.clear();

	if (isSourceDataCacheValid) {

		m_currentTable = sourceDataCache->GetFormattedNames().begin()->first;
	}
}

void KeywordFilterListWidget::OnAddFilter() {

	try {

		QStringList datasourceTable = Separate(m_currentTable);

		SynGlyphX::IntRoleDataFilterProxyModel* fieldTypeProxyModel = new SynGlyphX::IntRoleDataFilterProxyModel(this);
		fieldTypeProxyModel->setSourceModel(m_columnsModel);
		fieldTypeProxyModel->setFilterRole(SourceDataInfoModel::TypeRole);
		QSet<int> filterData;
		filterData.insert(SourceDataInfoModel::NoTypeData);
		filterData.insert(SynGlyphX::InputField::Type::Text);
		fieldTypeProxyModel->SetFilterData(filterData);

		SynGlyphX::StringRoleDataFilterProxyModel* filterOutFieldsInUseModel = new SynGlyphX::StringRoleDataFilterProxyModel(this);
		filterOutFieldsInUseModel->setSourceModel(fieldTypeProxyModel);
		filterOutFieldsInUseModel->setFilterRole(Qt::DisplayRole);

		QSet<QString> fieldsInUse;
		for (unsigned int row = 0; row < m_keywordFiltersTableWidget->rowCount(); ++row) {

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
		dialog.setWindowTitle(tr("Select Field(s) For Keyword Filter(s)"));

		if (dialog.exec() == QDialog::Accepted) {

			const QModelIndexList& selected = fieldSelectorWidget->selectionModel()->selectedIndexes();
			if (selected.isEmpty()) {

				return;
			}

			unsigned int nextRow = m_keywordFiltersTableWidget->rowCount();
			m_keywordFiltersTableWidget->setRowCount(nextRow + selected.count());
			boost::uuids::string_generator gen;
			FilteringParameters::ColumnKeywordFilterMap keywordFilterMap;
			for (unsigned int row = 0; row < nextRow; ++row) {

				keywordFilterMap.push_back(FilteringParameters::ColumnKeywordFilter(GetTextFromCell(row), GetKeywordFilterWidgetFromCell(row)->Get()));
			}

			for (const auto& modelIndex : selected) {

				QString field = filterOutFieldsInUseModel->data(modelIndex).toString();

				KeywordFilterWidget* filter = new KeywordFilterWidget(this);
				
				m_keywordFiltersTableWidget->setItem(nextRow, 0, CreateItem(field));
				m_keywordFiltersTableWidget->setCellWidget(nextRow++, 1, filter);

				m_removeAllButton->setEnabled(true);
				QObject::connect(filter, &KeywordFilterWidget::FilterUpdated, this, &KeywordFilterListWidget::OnFilterChanged);
			}

			m_updateButton->setEnabled(true);
		}
	}
	catch (const std::exception& e) {

		QMessageBox::warning(this, tr("Add Filter Error"), tr("Filtering Error: ") + e.what());
	}
}

void KeywordFilterListWidget::OnRemoveSelectedFilters() {

	//SaveRangesFromFiltersInTableWidget();

	const QModelIndexList& modelIndexList = m_keywordFiltersTableWidget->selectionModel()->selectedRows();

	//sort into highest row number first
	std::set<unsigned int, std::greater<unsigned int>> rowsToRemove;
	for (const auto& modelIndex : modelIndexList) {

		rowsToRemove.insert(modelIndex.row());
	}
	unsigned int startingResetRow = *rowsToRemove.rbegin();
	for (unsigned int row : rowsToRemove) {

		m_keywordFiltersTableWidget->removeRow(row);
	}

	m_removeAllButton->setEnabled(m_keywordFiltersTableWidget->rowCount() > 0);
	m_updateButton->setEnabled(DoAnyTablesHaveFilters());
}

void KeywordFilterListWidget::OnRemoveAllFilters() {

	ClearFiltersFromTableWidget();
	//SaveRangesFromFiltersInTableWidget();

	m_removeAllButton->setEnabled(false);
	m_updateButton->setEnabled(DoAnyTablesHaveFilters());
}

void KeywordFilterListWidget::OnUpdateFilters() {

	try {

		SynGlyphX::GlyphBuilderApplication::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
		SaveFiltersInTableWidget();
		for (Table2FiltersMap::const_iterator tableIterator = m_table2FiltersMap.begin(); tableIterator != m_table2FiltersMap.end(); ++tableIterator) {

			FilteringParameters filteringParameters;
			const FilteringManager::Table2FiltersMap& table2FiltersMap = m_filteringManager->GetTable2FiltersMap();
			if (table2FiltersMap.count(tableIterator.key())) {

				filteringParameters = table2FiltersMap[tableIterator.key()];
			}

			FilteringParameters::ColumnKeywordFilterMap keywordFilterMap;
			for (const auto& columnAndKeywordFilter : tableIterator.value()) {

				keywordFilterMap.push_back(FilteringParameters::ColumnKeywordFilter(columnAndKeywordFilter.first, columnAndKeywordFilter.second));
			}
			filteringParameters.SetKeywordFilters(keywordFilterMap);

			if (!m_filteringManager->GenerateFilterResultsForTable(tableIterator.key(), filteringParameters)) {

				QMessageBox::warning(this, tr("Filtering"), tr("No glyphs match the current combination of filters."));
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

QStringList KeywordFilterListWidget::Separate(const QString& datasourceTable) const {

	QStringList splitDatasourceTable = datasourceTable.split(':');
	if (splitDatasourceTable.count() < 2) {

		splitDatasourceTable.push_back(QString::fromStdWString(SynGlyphX::Datasource::SingleTableName));
	}

	return splitDatasourceTable;
}

void KeywordFilterListWidget::ClearFiltersFromTableWidget() {

	while (m_keywordFiltersTableWidget->rowCount() > 0) {

		m_keywordFiltersTableWidget->removeRow(0);
	}
}

KeywordFilterWidget* KeywordFilterListWidget::GetKeywordFilterWidgetFromCell(int row, int column) const {

	return dynamic_cast<KeywordFilterWidget*>(m_keywordFiltersTableWidget->cellWidget(row, column));
}

QString KeywordFilterListWidget::GetTextFromCell(int row, int column) const {

	return m_keywordFiltersTableWidget->item(row, column)->text();
}

void KeywordFilterListWidget::SaveFiltersInTableWidget() {

	FilteringParameters::ColumnKeywordFilterMap keywordFilterMap;
	for (unsigned int k = 0; k < m_keywordFiltersTableWidget->rowCount(); ++k) {

		FilteringParameters::ColumnKeywordFilter columnKeywordFilter;
		columnKeywordFilter.first = GetTextFromCell(k);
		KeywordFilterWidget* filterWidget = GetKeywordFilterWidgetFromCell(k);
		columnKeywordFilter.second = filterWidget->Get();

		keywordFilterMap.push_back(columnKeywordFilter);
	}

	m_table2FiltersMap[m_currentTable] = keywordFilterMap;
}

bool KeywordFilterListWidget::DoAnyTablesHaveFilters() const {

	for (const auto& filtersInTable : m_table2FiltersMap) {

		if (!filtersInTable.empty()) {

			return true;
		}
	}

	return false;
}

void KeywordFilterListWidget::OnFilterSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {

	const QModelIndexList& modelIndexList = m_keywordFiltersTableWidget->selectionModel()->selectedRows();
	m_removeSelectedContextMenuAction->setEnabled(!modelIndexList.empty());
}

QTableWidgetItem* KeywordFilterListWidget::CreateItem(const QString& text) {

	QTableWidgetItem* item = new QTableWidgetItem(text);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren);

	return item;
}

void KeywordFilterListWidget::OnFilterChanged() {

	m_updateButton->setEnabled(true);
}