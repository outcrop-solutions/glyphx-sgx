#include "FilterSetupWidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QStackedLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTableView>
#include <QtWidgets/QHeaderView>
#include "DMGlobal.h"
#include <QtWidgets/QUndoStack>
#include <QtWidgets/QUndoCommand>
#include "inputfieldmimedata.h"
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include "datatransformmodel.h"
#include <QtWidgets/QAction>
#include "RowMimeData.h"
#include <QtWidgets/QListWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QMenu>
#include <QtWidgets/QCheckBox>
//
//
//FrontEndFilterModel::FrontEndFilterModel(QObject* parent) :
//	QAbstractTableModel(parent) {
//
//
//}
//
//FrontEndFilterModel::~FrontEndFilterModel() {
//
//
//}
//
//int FrontEndFilterModel::rowCount(const QModelIndex& parent) const { 
//
//	if (!m_filters.empty()) {
//
//		return m_currentTableFilters->second.size();
//	}
//	
//	return 0;
//}
//
//void FrontEndFilterModel::SetFilters(const SynGlyphX::MultiTableFrontEndFilters& filters) {
//
//	beginResetModel();
//	m_filters = filters;
//	m_currentTableFilters = m_filters.begin();
//	endResetModel();
//}
//
//const SynGlyphX::MultiTableFrontEndFilters& FrontEndFilterModel::GetFilters() const {
//
//	return m_filters;
//}
//
//void FrontEndFilterModel::SetCurrentTable(const SynGlyphX::InputTable& table) {
//
//	if (table == m_currentTableFilters->first) {
//
//		return;
//	}
//
//	beginResetModel();
//	m_currentTableFilters = m_filters.find(table);
//	endResetModel();
//}
//
//SynGlyphX::InputTable FrontEndFilterModel::GetCurrentTable() { 
//	
//	if (m_filters.empty()) {
//
//		throw std::runtime_error("Can't get current table for filters since there are no filters.");
//	}
//
//	return m_currentTableFilters->first; 
//}
//
//void FrontEndFilterModel::AddField(const SynGlyphX::InputField& newField) {
//
//	SynGlyphX::InputTable newTable(newField);
//	if (m_filters.count(newTable) == 0) {
//
//		SynGlyphX::SingleTableFrontEndFilters newFilterTable;
//		newFilterTable.Insert(newField.GetField(), SynGlyphX::FrontEndFilterOptions());
//		m_filters[newField] = newFilterTable;
//
//		if (m_filters.size() == 1) {
//
//			beginResetModel();
//			m_currentTableFilters = m_filters.begin();
//			endResetModel();
//		}
//	}
//	else if (m_currentTableFilters->first == newTable) {
//
//		int newRow = rowCount();
//		beginInsertRows(QModelIndex(), newRow, newRow);
//		m_currentTableFilters->second.Insert(newField.GetField(), SynGlyphX::FrontEndFilterOptions());
//		endInsertRows();
//		SetCurrentTable(newField);
//	}
//	else {
//		
//		SynGlyphX::MultiTableFrontEndFilters::iterator iT = m_filters.find(newField);
//		iT->second.Insert(newField.GetField(), SynGlyphX::FrontEndFilterOptions());
//		SetCurrentTable(newField);
//	}
//}
//
//void FrontEndFilterModel::RemoveFields(const std::set<std::wstring>& fieldsToRemove) {
//
//	beginResetModel();
//	for (const auto& fieldToRemove : fieldsToRemove) {
//
//		m_currentTableFilters->second.Erase(fieldToRemove);
//	}
//	endResetModel();
//}
//
//void FrontEndFilterModel::RemoveTable(const SynGlyphX::InputTable& table) {
//
//	if (table == m_currentTableFilters->first) {
//
//		throw std::runtime_error("RemoveTable does not support removing of current table from FrontEndFilterModel.");
//	}
//	
//	if (m_filters.size() > 1) {
//
//		m_filters.erase(table);
//	}
//	else {
//
//		m_filters.clear();
//		m_currentTableFilters = m_filters.begin();
//	}
//}
//
//Qt::ItemFlags FrontEndFilterModel::flags(const QModelIndex& index) const {
//
//	if (index.column() == 0) {
//
//		return Qt::ItemNeverHasChildren | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
//	}
//	else {
//
//		return Qt::ItemNeverHasChildren | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsUserCheckable;
//	}
//}
//
//QVariant FrontEndFilterModel::headerData(int section, Qt::Orientation orientation, int role) const {
//
//	if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
//
//		if (section == 0) {
//
//			return tr("Field");
//		}
//		else if (section == 1) {
//
//			return tr("Required Field");
//		}
//		else if (section == 2) {
//
//			return tr("Multiple Selections Allowed");
//		}
//	}
//
//	return QVariant::Invalid;
//}
//
//QVariant FrontEndFilterModel::data(const QModelIndex& index, int role) const {
//
//	if (index.isValid() && !m_filters.empty()) {
//
//		SynGlyphX::SingleTableFrontEndFilters::const_iterator filter = m_currentTableFilters->second.begin();
//		std::advance(filter, index.row());
//		if ((index.column() == 0) && (role == Qt::DisplayRole)) {
//
//			return QString::fromStdWString(filter->first);
//		}
//		else if (role == Qt::CheckStateRole) {
//
//			if (index.column() == 1) {
//
//				if (filter->second.IsRequired()) {
//
//					return Qt::Checked;
//				}
//				else {
//
//					return Qt::Unchecked;
//				}
//			}
//			else if (index.column() == 2) {
//
//				if (filter->second.IsMultiselectAllowed()) {
//
//					return Qt::Checked;
//				}
//				else {
//
//					return Qt::Unchecked;
//				}
//			}
//		}
//	}
//	
//	return QVariant::Invalid;
//}
//
//bool FrontEndFilterModel::setData(const QModelIndex &index, const QVariant &value, int role) {
//
//	if (index.isValid() && !m_filters.empty()) {
//
//		SynGlyphX::SingleTableFrontEndFilters::iterator filter = m_currentTableFilters->second.begin();
//		std::advance(filter, index.row());
//
//		if ((index.column() != 0) && (role == Qt::CheckStateRole)) {
//
//			bool isChecked = (value.toInt() == Qt::Checked);
//
//			if (index.column() == 1) {
//
//				filter->second.SetRequired(isChecked);
//			}
//			else if (index.column() == 2) {
//
//				filter->second.SetAllowMultiselect(isChecked);
//			}
//			else {
//
//				return false;
//			}
//
//			emit dataChanged(index, index, QVector<int>(Qt::CheckStateRole));
//			return true;
//		}
//	}
//
//	return false;
//}
//
//QStringList FrontEndFilterModel::mimeTypes() const {
//
//	QStringList types;
//	types.push_back(SynGlyphX::RowMimeData::s_format);
//	return types;
//}
//
//bool FrontEndFilterModel::canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) const {
//
//	const SynGlyphX::RowMimeData* mimeData = qobject_cast<const SynGlyphX::RowMimeData*>(data);
//
//	return (mimeData != nullptr);
//}
//
//QMimeData* FrontEndFilterModel::mimeData(const QModelIndexList& indexes) const {
//
//	std::set<unsigned int> rows;
//	for (const auto& index : indexes) {
//
//		rows.insert(index.row());
//	}
//
//	SynGlyphX::RowMimeData* mimeData = new SynGlyphX::RowMimeData(rows);
//	return mimeData;
//}
//
//bool FrontEndFilterModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) {
//
//	const SynGlyphX::RowMimeData* rowMimeData = qobject_cast<const SynGlyphX::RowMimeData*>(data);
//
//	if (rowMimeData != nullptr) {
//
//		const std::set<unsigned int>& rowsToMove = rowMimeData->GetRowList();
//		if (!rowsToMove.empty()) {
//
//			SynGlyphX::SingleTableFrontEndFilters filtersToMove;
//			for (const auto& rowToMove : rowsToMove) {
//
//				filtersToMove.push_back(m_currentTableFilters->second[rowToMove]);
//			}
//			if (parent.row() == -1) {
//
//				for (unsigned int i = 0; i < filtersToMove.size(); ++i) {
//
//					m_currentTableFilters->second.Insert(filtersToMove[i].first, filtersToMove[i].second);
//				}
//			}
//			else {
//
//				for (unsigned int i = 0; i < filtersToMove.size(); ++i) {
//
//					m_currentTableFilters->second.Insert(parent.row() + i, filtersToMove[i]);
//				}
//			}
//			
//
//			return true;
//		}
//	}
//
//	return false;
//}
//
//FilterSetupWidget::FilterSetupWidget(QWidget *parent)
//	: QWidget(parent)
//{
//	setMinimumSize(512, 512);
//
//	QVBoxLayout* mainLayout = new QVBoxLayout(this);
//
//	QHBoxLayout* tableLayout = new QHBoxLayout(this);
//	tableLayout->addWidget(new QLabel(tr("Table:")));
//	m_tableComboBox = new QComboBox(this);
//	tableLayout->addWidget(m_tableComboBox, 1);
//	mainLayout->addLayout(tableLayout);
//
//	m_table = new QTableView(this);
//	m_model = new FrontEndFilterModel(m_table);
//	m_table->setModel(m_model);
//
//	m_table->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
//	m_table->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
//	m_table->setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
//	m_table->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
//	m_table->setDragEnabled(true);
//
//	m_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
//	m_table->verticalHeader()->hide();
//	m_table->horizontalHeader()->setStretchLastSection(true);
//	m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::ResizeToContents);
//	m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::ResizeToContents);
//	m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::ResizeToContents);
//	mainLayout->addWidget(m_table, 1);
//
//	setLayout(mainLayout);
//
//	setAcceptDrops(true);
//	setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
//
//	m_removeSelectedAction = new QAction(tr("Remove Selected"), this);
//	QObject::connect(m_removeSelectedAction, &QAction::triggered, this, &FilterSetupWidget::OnRemoveSelected);
//	addAction(m_removeSelectedAction);
//	m_removeTableAction = new QAction(tr("Remove Table"), this);
//	QObject::connect(m_removeTableAction, &QAction::triggered, this, &FilterSetupWidget::OnRemoveTable);
//	addAction(m_removeTableAction);
//	m_removeAllTablesAction = new QAction(tr("Remove All Tables"), this);
//	QObject::connect(m_removeAllTablesAction, &QAction::triggered, this, &FilterSetupWidget::OnRemoveAllTables);
//	addAction(m_removeAllTablesAction);
//
//	QObject::connect(m_table->selectionModel(), &QItemSelectionModel::selectionChanged, this, &FilterSetupWidget::OnSelectionChanged);
//	QObject::connect(m_tableComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &FilterSetupWidget::OnTableSelected);
//}
//
//FilterSetupWidget::~FilterSetupWidget()
//{
//
//}
//
//const SynGlyphX::MultiTableFrontEndFilters& FilterSetupWidget::GetFilters() const {
//
//	return m_model->GetFilters();
//}
//
//void FilterSetupWidget::SetFilters(const SynGlyphX::MultiTableFrontEndFilters& filters) {
//
//	OnRemoveAllTables();
//	m_model->SetFilters(filters);
//	for (const auto& filter : filters) {
//
//		AddTable(filter.first);
//	}
//	if (!filters.empty()) {
//
//		OnTableSelected(0);
//	}
//
//	UpdateContextMenuEnableStates();
//}
//
//void FilterSetupWidget::OnTableSelected(int index) {
//
//	if (index != -1) {
//
//		m_table->clearSelection();
//		m_model->SetCurrentTable(m_inputTables[index]);
//	}
//}
//
//bool FilterSetupWidget::IsTableInUse(const SynGlyphX::InputTable& table) const {
//
//	for (const auto& inputTable : m_inputTables) {
//
//		if (inputTable == table) {
//
//			return true;
//		}
//	}
//	return false;
//}
//
//void FilterSetupWidget::dragEnterEvent(QDragEnterEvent* event) {
//
//	const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
//	if (mimeData != nullptr) {
//
//		event->acceptProposedAction();
//	}
//}
//
//void FilterSetupWidget::dropEvent(QDropEvent* event) {
//
//	const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
//
//	SynGlyphX::InputField newField = mimeData->GetInputField();
//	bool newTableCreated = !IsTableInUse(newField);
//	if (newTableCreated) {
//
//		AddTable(newField);
//		UpdateContextMenuEnableStates();
//	}
//
//	m_model->AddField(newField);
//
//	unsigned int tableIndex = 0;
//	for (; tableIndex < m_inputTables.size(); ++tableIndex) {
//
//		if (m_inputTables[tableIndex] == newField) {
//
//			break;
//		}
//	}
//
//	if (newTableCreated) {
//
//		OnTableSelected(tableIndex);
//	}
//	m_tableComboBox->setCurrentIndex(tableIndex);
//
//	event->acceptProposedAction();
//}
//
//void FilterSetupWidget::AddTable(const SynGlyphX::InputTable& table) {
//
//	m_inputTables.push_back(table);
//	SynGlyphX::DataTransformMapping::ConstSharedPtr dataTransform = DMGlobal::Services()->GetDataTransformModel()->GetDataMapping();
//	QString formattedName = QString::fromStdWString(dataTransform->GetDatasources().at(table.GetDatasourceID())->GetFormattedName());
//	if (table.GetTable() != SynGlyphX::Datasource::SingleTableName) {
//
//		formattedName += ':' + QString::fromStdWString(table.GetTable());
//	}
//	
//	m_tableComboBox->blockSignals(true);
//	m_tableComboBox->addItem(formattedName);
//	m_tableComboBox->blockSignals(false);
//}
//
//void FilterSetupWidget::OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {
//
//	m_removeSelectedAction->setEnabled(!m_table->selectionModel()->selectedRows().empty());
//}
//
//void FilterSetupWidget::OnRemoveSelected() {
//
//	QModelIndexList selectedIndexList = m_table->selectionModel()->selectedRows();
//	if (selectedIndexList.size() == m_model->rowCount()) {
//
//		OnRemoveTable();
//	}
//	else {
//
//		std::set<std::wstring> fieldsToRemove;
//		for (const auto& index : selectedIndexList) {
//
//			fieldsToRemove.insert(m_model->data(index).toString().toStdWString());
//		}
//
//		m_model->RemoveFields(fieldsToRemove);
//	}
//}
//
//void FilterSetupWidget::OnRemoveTable() {
//
//	m_table->clearSelection();
//
//	if (m_model->GetFilters().size() > 1) {
//
//		unsigned int newTable = 0;
//		unsigned int currentTable = m_tableComboBox->currentIndex();
//		if (currentTable == 0) {
//
//			newTable = 1;
//		}
//		else {
//
//			newTable = currentTable - 1;
//		}
//
//		SynGlyphX::InputTable tableToRemove = m_model->GetCurrentTable();
//		m_tableComboBox->setCurrentIndex(newTable);
//		m_model->RemoveTable(tableToRemove);
//		m_tableComboBox->removeItem(currentTable);
//		std::vector<SynGlyphX::InputTable>::iterator toErase = m_inputTables.begin();
//		std::advance(toErase, currentTable);
//		m_inputTables.erase(toErase);
//
//		UpdateContextMenuEnableStates();
//	}
//	else {
//
//		OnRemoveAllTables();
//	}
//}
//
//void FilterSetupWidget::OnRemoveAllTables() {
//
//	m_model->SetFilters(SynGlyphX::MultiTableFrontEndFilters());
//	m_tableComboBox->clear();
//	m_inputTables.clear();
//
//	m_removeSelectedAction->setEnabled(false);
//	UpdateContextMenuEnableStates();
//}
//
//void FilterSetupWidget::UpdateContextMenuEnableStates() {
//
//	bool enable = (m_tableComboBox->count() > 0);
//	m_removeTableAction->setEnabled(enable);
//	m_removeAllTablesAction->setEnabled(enable);
//}

