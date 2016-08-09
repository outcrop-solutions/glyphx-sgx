#include "QueryBuilderDialog.h"
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include "DMGlobal.h"
#include "datatransformmodel.h"
#include "databaseserverdatasource.h"

using namespace SynGlyphX;

class GroupWidget : public QWidget
{
public:
	GroupWidget(QWidget *parent) : QWidget(parent)
	{
		QHBoxLayout* mainLayout = new QHBoxLayout(this);
		mainLayout->addWidget(new QLabel(tr("Group:")));
		m_cb = new QComboBox(this);
		m_cb->addItems(QStringList({ "and", "or" }));
		mainLayout->addWidget(m_cb);
		setLayout(mainLayout);
		setFixedSize(120, 40);
	}

	GroupWidget(const boost::property_tree::wptree& tree, QWidget *parent) : GroupWidget(parent)
	{
		m_cb->setCurrentText(QString::fromStdWString(tree.get<std::wstring>(L"<xmlattr>.logic")));
	}
	QComboBox* m_cb;
};

class StatementWidget : public QWidget
{
public:
	StatementWidget(const QStringList& nameList, QWidget *parent) : QWidget(parent)
	{
		QHBoxLayout* mainLayout = new QHBoxLayout(this);
		m_fieldCb = new QComboBox(this);
		m_fieldCb->addItems(nameList);
		mainLayout->addWidget(m_fieldCb);

		m_conditionCb = new QComboBox(this);
		m_conditionCb->addItems(QStringList({ "eq", "neq", "gt", "lt", "gte", "lte", "like" })); //add btw
		mainLayout->addWidget(m_conditionCb);
		
		m_lineEdit = new QLineEdit(this);
		mainLayout->addWidget(m_lineEdit);
		setLayout(mainLayout);
		setFixedSize(320, 40);
	}

	StatementWidget(const boost::property_tree::wptree& tree, const QStringList& nameList, QWidget *parent)
		: StatementWidget(nameList, parent)
	{	
		m_fieldCb->setCurrentText(QString::fromStdWString(tree.get<std::wstring>(L"<xmlattr>.columnname")));
		m_conditionCb->setCurrentText(QString::fromStdWString(tree.get<std::wstring>(L"<xmlattr>.operator")));
		m_lineEdit->setText(QString::fromStdWString(tree.get<std::wstring>(L"<xmlattr>.value")));
	}

	QComboBox* m_fieldCb;
	QComboBox* m_conditionCb;
	QLineEdit* m_lineEdit;
};

class QueryTreeWidget : public QTreeWidget
{
public:
	QueryTreeWidget(QWidget* parent) : QTreeWidget(parent)
	{
		setColumnCount(1);
		setHeaderLabel("Conditions:");
		auto model = DMGlobal::Services()->GetDataTransformModel();
		const DataTransformMapping::DatasourceMap& datasources = model->GetDataMapping()->GetDatasources();
		for (auto ds = datasources.begin(); ds != datasources.end(); ++ds)
		{
			if (Datasource::SourceType::DatabaseServer == ds->second->GetSourceType())
			{
				InputTable inputTable(datasources.begin()->first, datasources.begin()->second->GetTableNames().at(0));

				auto stats = model->GetTableStatsMap().at(inputTable);
				for (auto s : stats)
					m_names.push_back(s[0]);
			}

		}

		setMinimumSize(400, 400);
	}
	void SetItem(QTreeWidgetItem* item)
	{
		auto selection = selectedItems();
		if (selection.size() > 0)
		{
			if (selection[0]->data(0, Qt::UserRole).toString() == tr("Group"))
			{
				selection[0]->addChild(item);
				selection[0]->setExpanded(true);
			}
			else if (selection[0]->parent())
			{
				selection[0]->parent()->setExpanded(false); // hack to prevent widget overlap
				selection[0]->parent()->insertChild(selection[0]->parent()->indexOfChild(selection[0]) + 1, item);

				selection[0]->parent()->setExpanded(true);
			}
			else
				insertTopLevelItem(indexOfTopLevelItem(selection[0]), item);

		}
		else
		{
			addTopLevelItem(item);
		}

	}

	void OnAddCondition() 
	{
		QTreeWidgetItem *conditionItem = new QTreeWidgetItem();
		SetItem(conditionItem);
		setItemWidget(conditionItem, 0, new StatementWidget(m_names, this));
		resizeColumnToContents(0);
		adjustSize();
	}

