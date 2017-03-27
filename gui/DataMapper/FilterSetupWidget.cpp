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
		InputTable table = InputTable(item->data(Qt::UserRole).value<InputField>());
		auto it = std::find(m_tables.begin(), m_tables.end(), table);
		if (it != m_tables.end())
			m_tables.erase(it);
		delete item;

	}
	void AddInputField(const InputField& inputField)
	{
		SynGlyphX::DataTransformMapping::ConstSharedPtr dataTransform = DMGlobal::Services()->GetDataTransformModel()->GetDataMapping();
		auto dsid = inputField.GetDatasourceID();
		auto dataSources = dataTransform->GetDatasources();
		auto search = dataSources.find(dsid);
		if (search != dataSources.end())
		{
			QString formattedName = QString::fromStdWString(search->second->GetFormattedName());
			if (inputField.GetTable() != SynGlyphX::Datasource::SingleTableName)
			{
				formattedName += ':' + QString::fromStdWString(inputField.GetTable());
			}
			formattedName += ':' + QString::fromStdWString(inputField.GetField());
			auto item = new QListWidgetItem(formattedName, NULL);
			insertItem(count(), item);
			QVariant var;
			var.setValue(inputField);
			item->setData(Qt::UserRole, var);
			m_tables.push_back(InputTable(inputField));
		}
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
		if (count() > 0){
			if (item(0)->data(Qt::UserRole).value<InputField>().GetType() != mimeData->GetInputField().GetType()){
				return;
			}
		}
		event->acceptProposedAction();

	}

	void dragMoveEvent(QDragMoveEvent *event) override
	{
		event->accept();
	}

	void dropEvent(QDropEvent *event) override
	{
		const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
		if (mimeData != nullptr) 
		{
			auto inputField = mimeData->GetInputField();
			AddInputField(inputField);
		}

	}
	// we could use more appropriate containers, but InputTable does not have required operators defined
	std::vector<InputTable> m_tables;
};

class CbWidget : public QWidget
{
	Q_OBJECT
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
			setDragDropMode(DragDrop);
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
		
		void SetFilter(int row, const FrontEndFilter& filter)
		{
			auto listWgt = qobject_cast<FieldList*>(cellWidget(row, 0));
			for (const InputField& field : filter.fields)
			{
				listWgt->AddInputField(field);
			}
			qobject_cast<CbWidget*>(cellWidget(row, 1))->setChecked(filter.isRequired);
			qobject_cast<CbWidget*>(cellWidget(row, 2))->setChecked(filter.isMultiselectAllowed);
		}

		FrontEndFilter GetFilter(int row)
		{
			FrontEndFilter filter;
			auto listWgt = qobject_cast<FieldList*>(cellWidget(row, 0));
			for (int i = 0; i < listWgt->count(); i++)
			{
				auto item = listWgt->item(i);
				filter.fields.push_back(item->data(Qt::UserRole).value<InputField>());
			}
			filter.isRequired = qobject_cast<CbWidget*>(cellWidget(row, 1))->isChecked();
			filter.isMultiselectAllowed = qobject_cast<CbWidget*>(cellWidget(row, 2))->isChecked();
			return filter;
		}
		void AddRow(const FrontEndFilter& filter)
		{
			auto n = rowCount();
			AddRow();
			SetFilter(n, filter);
		}
		void Clear()
		{
			while (rowCount() > 0) removeRow(rowCount() - 1);
		}

		void RemoveField()
		{
			
		}
		void dragEnterEvent(QDragEnterEvent *event) override
		{
			const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
			if (mimeData == nullptr)
			{
				return;
			}

			event->acceptProposedAction();

		}

		void dragMoveEvent(QDragMoveEvent *event) override
		{
			int row = rowAt(event->pos().y());
			if (row == -1)
				event->accept();
			else
				event->ignore();
		}

		void dropEvent(QDropEvent *event) override
		{
			const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
			if (mimeData != nullptr)
			{
				auto inputField = mimeData->GetInputField();
				FrontEndFilter filter;
				filter.fields.push_back(inputField);
				AddRow(filter);
			}

		}
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
	//m_table->AddRow();
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
				QAction* removeFieldAction = menu->addAction(tr("Remove Field"));
				QObject::connect(removeFieldAction, &QAction::triggered, [=]()
				{
					wgt->RemoveSelected();
				});
			}
		}
		if (row >= 0)
		{
			QAction* addVisAction = menu->addAction(tr("Remove Filter"));
			QObject::connect(addVisAction, &QAction::triggered, [=]()
			{
				m_table->removeRow(row);
			});
		}


		//QTreeWidgetItem* item = m_table->itemAt(pos);
		QAction* addVisAction = menu->addAction(tr("Add Filter"));
		QObject::connect(addVisAction, &QAction::triggered, [=]() 
		{
			m_table->AddRow();
		}
		);

		menu->popup(m_table->viewport()->mapToGlobal(pos));
	});



}
void FilterSetupWidget::SetFilters(const SynGlyphX::MultiTableFrontEndFilters& filters) 
{
	m_table->Clear();
	if (filters.size() > 0)
	{
		for (const auto& filter : filters)
		{
			m_table->AddRow(filter);
		}
	}
	else
	{
		//m_table->AddRow();
	}
}

SynGlyphX::MultiTableFrontEndFilters FilterSetupWidget::GetFilters() const {

	MultiTableFrontEndFilters filters;
	for (int i = 0; i < m_table->rowCount(); i++)
	{
		FrontEndFilter filter = m_table->GetFilter(i);
		if (filter.fields.size() > 0)
			filters.push_back(filter);
	}
	return filters;
	
}

#include "FilterSetupWidget.moc"
