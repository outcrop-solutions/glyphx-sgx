#include "aliaseswidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtCore/QDebug>
#include <QtWidgets/QLineEdit>
#include "application.h"
#include "singlewidgetdialog.h"
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QAction>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyledItemDelegate>
#include <QtGui/QDragEnterEvent>
#include "datatransformmodel.h"
#include "inputfieldmimedata.h"
#include "DMGlobal.h"
#include "glyphrolestablemodel.h"
#include <QtWidgets/QUndoStack>
#include <QtWidgets/QUndoCommand>
#include <QtCore/QPointer>


class AliasTableWidget : public QTableWidget
{
//Q_OBJECT
public:
	explicit AliasTableWidget(QWidget *parent = Q_NULLPTR) : QTableWidget(parent) {}
	AliasTableWidget(int rows, int columns, QWidget *parent = Q_NULLPTR) : QTableWidget(rows, columns, parent) {}
private:
	virtual QMimeData*	mimeData(const QList<QTableWidgetItem *> items) const override
	{
		QMimeData* mimeData = new QMimeData();
		if (items.size() > 0 && items[0])
		{
			mimeData->setText(items[0]->text());
		}

		return mimeData;
	}
};


class AliasLineEdit : public QLineEdit
{
public:
	AliasLineEdit(QTableWidgetItem* nameItem, int row, AliasTableWidget* parent);
	virtual ~AliasLineEdit() {}
	const SynGlyphX::InputField& GetInputField() const { return m_inputField; }
	void SetInputField(const SynGlyphX::InputField& inputField);
	virtual void dragEnterEvent(QDragEnterEvent* event);
	virtual void dropEvent(QDropEvent* event);
	SynGlyphX::InputField m_inputField;
	QTableWidgetItem* m_nameItem;
	AliasTableWidget* m_tw;
	int m_row;
};

class ChangeAliasFieldCommand : public QUndoCommand 
{
	// we cannot store pointer to the widget, because undoing add/remove alias will recreate the widgets
public:
	ChangeAliasFieldCommand(AliasTableWidget* tw, int row, const SynGlyphX::InputField& inputField) :
		m_tw(tw),
		m_row(row),
		m_oldInputField(dynamic_cast<AliasLineEdit*>(tw->cellWidget(row, 1))->m_inputField),
		m_newInputField(inputField)
	{
		DMGlobal::Services()->SetModified();
	}
	void undo() override
	{
		DMGlobal::Services()->SetModified();
		auto ale = dynamic_cast<AliasLineEdit*>(m_tw->cellWidget(m_row, 1));
		if (ale) 
		{
			ale->m_inputField = m_oldInputField;
			ale->SetInputField(ale->m_inputField);
		}
	}
	void redo() override
	{
		DMGlobal::Services()->SetModified();
		auto ale = dynamic_cast<AliasLineEdit*>(m_tw->cellWidget(m_row, 1));
		if (ale) 
		{
			ale->m_inputField = m_newInputField;
			ale->SetInputField(ale->m_inputField);
		}
	}
	SynGlyphX::InputField m_newInputField;
	SynGlyphX::InputField m_oldInputField;
	AliasTableWidget* m_tw;
	int m_row;
};

AliasLineEdit::AliasLineEdit(QTableWidgetItem* nameItem, int row, AliasTableWidget* parent) :
QLineEdit(parent),
m_nameItem(nameItem),
m_row(row),
m_tw(parent)
{
	setContextMenuPolicy(Qt::NoContextMenu);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	setReadOnly(true);
}

