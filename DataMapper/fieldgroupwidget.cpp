#include "fieldgroupwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMessageBox>

FieldGroupWidget::FieldGroupWidget(DataTransformModel* dataTransformModel, QWidget *parent)
	: QWidget(parent),
	m_dataTransformModel(dataTransformModel)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QHBoxLayout* groupNameLayout = new QHBoxLayout(this);
	groupNameLayout->addWidget(new QLabel(tr("Group:")));
	m_groupsNameComboBox = new QComboBox(this);
	m_groupsNameComboBox->setEditable(false);
	for (auto& fieldGroup : m_dataTransformModel->GetDataMapping()->GetFieldGroupMap()) {

		m_groupsNameComboBox->addItem(QString::fromStdWString(fieldGroup.first));
	}
	
	QObject::connect(m_groupsNameComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &FieldGroupWidget::OnGroupChanged);
	groupNameLayout->addWidget(m_groupsNameComboBox, 1);

	m_saveButton = new QPushButton(tr("Save"), this);
	m_saveButton->setEnabled(false);
	QObject::connect(m_saveButton, &QPushButton::clicked, this, &FieldGroupWidget::OnSaveGroup);
	groupNameLayout->addWidget(m_saveButton);
	m_saveAsButton = new QPushButton(tr("Save As"), this);
	QObject::connect(m_saveAsButton, &QPushButton::clicked, this, &FieldGroupWidget::OnSaveAsGroup);
	groupNameLayout->addWidget(m_saveAsButton);
	m_revertButton = new QPushButton(tr("Revert"), this);
	m_revertButton->setEnabled(false);
	QObject::connect(m_revertButton, &QPushButton::clicked, this, &FieldGroupWidget::OnRevertGroup);
	groupNameLayout->addWidget(m_revertButton);

	mainLayout->addLayout(groupNameLayout);

	m_fieldTableView = new QTableView(this);
	m_fieldTableView->verticalHeader()->hide();
	m_fieldTableView->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
	//m_fieldTableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

	m_fieldTableHeaderView = new SynGlyphX::CheckBoxHeaderView(Qt::Horizontal, this);
	m_fieldTableView->setHorizontalHeader(m_fieldTableHeaderView);
	QObject::connect(m_fieldTableHeaderView, &SynGlyphX::CheckBoxHeaderView::CheckBoxClicked, this, &FieldGroupWidget::OnCheckBoxHeaderViewClicked);
	
	m_fieldGroupModel = new FieldGroupModel(this);
	
	m_fieldTableView->setModel(m_fieldGroupModel);
	m_fieldGroupModel->ResetTable(m_dataTransformModel);
	m_fieldTableView->resizeColumnsToContents();
	
	if (m_groupsNameComboBox->count() > 0) {

		m_groupsNameComboBox->setCurrentIndex(0);
	}
	QObject::connect(m_fieldGroupModel, &FieldGroupModel::dataChanged, this, &FieldGroupWidget::OnFieldGroupModelDataChanged);

	mainLayout->addWidget(m_fieldTableView);

	setLayout(mainLayout);
}

FieldGroupWidget::~FieldGroupWidget()
{

}

const QString& FieldGroupWidget::GetCurrentGroupName() const {

	return m_currentGroupName;
}

void FieldGroupWidget::SetCurrentGroupName(const QString& groupName) {

	m_groupsNameComboBox->setCurrentText(groupName);
}

void FieldGroupWidget::OnSaveGroup() {

	if (m_currentGroupName.isEmpty()) {

		OnSaveAsGroup();
	}
	else {

		m_dataTransformModel->UpdateFieldGroup(m_currentGroupName.toStdWString(), m_fieldGroupModel->GetCheckedItems());
		EnableSaveAndRevertButtons(false);
	}
}

