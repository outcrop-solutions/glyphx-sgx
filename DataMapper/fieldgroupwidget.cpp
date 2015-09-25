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
	if (m_groupsNameComboBox->count() > 0) {

		m_currentGroupName = m_groupsNameComboBox->itemData(0).toString();
	}
	QObject::connect(m_groupsNameComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &FieldGroupWidget::OnGroupChanged);
	groupNameLayout->addWidget(m_groupsNameComboBox);
	mainLayout->addLayout(groupNameLayout);

	QHBoxLayout* saveRevertLayout = new QHBoxLayout(this);
	m_saveButton = new QPushButton(tr("Save"), this);
	m_saveButton->setEnabled(false);
	QObject::connect(m_saveButton, &QPushButton::clicked, this, &FieldGroupWidget::OnSaveGroup);
	saveRevertLayout->addWidget(m_saveButton);
	m_saveAsButton = new QPushButton(tr("Save"), this);
	QObject::connect(m_saveAsButton, &QPushButton::clicked, this, &FieldGroupWidget::OnSaveAsGroup);
	saveRevertLayout->addWidget(m_saveAsButton);
	m_revertButton = new QPushButton(tr("Revert"), this);
	m_revertButton->setEnabled(false);
	QObject::connect(m_revertButton, &QPushButton::clicked, this, &FieldGroupWidget::OnRevertGroup);
	saveRevertLayout->addWidget(m_revertButton);
	mainLayout->addLayout(saveRevertLayout);

	m_fieldTableView = new QTableView(this);
	m_fieldTableView->setEnabled(m_groupsNameComboBox->count() > 0);
	SynGlyphX::CheckBoxHeaderView* fieldTableHeaderView = new SynGlyphX::CheckBoxHeaderView(Qt::Horizontal, this);
	m_fieldTableView->setHorizontalHeader(fieldTableHeaderView);
	m_fieldGroupModel = new FieldGroupModel(this);
	m_fieldGroupModel->ResetTable(m_dataTransformModel);

	mainLayout->addWidget(m_fieldTableView);

	setLayout(mainLayout);
}

FieldGroupWidget::~FieldGroupWidget()
{

}

void FieldGroupWidget::OnSaveGroup() {

	if (m_currentGroupName.isEmpty()) {

		OnSaveAsGroup();
	}
	else {

		EnableButtons(false, false);
	}
}

void FieldGroupWidget::OnSaveAsGroup() {

	QString newGroupName = GetNewGroupName();
	if (!newGroupName.isEmpty()) {

		EnableButtons(false, false);
	}
}

void FieldGroupWidget::OnRevertGroup() {

	EnableButtons(false, false);
}

void FieldGroupWidget::OnGroupChanged(int index) {

	if (index == -1) {

		m_fieldTableView->setEnabled(false);
		m_currentGroupName.clear();
		EnableButtons(false, false);
	}
	else if (m_currentGroupName != m_groupsNameComboBox->currentText()) {

		if (CheckIfGroupNeedsToBeSaved()) {

			m_currentGroupName = m_groupsNameComboBox->currentText();
			EnableButtons(false, false);
			m_fieldTableView->setEnabled(true);
		}
		else {

			m_groupsNameComboBox->setCurrentText(m_currentGroupName);
		}
	}
}

void FieldGroupWidget::EnableButtons(bool enableSave, bool enableRevert) {

	m_saveButton->setEnabled(enableSave);
	m_revertButton->setEnabled(enableRevert);
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
		newGroupName = QInputDialog::getText(this, tr("Create New Group Name"), tr("New Group:"), QLineEdit::Normal, newGroupName);
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