void AliasLineEdit::SetInputField(const SynGlyphX::InputField& inputField) {
	m_inputField = inputField;
	if (m_inputField.IsValid()) {

		SynGlyphX::Datasource::ConstSharedPtr datasource = 
			DMGlobal::Services()->GetDataTransformModel()->GetDataMapping()->GetDatasources().at(inputField.GetDatasourceID());

		QString text = QString::fromStdWString(datasource->GetFormattedName());
		if (datasource->CanDatasourceHaveMultipleTables()) {

			text += ":" + QString::fromStdWString(inputField.GetTable());
		}
		text += ":" + QString::fromStdWString(inputField.GetField());
		setText(text);
		QString name = m_nameItem->text();
		DMGlobal::Services()->GetDataTransformModel()->GetInputFieldManager()->SetInputField(name.toStdWString(), m_inputField);
		m_nameItem->setFlags(m_nameItem->flags() & (~Qt::ItemIsEditable) | Qt::ItemIsDragEnabled);
	}
	else {
		m_nameItem->setFlags(m_nameItem->flags() & ~Qt::ItemIsDragEnabled | Qt::ItemIsEditable);
		clear();
	}
}

void AliasLineEdit::dragEnterEvent(QDragEnterEvent *event) {

	const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
	if (mimeData == nullptr) {
		return;
	}
	else if (m_inputField.IsValid() && (mimeData->GetInputField().IsNumeric() != m_inputField.IsNumeric()))
		return;

	event->acceptProposedAction();
}

void AliasLineEdit::dropEvent(QDropEvent* event) {
	const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
	if (mimeData != nullptr) {
		auto command = new ChangeAliasFieldCommand(m_tw, m_row, mimeData->GetInputField());
		command->setText(tr("Change Alias Field"));
		DMGlobal::Services()->GetUndoStack()->push(command);
	}
}


class AddAliasCommand : public QUndoCommand
{
public:
	AddAliasCommand(AliasTableWidget* tw,  QString name) :
		m_firstCall(true),
		m_aliasesTableWidget(tw),
		m_name(name)
	{
		setText(QObject::tr("Add Alias"));
		m_row = m_aliasesTableWidget->rowCount();
	}

	void undo() override
	{
		DMGlobal::Services()->SetModified();
		m_aliasesTableWidget->removeRow(m_row);
	}

	void redo() override
	{
		DMGlobal::Services()->SetModified();
		QTableWidgetItem* nameItem = new QTableWidgetItem(m_name);
		nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsDragEnabled);
		auto ale = new AliasLineEdit(nameItem, m_row, m_aliasesTableWidget);
		ale->setContentsMargins(0, 0, 0, 0);
		m_aliasesTableWidget->insertRow(m_row);
		m_aliasesTableWidget->setItem(m_row, 0, ale->m_nameItem);
		m_aliasesTableWidget->setCellWidget(m_row, 1, ale);
	}
	bool m_firstCall;
	AliasTableWidget* m_aliasesTableWidget;
	QString m_name;
	int m_row;
};

class RemoveAliasCommand : public QUndoCommand
{
public:
	RemoveAliasCommand(AliasTableWidget* tw, int row) :
		m_firstCall(true),
		m_aliasesTableWidget(tw),
		m_row(row)
	{
		setText(QObject::tr("Remove Alias"));
		auto ale = dynamic_cast<AliasLineEdit*>(tw->cellWidget(row, 1));
		m_inputField = ale->m_inputField;
		m_name = ale->m_nameItem->text();
	}

	void undo() override
	{
		DMGlobal::Services()->SetModified();
		QTableWidgetItem* nameItem = new QTableWidgetItem(m_name);
		nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsDragEnabled);
		auto ale = new AliasLineEdit(nameItem, m_row, m_aliasesTableWidget);
		ale->SetInputField(m_inputField);
		ale->setContentsMargins(0, 0, 0, 0);
		m_aliasesTableWidget->insertRow(m_row);
		m_aliasesTableWidget->setItem(m_row, 0, ale->m_nameItem);
		m_aliasesTableWidget->setCellWidget(m_row, 1, ale);

	}

	void redo() override
	{
		DMGlobal::Services()->SetModified();
		m_aliasesTableWidget->removeRow(m_row);
	}

	bool m_firstCall;
	AliasTableWidget* m_aliasesTableWidget;
	int m_row;
	SynGlyphX::InputField m_inputField;
	QString m_name;
};