using namespace SynGlyphX;
Q_DECLARE_METATYPE(SynGlyphX::InputTable)
class FieldList : public QListWidget
{
	Q_OBJECT
public:
	FieldList(QWidget* parent) : QListWidget(parent) 	
	{
		setDragDropMode(DropOnly);
		//resize(10, 10);
	}

	void RemoveSelected()
	{
		auto item = selectedItems()[0];
		auto table = item->data(Qt::UserRole).value<InputTable>();
		auto it = std::find(m_tables.begin(), m_tables.end(), table);
		if (it != m_tables.end())
			m_tables.erase(it);
		delete item;

	}
	void dragEnterEvent(QDragEnterEvent *event) override 
	{
		const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
		if (mimeData == nullptr) 
		{
			return;
		}
		//tables in list must be unique
		InputTable table(mimeData->GetInputField()); 
		for (const auto& t : m_tables)
		{
			if (t == table)
				return;
		}
		event->acceptProposedAction();

	}

	void dragMoveEvent(QDragMoveEvent *event)
	{
		event->accept();
	}

	void dropEvent(QDropEvent *event) override
	{
		const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
		if (mimeData != nullptr) 
		{
			auto inputField = mimeData->GetInputField();
			SynGlyphX::DataTransformMapping::ConstSharedPtr dataTransform = DMGlobal::Services()->GetDataTransformModel()->GetDataMapping();
			QString formattedName = QString::fromStdWString(dataTransform->GetDatasources().at(inputField.GetDatasourceID())->GetFormattedName());
			if (inputField.GetTable() != SynGlyphX::Datasource::SingleTableName)
			{
				formattedName += ':' + QString::fromStdWString(inputField.GetTable());
			}
			formattedName += ':' + QString::fromStdWString(inputField.GetField());
			auto item = new QListWidgetItem(formattedName, this);
			QVariant var;
			var.setValue(InputTable(inputField));
			item->setData(Qt::UserRole, var);
			m_tables.push_back(InputTable(inputField));
		}

	}
	// we could use more appropriate containers, but InputTable does not have required operators defined
	std::vector<InputTable> m_tables;
};

