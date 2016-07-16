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

FilteringTable::FilteringTable(SourceDataInfoModel* columnsModel, FilteringManager* filteringManager, const QString& label, bool includeMoveUpDown, QWidget *parent)
	: QWidget(parent),
	m_columnsModel(columnsModel),
	m_filteringManager(filteringManager),
	m_removeSelectedContextMenuAction(nullptr),
	m_moveRowUpContextMenuAction(nullptr),
	m_moveRowDownContextMenuAction(nullptr),
	m_currentTable("")
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->setContentsMargins(8, 8, 8, 8);

	m_addButton = new QPushButton(tr("Add"), this);
	QObject::connect(m_addButton, &QPushButton::clicked, this, &FilteringTable::OnAddFilter);
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

	m_filterTableWidget = new QTableWidget(0, 2, this);
	m_filterTableWidget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	m_filterTableWidget->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	m_filterTableWidget->setFrameShape(QFrame::Shape::NoFrame);
	m_filterTableWidget->horizontalHeader()->setStretchLastSection(true);
	m_filterTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	m_filterTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	m_filterTableWidget->verticalHeader()->hide();
	m_filterTableWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

	QStringList headerLabels;
	headerLabels << tr("Field") << label;
	m_filterTableWidget->setHorizontalHeaderLabels(headerLabels);

	m_removeSelectedContextMenuAction = new QAction(tr("Remove"), this);
	m_removeSelectedContextMenuAction->setEnabled(false);
	QObject::connect(m_removeSelectedContextMenuAction, &QAction::triggered, this, &FilteringTable::OnRemoveSelectedFilters);
	m_filterTableWidget->addAction(m_removeSelectedContextMenuAction);

	if (includeMoveUpDown) {

		m_filterTableWidget->addAction(SynGlyphX::SharedActionList::CreateSeparator(this));

		m_moveRowUpContextMenuAction = new QAction(tr("Move Up"), m_filterTableWidget);
		m_moveRowUpContextMenuAction->setEnabled(false);
		QObject::connect(m_moveRowUpContextMenuAction, &QAction::triggered, this, &FilteringTable::OnMoveUpRow);
		QObject::connect(m_moveRowUpContextMenuAction, &QAction::changed, this, [&, this](){ UpdatedEnableStateForButton(m_moveRowUpContextMenuAction, m_moveUpButton); });
		m_filterTableWidget->addAction(m_moveRowUpContextMenuAction);

		m_moveRowDownContextMenuAction = new QAction(tr("Move Down"), m_filterTableWidget);
		m_moveRowDownContextMenuAction->setEnabled(false);
		QObject::connect(m_moveRowDownContextMenuAction, &QAction::triggered, this, &FilteringTable::OnMoveDownRow);
		QObject::connect(m_moveRowDownContextMenuAction, &QAction::changed, this, [&, this](){ UpdatedEnableStateForButton(m_moveRowDownContextMenuAction, m_moveDownButton); });
		m_filterTableWidget->addAction(m_moveRowDownContextMenuAction);
	}

	mainLayout->addWidget(m_filterTableWidget);

	setLayout(mainLayout);

	QObject::connect(m_filteringManager, &FilteringManager::FilterResultsChanged, this, &FilteringTable::OnFilterResultsChanged);
	QObject::connect(m_filterTableWidget->selectionModel(), &QItemSelectionModel::selectionChanged, this, &FilteringTable::OnFilterSelectionChanged);
}

FilteringTable::~FilteringTable()
{

}

void FilteringTable::OnRemoveSelectedFilters() {

	const QModelIndexList& modelIndexList = m_filterTableWidget->selectionModel()->selectedRows();

	//sort into highest row number first
	std::set<unsigned int, std::greater<unsigned int>> rowsToRemove;
	for (const auto& modelIndex : modelIndexList) {

		rowsToRemove.insert(modelIndex.row());
	}
	
	for (unsigned int row : rowsToRemove) {

		m_filterTableWidget->removeRow(row);
	}

	m_removeAllButton->setEnabled(m_filterTableWidget->rowCount() > 0);
	m_updateButton->setEnabled(DoAnyTablesHaveFilters());

	UpdateFromSelectedRowsRemoved(*rowsToRemove.rbegin());
}

void FilteringTable::OnFilterSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	const QModelIndexList& modelIndexList = m_filterTableWidget->selectionModel()->selectedRows();
	m_removeSelectedContextMenuAction->setEnabled(!modelIndexList.empty());

	if (m_moveRowUpContextMenuAction != nullptr) {

		if (modelIndexList.count() == 1) {

			int row = modelIndexList[0].row();
			m_moveRowUpContextMenuAction->setEnabled(row > 0);
			m_moveRowDownContextMenuAction->setEnabled(row < (m_filterTableWidget->rowCount() - 1));
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

	if (m_filteringManager->GetFilterResultsByTable().empty() && (m_filterTableWidget->rowCount() > 0)) {

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

	if (isSourceDataCacheValid) {

		m_currentTable = sourceDataCache->GetFormattedNames().begin()->first;
	}
}

void FilteringTable::SwitchTable(const QString& table) {

	SaveFiltersInTableWidget();
	m_currentTable = table;
	ResetForNewTable();
}

void FilteringTable::ClearFiltersFromTableWidget() {

	while (m_filterTableWidget->rowCount() > 0) {

		m_filterTableWidget->removeRow(0);
	}
}

void FilteringTable::OnRemoveAllFilters() {

	ClearFiltersFromTableWidget();

	m_removeAllButton->setEnabled(false);
	m_updateButton->setEnabled(DoAnyTablesHaveFilters());
}

void FilteringTable::OnMoveUpRow() {

	const QModelIndexList& modelIndexList = m_filterTableWidget->selectionModel()->selectedRows();
	unsigned int sourceRow = modelIndexList[0].row();

	MoveRow(sourceRow, sourceRow - 1);
}

void FilteringTable::OnMoveDownRow() {

	const QModelIndexList& modelIndexList = m_filterTableWidget->selectionModel()->selectedRows();
	unsigned int sourceRow = modelIndexList[0].row();

	MoveRow(sourceRow, sourceRow + 1);
}

void FilteringTable::MoveRow(unsigned int sourceRow, unsigned int destinationRow) {

	//default for move row where move row is not enabled
}

QString FilteringTable::GetTextFromCell(int row) const {

	return m_filterTableWidget->item(row, 0)->text();
}

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

QTableWidgetItem* FilteringTable::CreateItem(const QString& text) {

	QTableWidgetItem* item = new QTableWidgetItem(text);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren);

	return item;
}

void FilteringTable::OnAddFilter() {

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
		for (unsigned int row = 0; row < m_filterTableWidget->rowCount(); ++row) {

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

			QSet<QString> newFields;
			for (const auto& modelIndex : selected) {

				newFields.insert(filterOutFieldsInUseModel->data(modelIndex).toString());
			}
			AddFilters(newFields);

			m_removeAllButton->setEnabled(true);
			m_updateButton->setEnabled(true);
		}
	}
	catch (const std::exception& e) {

		QMessageBox::warning(this, tr("Add Filter Error"), tr("Filtering Error: ") + e.what());
	}
}