class AliasItemDelegate : public QStyledItemDelegate
{
public:
	AliasItemDelegate(QObject* parent) : QStyledItemDelegate(parent) {}
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override
	{
		if (index.column() == 0)
		{
			QLineEdit* editor = new QLineEdit(parent);
			editor->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9_]+"), editor));
			return editor;
		}
		else
		{
			return QStyledItemDelegate::createEditor(parent, option, index);
		}
	}

};

AliasesWidget::AliasesWidget(QWidget *parent)
	: QWidget(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);

	QFrame* border = new QFrame(this);
	border->setFrameStyle(QFrame::Shape::HLine | QFrame::Shadow::Sunken);
	mainLayout->addWidget(border);

	QStringList headerLabels;
	headerLabels << tr("   Alias") << tr("Bound Field");

	m_aliasesTableWidget = new AliasTableWidget(0, 2, this);

	QTableWidgetItem* item = new QTableWidgetItem();
	item->setIcon(*(new QIcon(":SGXGUI/Resources/plus.png")));
	m_aliasesTableWidget->setHorizontalHeaderItem(0, item);
	m_aliasesTableWidget->setHorizontalHeaderLabels(headerLabels);
	m_aliasesTableWidget->setHorizontalHeader(SelectableHeaderView());

	m_aliasesTableWidget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	m_aliasesTableWidget->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
	m_aliasesTableWidget->setDragDropMode(QAbstractItemView::DragOnly);
	m_aliasesTableWidget->setFrameShape(QFrame::Shape::NoFrame);
	m_aliasesTableWidget->horizontalHeader()->setStretchLastSection(true);
	m_aliasesTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	m_aliasesTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	m_aliasesTableWidget->verticalHeader()->hide();
	m_aliasesTableWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
	QObject::connect(m_aliasesTableWidget, &QTableWidget::cellChanged, this, &AliasesWidget::OnCellChanged);


	m_removeSelectedContextMenuAction = new QAction(tr("Remove Alias"), m_aliasesTableWidget);
	QObject::connect(m_removeSelectedContextMenuAction, &QAction::triggered, this, &AliasesWidget::removeAlias);
	m_aliasesTableWidget->addAction(m_removeSelectedContextMenuAction);

	m_aliasesTableWidget->setItemDelegate(new AliasItemDelegate(m_aliasesTableWidget));
	//m_aliasesTableWidget->addAction(SynGlyphX::SharedActionList::CreateSeparator(this));

	mainLayout->addWidget(m_aliasesTableWidget, 1);

	setLayout(mainLayout);

	QObject::connect(m_aliasesTableWidget->selectionModel(), &QItemSelectionModel::selectionChanged, this, &AliasesWidget::Clear);

}


AliasesWidget::~AliasesWidget()
{
}

QSize AliasesWidget::sizeHint() const {

	return QSize(256, 256);
}

void AliasesWidget::removeAlias() {

	QList<QTableWidgetItem *> selection = m_aliasesTableWidget->selectedItems();
	if (!selection.isEmpty()) {

		QWidget *page = new QWidget;
		QVBoxLayout* mainLayout = new QVBoxLayout(page);
		page->setLayout(mainLayout);

		QHBoxLayout* nameLayout = new QHBoxLayout(page);

		QLabel* nameLabel = new QLabel(tr("Remove alias ") + selection.at(0)->text() + " and all of it's associated bindings?", page);
		nameLayout->addWidget(nameLabel);

		mainLayout->addLayout(nameLayout);

		SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::StandardButton::Yes | QDialogButtonBox::StandardButton::No, page, this);
		dialog.setWindowTitle(tr("Remove Alias"));

		if (dialog.exec() == QDialog::Accepted) {
			QString name = selection.at(0)->text();
			auto undoStack = DMGlobal::Services()->GetUndoStack();
			undoStack->beginMacro(tr("Remove Alias"));
			DMGlobal::Services()->BeginTransaction("Remove Alias Bindigns", SynGlyphX::TransactionType::ChangeTree);
			DMGlobal::Services()->GetDataTransformModel()->GetInputFieldManager()->RemoveInputFieldAndBindings(name.toStdWString());
			DMGlobal::Services()->EndTransaction();
			DMGlobal::Services()->GetGlyphRolesTableModel()->Refresh();
			auto command = new RemoveAliasCommand(m_aliasesTableWidget, selection.at(0)->row());
			undoStack->push(command);
			undoStack->endMacro();
		}
	}
}

