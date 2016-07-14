#include "aliaseswidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtCore/QDebug>
#include "application.h"
#include "singlewidgetdialog.h"
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QAction>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>

AliasesWidget::AliasesWidget(QWidget *parent)
	: QWidget(parent)
	//m_model(model)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);

	QFrame* border = new QFrame(this);
	border->setFrameStyle(QFrame::Shape::HLine | QFrame::Shadow::Sunken);
	mainLayout->addWidget(border);

	QStringList headerLabels;
	headerLabels << tr("   Alias") << tr("Bound Field");

	m_aliasesTableWidget = new QTableWidget(0, 2, this);

	QTableWidgetItem* item = new QTableWidgetItem();
	item->setIcon(*(new QIcon(":SGXGUI/Resources/plus.png")));
	m_aliasesTableWidget->setHorizontalHeaderItem(0, item);
	m_aliasesTableWidget->setHorizontalHeaderLabels(headerLabels);
	m_aliasesTableWidget->setHorizontalHeader(SelectableHeaderView());

	m_aliasesTableWidget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	m_aliasesTableWidget->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
	m_aliasesTableWidget->setDragDropMode(QAbstractItemView::DragDrop);
	m_aliasesTableWidget->setFrameShape(QFrame::Shape::NoFrame);
	m_aliasesTableWidget->horizontalHeader()->setStretchLastSection(true);
	m_aliasesTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	m_aliasesTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	m_aliasesTableWidget->verticalHeader()->hide();
	m_aliasesTableWidget->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
	
	m_removeSelectedContextMenuAction = new QAction(tr("Remove Alias"), m_aliasesTableWidget);
	QObject::connect(m_removeSelectedContextMenuAction, &QAction::triggered, this, &AliasesWidget::removeAlias);
	m_aliasesTableWidget->addAction(m_removeSelectedContextMenuAction);

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
			m_aliasesTableWidget->removeRow(selection.at(0)->row());
			//Remove associated bindings
		}
	}
}

void AliasesWidget::Clear() {

	SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
	//m_aliasManager->ClearAllAliases();
	SynGlyphX::Application::restoreOverrideCursor();
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

void AliasesWidget::sectionClicked(int index)
{
	if (index == 0){

		int rows = m_aliasesTableWidget->rowCount();
		m_aliasesTableWidget->insertRow(rows);

		//QLineEdit* nameLineEdit = new QLineEdit(QString::fromStdWString(datasource.GetFormattedName()), this);
		//nameLineEdit->setMinimumWidth(384);
		//nameLineEdit->setReadOnly(true);
		//nameLayout->addWidget(nameLineEdit);

		m_aliasesTableWidget->setItem(rows, 0, new QTableWidgetItem("A"+QString::number(rows)));
		m_aliasesTableWidget->item(rows, 0)->setFlags(Qt::ItemIsDragEnabled | m_aliasesTableWidget->item(rows, 0)->flags());

		//TEST BEGIN
		/*
		BindingLineEdit* d_LineEdit = new BindingLineEdit(m_model);
		d_LineEdit->layout()->setContentsMargins(QMargins(8, 5, 8, 5));

		QDataWidgetMapper* d_Mapper = new QDataWidgetMapper(this);
		d_Mapper->setModel(m_model);
		d_Mapper->addMapping(d_LineEdit, GlyphRolesTableModel::s_mappedFieldColumn);

		m_dataWidgetMappersAndRows[d_Mapper] = rows;

		QObject::connect(d_LineEdit, &BindingLineEdit::ValueChangedByUser, d_Mapper, &QDataWidgetMapper::submit);
		*/
		//TEST END

		m_aliasesTableWidget->setItem(rows, 1, new QTableWidgetItem(" "));
		//m_aliasesTableWidget->item(rows, 1)->setFlags(Qt::ItemIsDragEnabled | m_aliasesTableWidget->item(rows, 1)->flags());
	}
}