class CbWidget : public QWidget
{
public:
	CbWidget(QWidget *parent) : QWidget(parent)
	{
		m_checkBox = new QCheckBox(this);
		QHBoxLayout* l = new QHBoxLayout();
		l->setAlignment(Qt::AlignCenter);
		l->addWidget(m_checkBox);
		setLayout(l);
	}
	void setChecked(bool c) { m_checkBox->setChecked(c); }
	bool isChecked() { return m_checkBox->isChecked(); }
	QCheckBox* m_checkBox;
};

class FilterTableWidget : public QTableWidget
{
	Q_OBJECT
public:
		//FilterTableWidget(QWidget* parent) : QTableWidget(parent) {}
		FilterTableWidget(int rows, int columns, QWidget *parent) : QTableWidget(rows, columns, parent) 
		{
			//setDragDropMode(DragDrop);


		}
		void AddRow()
		{
			auto n = rowCount();
			insertRow(n);
			auto listWgt = new FieldList(this);
			listWgt->setAutoFillBackground(true);
			listWgt->setMaximumHeight(100);
			setCellWidget(n, 0, listWgt);
			CbWidget* cbw1 = new CbWidget(this);
			setCellWidget(n, 1, cbw1);
			CbWidget* cbw2 = new CbWidget(this);
			setCellWidget(n, 2, cbw2);
			resizeRowsToContents();
		}

