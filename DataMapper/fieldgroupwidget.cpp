#include "fieldgroupwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTableView>

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
	QObject::connect(m_groupsNameComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), this, &FieldGroupWidget::OnGroupChanged);
	groupNameLayout->addWidget(m_groupsNameComboBox);
	mainLayout->addLayout(groupNameLayout);

	QHBoxLayout* saveRevertLayout = new QHBoxLayout(this);
	QPushButton* newButton = new QPushButton(tr("New"), this);
	QObject::connect(newButton, &QPushButton::clicked, this, &FieldGroupWidget::OnNewGroup);
	saveRevertLayout->addWidget(newButton);
	m_copyButton = new QPushButton(tr("Copy"), this);
	m_copyButton->setEnabled(m_groupsNameComboBox->count() > 0);
	QObject::connect(m_copyButton, &QPushButton::clicked, this, &FieldGroupWidget::OnCopyGroup);
	saveRevertLayout->addWidget(m_copyButton);
	m_saveButton = new QPushButton(tr("Save"), this);
	m_saveButton->setEnabled(false);
	QObject::connect(m_saveButton, &QPushButton::clicked, this, &FieldGroupWidget::OnSaveGroup);
	saveRevertLayout->addWidget(m_saveButton);
	m_revertButton = new QPushButton(tr("Revert"), this);
	m_revertButton->setEnabled(false);
	QObject::connect(m_revertButton, &QPushButton::clicked, this, &FieldGroupWidget::OnRevertGroup);
	saveRevertLayout->addWidget(m_revertButton);
	mainLayout->addLayout(saveRevertLayout);

	QTableView* fieldTableView = new QTableView(this);
	m_fieldGroupModel = new FieldGroupModel(this);
	m_fieldGroupModel->ResetTable(m_dataTransformModel);

	setLayout(mainLayout);
}

FieldGroupWidget::~FieldGroupWidget()
{

}

void FieldGroupWidget::OnNewGroup() {


}

void FieldGroupWidget::OnCopyGroup() {


}

void FieldGroupWidget::OnSaveGroup() {


}

void FieldGroupWidget::OnRevertGroup() {


}

void FieldGroupWidget::OnGroupChanged(const QString& newGroupName) {

	if (m_currentGroupName != newGroupName) {


	}
}