void FieldGroupWidget::OnSaveAsGroup() {

	QString newGroupName = GetNewGroupName();
	if (!newGroupName.isEmpty()) {

		m_dataTransformModel->UpdateFieldGroup(newGroupName.toStdWString(), m_fieldGroupModel->GetCheckedItems());
		EnableSaveAndRevertButtons(false);
	}
}

void FieldGroupWidget::OnRevertGroup() {

	m_fieldGroupModel->SetCheckedItems(m_dataTransformModel->GetDataMapping()->GetFieldGroupMap().at(m_currentGroupName.toStdWString()));
	EnableSaveAndRevertButtons(false);
}

void FieldGroupWidget::OnGroupChanged(int index) {

	if (index == -1) {

		m_currentGroupName.clear();
		EnableSaveAndRevertButtons(false);
	}
	else if (m_currentGroupName != m_groupsNameComboBox->currentText()) {

		if (CheckIfGroupNeedsToBeSaved()) {

			m_currentGroupName = m_groupsNameComboBox->currentText();
			m_fieldGroupModel->SetCheckedItems(m_dataTransformModel->GetDataMapping()->GetFieldGroupMap().at(m_currentGroupName.toStdWString()));
			EnableSaveAndRevertButtons(false);
		}
		else {

			m_groupsNameComboBox->setCurrentText(m_currentGroupName);
		}
	}
}

void FieldGroupWidget::EnableSaveAndRevertButtons(bool enable) {

	m_saveButton->setEnabled(enable);
	m_revertButton->setEnabled(enable);
}

bool FieldGroupWidget::CheckIfGroupNeedsToBeSaved() {

	if (m_saveButton->isEnabled()) {

		QMessageBox::StandardButton button = QMessageBox::question(this, tr("Save Group"), m_currentGroupName + tr(" has changes which have not been saved.  Do you wish to save changes to this group?"),
			QMessageBox::StandardButton::Save | QMessageBox::StandardButton::Discard | QMessageBox::StandardButton::Cancel, QMessageBox::StandardButton::Save);
		if (button == QMessageBox::StandardButton::Cancel) {

			return false;
		}
		else if (button == QMessageBox::StandardButton::Save) {

			OnSaveGroup();
		}
	}

	return true;
}

QString FieldGroupWidget::GetNewGroupName() {

	QString newGroupName;
	bool isValid = false;
	do {

		bool okPressed = true;
		newGroupName = QInputDialog::getText(this, tr("Create New Group Name"), tr("New Group:"), QLineEdit::Normal, newGroupName, &okPressed);
		if (!okPressed) {

			newGroupName.clear();
			isValid = true;
		}
		else if (newGroupName.isEmpty()) {

			QMessageBox::warning(this, tr("Create New Group Error"), tr("New group name can not be empty."));
		}
		else if (m_dataTransformModel->GetFieldGroupMap().count(newGroupName.toStdWString()) > 0) {

			QMessageBox::warning(this, tr("Create New Group Error"), tr("New group name can not be the same as an existing group name."));
		}
		else {

			isValid = true;
		}
	} while (!isValid);

	return newGroupName;
}

void FieldGroupWidget::OnFieldGroupModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {

	m_fieldTableHeaderView->SetState(m_fieldGroupModel->AreFieldsChecked());
	EnableSaveAndRevertButtons(m_groupsNameComboBox->count() > 0);
}

void FieldGroupWidget::OnCheckBoxHeaderViewClicked(SynGlyphX::AllSomeNone state) {

	if (state == SynGlyphX::AllSomeNone::All) {

		SynGlyphX::FieldGroup fieldGroup;
		for (const auto& table : m_dataTransformModel->GetSourceDataManager().GetNumericFieldsByTable()) {

			for (const auto& field : table.second) {

				fieldGroup.emplace(SynGlyphX::InputField(table.first.GetDatasourceID(), table.first.GetTable(), field, SynGlyphX::InputField::Type::Real));
			}
		}
		m_fieldGroupModel->SetCheckedItems(fieldGroup);
	}
	else {

		m_fieldGroupModel->UncheckAllItems();
	}
}