#include "tablechoicewidget.h"
#include <QtWidgets/QStackedLayout>
#include "groupboxsinglewidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

TableChoiceWidget::TableChoiceWidget(QWidget *parent)
	: QWidget(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QStringList buttonNames;
	buttonNames << tr("Choose Main Tables") << tr("Create Joined Table");
	m_choiceTypeRadioButtonWidget = new SynGlyphX::RadioButtonGroupWidget(buttonNames, Qt::Horizontal, this);
	mainLayout->addWidget(m_choiceTypeRadioButtonWidget);

	QStackedLayout* stackedLayout = new QStackedLayout(this);
	stackedLayout->setContentsMargins(0, 0, 0, 0);

	m_baseTableListWidget = new QListWidget(this);
	m_baseTableListWidget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	m_baseTableListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	SynGlyphX::GroupBoxSingleWidget* multiTableChoiceGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Choose Main Table(s)"), m_baseTableListWidget, this);
	stackedLayout->addWidget(multiTableChoiceGroupBox);

	QWidget* joinedTableWidget = new QWidget(this);
	QVBoxLayout* joinedTableWidgetLayout = new QVBoxLayout(joinedTableWidget);
	joinedTableWidgetLayout->setContentsMargins(0, 0, 0, 0);

	QHBoxLayout* joinedTableComboBoxLayout = new QHBoxLayout(this);
	joinedTableComboBoxLayout->addWidget(new QLabel(tr("Main Table:")));
	m_mainTableComboBox = new QComboBox(this);
	joinedTableComboBoxLayout->addWidget(m_mainTableComboBox);
	joinedTableComboBoxLayout->addStretch(1);

	joinedTableWidgetLayout->addLayout(joinedTableComboBoxLayout);

	m_linkedTablesListWidget = new QListWidget(this);
	m_linkedTablesListWidget->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	m_linkedTablesListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	SynGlyphX::GroupBoxSingleWidget* linkedTableChoiceGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Choose Linked Table(s)"), m_linkedTablesListWidget, this);
	joinedTableWidgetLayout->addWidget(linkedTableChoiceGroupBox);

	joinedTableWidget->setLayout(joinedTableWidgetLayout);
	stackedLayout->addWidget(joinedTableWidget);

	mainLayout->addLayout(stackedLayout);

	setLayout(mainLayout);

	QObject::connect(m_choiceTypeRadioButtonWidget, &SynGlyphX::RadioButtonGroupWidget::ButtonClicked, stackedLayout, &QStackedLayout::setCurrentIndex);
	QObject::connect(m_mainTableComboBox, &QComboBox::currentTextChanged, this, &TableChoiceWidget::OnNewMainTableInJoinedModeSelected);
}

TableChoiceWidget::~TableChoiceWidget()
{

}

void TableChoiceWidget::SetTables(const QStringList& mainTables, Table2ForiegnKeyTablesMap foriegnKeyTablesMap) {

	if (mainTables.isEmpty()) {

		throw std::invalid_argument("There needs to be at least one table");
	}

	m_mainTables = mainTables;
	m_foriegnKeyTablesMap = foriegnKeyTablesMap;

	m_baseTableListWidget->clear();
	m_baseTableListWidget->addItems(mainTables);
	m_mainTableComboBox->clear();
	m_mainTableComboBox->addItems(mainTables);

	if (m_foriegnKeyTablesMap.empty()) {

		m_choiceTypeRadioButtonWidget->SetButtonEnabled(1, false);
		m_choiceTypeRadioButtonWidget->SetCheckedButton(0);
	}
	else {

		m_choiceTypeRadioButtonWidget->SetButtonEnabled(1, true);
		OnNewMainTableInJoinedModeSelected(mainTables[0]);
	}
}

QStringList TableChoiceWidget::GetChosenMainTables() const {

	if (IsInJoinedTableMode()) {

		return QStringList(m_mainTableComboBox->currentText());
	}
	else {

		return GetSelectedTables(m_baseTableListWidget);
	}
}

QStringList TableChoiceWidget::GetChosenForiegnTables() const {

	if (IsInJoinedTableMode()) {

		return GetSelectedTables(m_linkedTablesListWidget);
	}
	else {

		return QStringList();
	}
}

bool TableChoiceWidget::IsInJoinedTableMode() const {

	return (m_choiceTypeRadioButtonWidget->GetCheckedButton() == 1);
}

void TableChoiceWidget::OnNewMainTableInJoinedModeSelected(const QString& mainTable) {

	m_linkedTablesListWidget->clear();
	if (m_foriegnKeyTablesMap.contains(mainTable)) {

		m_linkedTablesListWidget->addItems(m_foriegnKeyTablesMap.value(mainTable));
	}
}

QStringList TableChoiceWidget::GetSelectedTables(const QListWidget* const listWidget) const {

	QStringList tables;

	for (const auto& selectedItem : listWidget->selectedItems()) {

		tables << selectedItem->text();
	}

	return tables;
}