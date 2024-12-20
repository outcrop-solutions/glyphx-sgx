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
	Q_OBJECT
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
	Q_OBJECT
public:
	StatementWidget(const QStringList& nameList, QWidget *parent) : QWidget(parent)
	{
		QHBoxLayout* mainLayout = new QHBoxLayout(this);
		m_fieldCb = new QComboBox(this);
		m_fieldCb->addItems(nameList);
		mainLayout->addWidget(m_fieldCb);
		m_fieldCb->setFixedWidth(120);

		m_conditionCb = new QComboBox(this);
		m_conditionCb->addItems(QStringList({ "eq", "neq", "gt", "lt", "gte", "lte", "btw", "like" }));
		m_conditionCb->setFixedWidth(50);
		mainLayout->addWidget(m_conditionCb);

		m_lineEdit = new QLineEdit(this);
		m_lineEdit->setFixedWidth(60);
		m_lineEdit2 = new QLineEdit(this);
		m_lineEdit2->setFixedWidth(60);
		mainLayout->addWidget(m_lineEdit);
		mainLayout->addWidget(m_lineEdit2);
		m_lineEdit2->hide();
		setLayout(mainLayout);
		setFixedSize(360, 40);

		connect(m_conditionCb, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged),
			[=](const QString &text)
		{
			if (text == "btw")
				m_lineEdit2->show();
			else
				m_lineEdit2->hide();

		});

	}

	StatementWidget(const boost::property_tree::wptree& tree, const QStringList& nameList, QWidget *parent)
		: StatementWidget(nameList, parent)
	{	
		m_fieldCb->setCurrentText(QString::fromStdWString(tree.get<std::wstring>(L"<xmlattr>.columnname")));
		m_conditionCb->setCurrentText(QString::fromStdWString(tree.get<std::wstring>(L"<xmlattr>.operator")));
		if (m_conditionCb->currentText() == "btw")
		{
			m_lineEdit->setText(QString::fromStdWString(tree.get<std::wstring>(L"<xmlattr>.min")));
			m_lineEdit2->setText(QString::fromStdWString(tree.get<std::wstring>(L"<xmlattr>.max")));
		}
		else
		{
			m_lineEdit->setText(QString::fromStdWString(tree.get<std::wstring>(L"<xmlattr>.value")));
		}
		
	}

	QComboBox* m_fieldCb;
	QComboBox* m_conditionCb;
	QLineEdit* m_lineEdit;
	QLineEdit* m_lineEdit2;
};

class QueryTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	QueryTreeWidget(const QStringList& names, QWidget* parent) : QTreeWidget(parent),
		m_names(names)
	{
		setColumnCount(1);
		setHeaderLabel("Conditions:");


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
				insertTopLevelItem(indexOfTopLevelItem(selection[0]) + 1, item);

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

	void AddStatement(QTreeWidgetItem* parent, const boost::property_tree::wptree& tree)
	{
		QTreeWidgetItem *widgetItem = new QTreeWidgetItem();
		parent->addChild(widgetItem);
		setItemWidget(widgetItem, 0, new StatementWidget(tree, m_names, this));
		resizeColumnToContents(0);
		adjustSize();
	}

	void AddGroup(QTreeWidgetItem* parent, const boost::property_tree::wptree& tree)
	{
		QTreeWidgetItem *widgetItem = new QTreeWidgetItem();
		widgetItem->setData(0, Qt::UserRole, tr("Group"));
		parent->addChild(widgetItem);
		setItemWidget(widgetItem, 0, new GroupWidget(tree, this));
		resizeColumnToContents(0);
		adjustSize();
		for (const boost::property_tree::wptree::value_type& childItem : tree)
		{
			if (childItem.first == L"Group")
				AddGroup(widgetItem, childItem.second);
			else if (childItem.first == L"Statement")
				AddStatement(widgetItem, childItem.second);
		}
		widgetItem->setExpanded(true);
	}

	void AddTree(QTreeWidgetItem* parent, const boost::property_tree::wptree& tree)
	{
		for (const boost::property_tree::wptree::value_type& treeItem : tree)
		{
			if (treeItem.first == L"Statement")
			{
				AddStatement(parent, treeItem.second);
			}
			else if(treeItem.first == L"Group")
			{
				AddGroup(parent, treeItem.second);
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
			GroupWidget* sw = qobject_cast<GroupWidget*>(itemWidget(item, 0));
			entry.put(L"<xmlattr>.logic", sw->m_cb->currentText().toStdWString());
			for (int i = 0; i < item->childCount(); i++)
			{
				auto child = item->child(i);
				ExportToPropertyTree(child, entry);
			}
		}
		else
		{
			StatementWidget* sw = qobject_cast<StatementWidget*>(itemWidget(item, 0));
			if (sw)
			{
				boost::property_tree::wptree& entry = tree.add(L"Statement", L"");
				entry.put(L"<xmlattr>.columnname", sw->m_fieldCb->currentText().toStdWString());
				entry.put(L"<xmlattr>.operator", sw->m_conditionCb->currentText().toStdWString());
				if (sw->m_conditionCb->currentText() == "btw")
				{
					entry.put(L"<xmlattr>.min", sw->m_lineEdit->text().toStdWString());
					entry.put(L"<xmlattr>.max", sw->m_lineEdit2->text().toStdWString());
				}
				else
				{
					entry.put(L"<xmlattr>.value", sw->m_lineEdit->text().toStdWString());
				}
				
			}

		}
		return tree;
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
	m_tableComboBox = new QComboBox(this);
	mainLayout->addWidget(m_tableComboBox, 0, Qt::AlignHCenter);
	QGroupBox* queryGroupBox = new QGroupBox(tr("Query"), this);
	QVBoxLayout* queryLayout = new QVBoxLayout(queryGroupBox);
	mainLayout->addWidget(queryGroupBox);


	
	auto model = DMGlobal::Services()->GetDataTransformModel();
	const DataTransformMapping::DatasourceMap& datasources = model->GetDataMapping()->GetDatasources();
	if (datasources.size() == 0)
		return;

	for (auto ds = datasources.begin(); ds != datasources.end(); ++ds)
	{
		if (Datasource::SourceType::DatabaseServer == ds->second->GetSourceType() || ds->second->IsOriginalDatasourceADatabase())
		{
			auto tableNames = ds->second->GetTableNames();
			for (auto tableName : tableNames)
			{
				QStringList names;
				InputTable inputTable(ds->first, tableName);
				m_tableList.push_back(inputTable);
				auto stats = model->GetTableStatsMap().at(inputTable);
				for (auto s : stats)
					names.push_back(s[0]);
				auto treeWidget = new QueryTreeWidget(names, this);
				m_treeWidgets.push_back(treeWidget);
				treeWidget->SetFromTree(ds->second->GetQuery(tableName));
				queryLayout->addWidget(treeWidget);
				treeWidget->hide();
			}
			break; //currently only one database source supported 
		}
	}

	if (m_tableList.size() == 0)
		return;

	m_currentTreeWidget = m_treeWidgets[0];
	m_treeWidgets[0]->show();

	for (int i = 0; i < m_tableList.size(); ++i)
	{
		m_tableComboBox->insertItem(i, QString::fromStdWString(m_tableList[i].GetTable()));

	}

	QObject::connect(m_tableComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index){
		m_currentTreeWidget->hide();
		m_currentTreeWidget = m_treeWidgets[index];
		m_currentTreeWidget->show();
	});
	//m_treeWidget->SetFromTree(dataSource->GetQuery(dataSource->GetTableNames()[0]));
	

	//queryLayout->removeWidget(m_treeWidget);

	QPushButton* addConditionButton = new QPushButton(tr("Add Condition"), this);
	QObject::connect(addConditionButton, &QPushButton::clicked, this, [=]() { m_currentTreeWidget->OnAddCondition(); });
	queryLayout->addWidget(addConditionButton);
	
	QPushButton* addGroupButton = new QPushButton(tr("Add Group"), this);
	QObject::connect(addGroupButton, &QPushButton::clicked, this, [=]() { m_currentTreeWidget->OnAddGroup(); });
	queryLayout->addWidget(addGroupButton);

	QPushButton* removeButton = new QPushButton(tr("Remove"), this);
	QObject::connect(removeButton, &QPushButton::clicked, this, [=]() { m_currentTreeWidget->OnRemove(); });
	queryLayout->addWidget(removeButton);

	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	mainLayout->addWidget(dialogButtonBox);

	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &QueryBuilderDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &QueryBuilderDialog::reject);

	
	setLayout(mainLayout);
}

void QueryBuilderDialog::accept()
{
	auto model = DMGlobal::Services()->GetDataTransformModel();
	auto dataSource = model->GetDataSource(m_tableList[0].GetDatasourceID()); //one data source is currently supported
	for (int i = 0; i < m_treeWidgets.size(); ++i)
	{
		boost::property_tree::wptree  tree;
		m_treeWidgets[i]->GetPropertyTree(tree);
		dataSource->SetQuery(m_tableList[i].GetTable(), tree);
	}

	DMGlobal::Services()->SetModified(true);
	QDialog::accept();
}

#include "QueryBuilderDialog.moc"