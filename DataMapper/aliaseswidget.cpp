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

class AliasLineEdit : public QLineEdit
{
public:
	AliasLineEdit(SynGlyphX::DataTransformModel* dataTransformModel, QTableWidgetItem* nameItem, QWidget *parent = 0);
	virtual ~AliasLineEdit() {}
	const SynGlyphX::InputField& GetInputField() const { return m_inputField; }
	void SetInputField(const SynGlyphX::InputField& inputField);
protected:
	virtual void dragEnterEvent(QDragEnterEvent* event);
	virtual void dropEvent(QDropEvent* event);
private:
	SynGlyphX::InputField m_inputField;
	QTableWidgetItem* m_nameItem;
	SynGlyphX::DataTransformModel* m_dataTransformModel;
};

AliasLineEdit::AliasLineEdit(SynGlyphX::DataTransformModel* dataTransformModel, QTableWidgetItem* nameItem, QWidget *parent) :
QLineEdit(parent),
m_nameItem(nameItem),
m_dataTransformModel(dataTransformModel)
{
	setContextMenuPolicy(Qt::NoContextMenu);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	setReadOnly(true);
}

void AliasLineEdit::SetInputField(const SynGlyphX::InputField& inputField) {
	m_inputField = inputField;
	if (m_inputField.IsValid()) {

		SynGlyphX::Datasource::ConstSharedPtr datasource = m_dataTransformModel->GetDataMapping()->GetDatasources().at(inputField.GetDatasourceID());

		QString text = QString::fromStdWString(datasource->GetFormattedName());
		if (datasource->CanDatasourceHaveMultipleTables()) {

			text += ":" + QString::fromStdWString(inputField.GetTable());
		}
		text += ":" + QString::fromStdWString(inputField.GetField());
		setText(text);
		QString name = m_nameItem->text();
		m_dataTransformModel->GetInputFieldManager()->SetInputField(name.toStdWString(), m_inputField);
		m_nameItem->setFlags(m_nameItem->flags() & (~Qt::ItemIsEditable) | Qt::ItemIsDragEnabled);
	}
	else {

		clear();
	}
}

void AliasLineEdit::dragEnterEvent(QDragEnterEvent *event) {

	const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
	if (mimeData == nullptr) {

		return;
	}

	event->acceptProposedAction();
}

void AliasLineEdit::dropEvent(QDropEvent* event) {
	const InputFieldMimeData* mimeData = qobject_cast<const InputFieldMimeData*>(event->mimeData());
	if (mimeData != nullptr) {
		SetInputField(mimeData->GetInputField());
	}
}

class AliasTableWidget : public QTableWidget
{
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

AliasesWidget::AliasesWidget(SynGlyphX::DataTransformModel* model, QWidget *parent)
	: QWidget(parent),
	m_model(model)
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
			m_model->GetInputFieldManager()->ClearInputFieldBindings(name.toStdWString());
			DMGlobal::Services()->GetGlyphRolesTableModel()->Refresh();
			m_aliasesTableWidget->removeRow(selection.at(0)->row());

			//Remove associated bindings
		}
	}
}

void AliasesWidget::Clear() {

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
	SynGlyphX::Application::restoreOverrideCursor();
}

void AliasesWidget::Refresh() {
	QObject::disconnect(m_aliasesTableWidget, &QTableWidget::cellChanged, this, &AliasesWidget::OnCellChanged);
	auto fieldMap = m_model->GetInputFieldManager()->GetFieldMap();
	for (const auto& field : m_model->GetInputFieldManager()->GetFieldMap())
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

			AliasLineEdit* fieldInputWidget = new AliasLineEdit(m_model, nameItem, this);
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
		m_aliasesTableWidget->insertRow(row);
		//QWidget* nameInputWidget = new QLineEdit(this);
		QTableWidgetItem* nameItem = new QTableWidgetItem(GenerateUniqueName("A" + QString::number(row)));
		nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsDragEnabled);
		//nameInputWidget->setContentsMargins(0, 0, 0, 0);
		//m_aliasesTableWidget->setCellWidget(row, 0, nameInputWidget);
		m_aliasesTableWidget->setItem(row, 0, nameItem);

		QWidget* fieldInputWidget = new AliasLineEdit(m_model, nameItem, this);

		fieldInputWidget->setContentsMargins(0, 0, 0, 0);
		m_aliasesTableWidget->setCellWidget(row, 1, fieldInputWidget);
	}

}

void AliasesWidget::dragEnterEvent(QDragEnterEvent *event)
{
	event->acceptProposedAction();
}