	void OnAddGroup() 
	{
		QTreeWidgetItem *groupItem = new QTreeWidgetItem();
		groupItem->setData(0, Qt::UserRole, tr("Group"));
		SetItem(groupItem);
		setItemWidget(groupItem, 0, new GroupWidget(this));
		resizeColumnToContents(0);
		adjustSize();
	}

	void OnRemove() 
	{
		auto selection = selectedItems();
		if (selection.size() > 0)
		{
			//auto parent = selection[0]->parent();
			//parent->removeChild(selection[0]);
			delete selection[0];
		}
	}

	void AddTree(QTreeWidgetItem* parent, const boost::property_tree::wptree& tree)
	{
		for (const boost::property_tree::wptree::value_type& treeItem : tree)
		{
			QTreeWidgetItem *widgetItem = new QTreeWidgetItem();
			parent->addChild(widgetItem);
			if (treeItem.first == L"Statement")
			{
				setItemWidget(widgetItem, 0, new StatementWidget(treeItem.second, m_names, this));
				resizeColumnToContents(0);
				adjustSize();
			}
			else
			{
				setItemWidget(widgetItem, 0, new GroupWidget(treeItem.second, this));
				resizeColumnToContents(0);
				adjustSize();
				for (const boost::property_tree::wptree::value_type& treeItem : treeItem.second)
				{
					AddTree(widgetItem, treeItem.second);
				}
			}
		}
	}

	void SetFromTree(const boost::property_tree::wptree& queryTree)
	{
		AddTree(invisibleRootItem(), queryTree);

	}


	boost::property_tree::wptree& ExportToPropertyTree(QTreeWidgetItem* item, boost::property_tree::wptree& tree) 
	{
		if (item->data(0, Qt::UserRole).toString() == tr("Group"))
		{
			boost::property_tree::wptree& entry = tree.add(L"Group", L"");
			GroupWidget* sw = dynamic_cast<GroupWidget*>(item->treeWidget());
			entry.put(L"<xmlattr>.columnname", sw->m_cb->currentText().toStdWString());
			for (int i = 0; i < item->childCount(); i++)
			{
				auto child = item->child(i);
				ExportToPropertyTree(child, tree);
			}
		}
		else
		{
			boost::property_tree::wptree& entry = tree.add(L"Statement", L"");
			StatementWidget* sw = dynamic_cast<StatementWidget*>(item->treeWidget());

			entry.put(L"<xmlattr>.columnname", sw->m_fieldCb->currentText().toStdWString());
			entry.put(L"<xmlattr>.operator", sw->m_conditionCb->currentText().toStdWString());
			entry.put(L"<xmlattr>.value", sw->m_lineEdit->text().toStdWString());
		}
	}

	void GetPropertyTree(boost::property_tree::wptree&  tree)
	{
		tree.clear();
		//boost::property_tree::wptree& entry = tree->add(L"Query", L"");
		for (int i = 0; i < invisibleRootItem()->childCount(); i++)
		{
			auto item = invisibleRootItem()->child(i);
			ExportToPropertyTree(item, tree);
		}
	}

	QStringList m_names;
};


QueryBuilderDialog::QueryBuilderDialog(QWidget* parent) : QDialog(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	QGroupBox* queryGroupBox = new QGroupBox(tr("Query"), this);
	QVBoxLayout* queryLayout = new QVBoxLayout(queryGroupBox);
	mainLayout->addWidget(queryGroupBox);

	m_treeWidget = new QueryTreeWidget(this);
	queryLayout->addWidget(m_treeWidget);

	//queryLayout->removeWidget(m_treeWidget);

	QPushButton* addConditionButton = new QPushButton(tr("Add Condition"), this);
	QObject::connect(addConditionButton, &QPushButton::clicked, m_treeWidget, &QueryTreeWidget::OnAddCondition);
	queryLayout->addWidget(addConditionButton);
	
	QPushButton* addGroupButton = new QPushButton(tr("Add Group"), this);
	QObject::connect(addGroupButton, &QPushButton::clicked, m_treeWidget, &QueryTreeWidget::OnAddGroup);
	queryLayout->addWidget(addGroupButton);

	QPushButton* removeButton = new QPushButton(tr("Remove"), this);
	QObject::connect(removeButton, &QPushButton::clicked, m_treeWidget, &QueryTreeWidget::OnRemove);
	queryLayout->addWidget(removeButton);

	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	mainLayout->addWidget(dialogButtonBox);

	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &QueryBuilderDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &QueryBuilderDialog::reject);

	
	setLayout(mainLayout);
}

void QueryBuilderDialog::accept()
{


}