		void RemoveField()
		{
			
		}
		//void dragEnterEvent(QDragEnterEvent *event) override
		//{
		//	const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
		//	if (mimeData == nullptr) {
		//		return;
		//	}
		//	event->acceptProposedAction();

		//}

		//void dragMoveEvent(QDragMoveEvent *event)
		//{
		//	event->accept();
		//}

		//void dropEvent(QDropEvent *event) override
		//{
		//	const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
		//	if (mimeData != nullptr) {
		//		//addItem(QString::fromStdWString(mimeData->GetInputField().GetField()));
		//		insertRow(rowCount());
		//		
		//	}

		//}

};


FilterSetupWidget::FilterSetupWidget(QWidget *parent)
	: QWidget(parent)
{
	setMinimumSize(300, 512);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QHBoxLayout* tableLayout = new QHBoxLayout(this);
	m_table = new FilterTableWidget(0, 3, this);
	m_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	m_table->verticalHeader()->hide();
	m_table->horizontalHeader()->setStretchLastSection(true);
	//m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::ResizeToContents);
	m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::ResizeToContents);
	m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::ResizeToContents);
	m_table->setHorizontalHeaderLabels(QStringList() << tr("Fields") << tr("Required") << tr("Multiple selection"));

	mainLayout->addWidget(m_table, 1);
	m_table->AddRow();
	mainLayout->addLayout(tableLayout);


	//setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

	//m_removeSelectedAction = new QAction(tr("Remove Selected"), this);
	//QObject::connect(m_removeSelectedAction, &QAction::triggered, this, &FilterSetupWidget::OnRemoveSelected);
	//addAction(m_removeSelectedAction);
	//m_removeTableAction = new QAction(tr("Remove Table"), this);
	//QObject::connect(m_removeTableAction, &QAction::triggered, this, &FilterSetupWidget::OnRemoveTable);
	//addAction(m_removeTableAction);
	//m_removeAllTablesAction = new QAction(tr("Remove All Tables"), this);
	//QObject::connect(m_removeAllTablesAction, &QAction::triggered, this, &FilterSetupWidget::OnRemoveAllTables);
	//addAction(m_removeAllTablesAction);

	m_table->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_table, &QTableWidget::customContextMenuRequested, [=](const QPoint &pos)
	{

		QMenu *menu = new QMenu(this);
		connect(menu, SIGNAL(aboutToHide()), menu, SLOT(deleteLater()));
		int row = m_table->rowAt(pos.y());
		int col = m_table->columnAt(pos.x());
		if (col == 0)
		{
			FieldList* wgt = qobject_cast<FieldList*>(m_table->cellWidget(row, col));
			if (wgt && wgt->selectedItems().size() > 0)
			{
				QAction* addVisAction = menu->addAction(tr("Remove Field"));
				QObject::connect(addVisAction, &QAction::triggered, [=]()
				{
					wgt->RemoveSelected();
				});
			}

		}

		//QTreeWidgetItem* item = m_table->itemAt(pos);
		QAction* addVisAction = menu->addAction(tr("Add Filter"));
		QObject::connect(addVisAction, &QAction::triggered, [=]() {
			m_table->AddRow();
		}
		);

		menu->popup(m_table->viewport()->mapToGlobal(pos));
	});



}
void FilterSetupWidget::SetFilters(const SynGlyphX::MultiTableFrontEndFilters& filters) 
{
	m_filters = filters;
}

const SynGlyphX::MultiTableFrontEndFilters& FilterSetupWidget::GetFilters() const {

	return m_filters;
}

#include "FilterSetupWidget.moc"