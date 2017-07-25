#include "GroupSelectionDialog.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtCore/QCoreApplication>

GroupSelectionDialog::GroupSelectionDialog(QStringList groupNames, QWidget *parent)
	: QDialog(parent),
	m_groupNames(groupNames)
{
	QVBoxLayout* layout = new QVBoxLayout();

	l_groupBox = new QGroupBox("Available Groups:");

	QVBoxLayout* l_layout = new QVBoxLayout();

	m_availableGroups = new QListWidget();
	m_availableGroups->setSelectionMode(QAbstractItemView::SingleSelection);
	l_layout->addWidget(m_availableGroups);
	l_groupBox->setLayout(l_layout);

	layout->addWidget(l_groupBox);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, this);
	QPushButton* okButton = buttonBox->button(QDialogButtonBox::Ok);
	okButton->setDisabled(true);
	layout->addWidget(buttonBox);

	setLayout(layout);
	
	QObject::connect(m_availableGroups, &QListWidget::itemSelectionChanged, okButton, [=](){
		okButton->setDisabled(m_availableGroups->selectedItems().size() != 1);
	});
	QObject::connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);

	PopulateAvailableGroups();

}

GroupSelectionDialog::~GroupSelectionDialog()
{
}

void GroupSelectionDialog::PopulateAvailableGroups(){

	for (const auto& field : m_groupNames){
		QListWidgetItem * item = new QListWidgetItem(field);
		m_availableGroups->addItem(item);
	}
}