void AliasesWidget::Clear() {

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
	SynGlyphX::Application::restoreOverrideCursor();
}

void AliasesWidget::Refresh() {
	while (m_aliasesTableWidget->rowCount() > 0)
		m_aliasesTableWidget->removeRow(0);
	QObject::disconnect(m_aliasesTableWidget, &QTableWidget::cellChanged, this, &AliasesWidget::OnCellChanged);
	auto fieldMap = DMGlobal::Services()->GetDataTransformModel()->GetInputFieldManager()->GetFieldMap();
	for (const auto& field : fieldMap)
	{ 
		QString name = QString::fromStdWString(field.first);
		if (name[0] != '~')
		{
			int row = m_aliasesTableWidget->rowCount();
			m_aliasesTableWidget->insertRow(row);
			//QWidget* nameInputWidget = new QLineEdit(this);
			QTableWidgetItem* nameItem = new QTableWidgetItem(name);
			//nameInputWidget->setContentsMargins(0, 0, 0, 0);
			//m_aliasesTableWidget->setCellWidget(row, 0, nameInputWidget);
			m_aliasesTableWidget->setItem(row, 0, nameItem);

			AliasLineEdit* fieldInputWidget = new AliasLineEdit(nameItem, row, m_aliasesTableWidget);
			fieldInputWidget->SetInputField(field.second);
			fieldInputWidget->setContentsMargins(0, 0, 0, 0);
			m_aliasesTableWidget->setCellWidget(row, 1, fieldInputWidget);
		}

	}
	QObject::connect(m_aliasesTableWidget, &QTableWidget::cellChanged, this, &AliasesWidget::OnCellChanged);
}

QString AliasesWidget::customStyleSheet()
{
	QString style;

	return style;
}

QHeaderView* AliasesWidget::SelectableHeaderView()
{
	m_headers = new QHeaderView(Qt::Horizontal);
	m_headers->setSectionsClickable(true);
	QObject::connect(m_headers, &QHeaderView::sectionClicked, this, &AliasesWidget::sectionClicked);

	return m_headers;
}

void AliasesWidget::OnCellChanged(int row, int column)
{
	QObject::disconnect(m_aliasesTableWidget, &QTableWidget::cellChanged, this, &AliasesWidget::OnCellChanged);
	if (column == 0)
	{
		QString n = m_aliasesTableWidget->item(row, column)->text();
		auto list = m_aliasesTableWidget->findItems(n, Qt::MatchExactly); // will always find one
		if (list.size() > 1)
		{
			QString u = GenerateUniqueName(n);
			m_aliasesTableWidget->item(row, column)->setText(u);
		}
	}
	QObject::connect(m_aliasesTableWidget, &QTableWidget::cellChanged, this, &AliasesWidget::OnCellChanged);
}

QString AliasesWidget::GenerateUniqueName(QString hint)
{
	//auto ifm = m_model->GetInputFieldManager();
	int n = 1;
	while (!m_aliasesTableWidget->findItems(hint, Qt::MatchExactly).isEmpty())
	{
		hint += QString::number(n++);
	}
	return hint;

}
void AliasesWidget::sectionClicked(int index)
{
	if (index == 0){

		int row = m_aliasesTableWidget->rowCount();

		auto command = new AddAliasCommand(m_aliasesTableWidget, GenerateUniqueName("A" + QString::number(row)));
		DMGlobal::Services()->GetUndoStack()->push(command);
	}

}

void AliasesWidget::dragEnterEvent(QDragEnterEvent *event)
{
	event->acceptProposedAction();
}

//#include "aliaseswidget.moc"