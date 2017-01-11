#include "ElasticListSetupTab.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtCore/QCoreApplication>

namespace SynGlyphX {

	ElasticListSetupTab::ElasticListSetupTab(std::wstring tableName, QWidget *parent)
		: QWidget(parent),
		m_tableName(tableName)
	{
		QHBoxLayout* layout = new QHBoxLayout();

		l_groupBox = new QGroupBox(tr("Available Fields:"));
		r_groupBox = new QGroupBox(tr("Elastic List:"));

		QVBoxLayout* l_layout = new QVBoxLayout();
		QVBoxLayout* r_layout = new QVBoxLayout();

		m_availableFields = new QListWidget();
		m_availableFields->setSelectionMode(QAbstractItemView::ExtendedSelection);
		l_layout->addWidget(m_availableFields);
		l_groupBox->setLayout(l_layout);

		QVBoxLayout* b_layout = new QVBoxLayout();

		QIcon leftIcon;
		QPixmap leftPm(":SGXGUI/Resources/left_arrow.png");
		leftIcon.addPixmap(leftPm, QIcon::Normal, QIcon::On);
		QPushButton* left = new QPushButton(this);
		left->setIcon(leftIcon);

		QIcon rightIcon;
		QPixmap rightPm(":SGXGUI/Resources/right_arrow.png");
		rightIcon.addPixmap(rightPm, QIcon::Normal, QIcon::On);
		QPushButton* right = new QPushButton(this);
		right->setIcon(rightIcon);

		b_layout->addWidget(left);
		b_layout->addWidget(right);

		m_elasticList = new QListWidget();
		m_elasticList->setSelectionMode(QAbstractItemView::ExtendedSelection);
		m_elasticList->setDragDropMode(QAbstractItemView::InternalMove);
		r_layout->addWidget(m_elasticList);
		r_groupBox->setLayout(r_layout);

		layout->addWidget(l_groupBox);
		layout->addLayout(b_layout);
		layout->addWidget(r_groupBox);

		setLayout(layout);

		QObject::connect(left, &QPushButton::clicked, this, &ElasticListSetupTab::MoveToAvailableFields);
		QObject::connect(right, &QPushButton::clicked, this, &ElasticListSetupTab::MoveToElasticList);

	}

	ElasticListSetupTab::~ElasticListSetupTab()
	{
	}

	void ElasticListSetupTab::PopulateAvailableFields(std::vector<std::wstring> available){

		for (auto field : available){
			QListWidgetItem * item = new QListWidgetItem(QString::fromStdWString(field));
			m_availableFields->addItem(item);
		}
	}

	void ElasticListSetupTab::PopulateElasticList(std::vector<std::wstring> elastic){

		for (auto field : elastic){
			QListWidgetItem * item = new QListWidgetItem(QString::fromStdWString(field));
			m_elasticList->addItem(item);
		}
	}

	void ElasticListSetupTab::MoveToAvailableFields(){

		QList<QListWidgetItem *> selected = m_elasticList->selectedItems();
		if (selected.isEmpty()) {
			return;
		}

		for (int i = 0; i < selected.size(); i++) {

			m_availableFields->insertItem(m_availableFields->count(), m_elasticList->takeItem(m_elasticList->row(selected.at(i))));
		}
	}

	void ElasticListSetupTab::MoveToElasticList(){

		QList<QListWidgetItem *> selected = m_availableFields->selectedItems();
		if (selected.isEmpty()) {
			return;
		}

		for (int i = 0; i < selected.size(); i++) {

			m_elasticList->insertItem(m_elasticList->count(), m_availableFields->takeItem(m_availableFields->row(selected.at(i))));
		}
	}

	std::wstring ElasticListSetupTab::GetTableName(){
		return m_tableName;
	}

	std::vector<std::wstring> ElasticListSetupTab::GetElasticList(){

		std::vector<std::wstring> orderedFields;
		for (int i = 0; i < m_elasticList->count(); i++){
			orderedFields.push_back(m_elasticList->item(i)->text().toStdWString());
		}
		return orderedFields;
	}

} //namespace SynGlyphX