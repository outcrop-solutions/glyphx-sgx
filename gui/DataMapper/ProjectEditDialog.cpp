#include "ProjectEditDialog.h"
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
#include "xmlpropertytreefile.h"
#include "browselineedit.h"
using namespace SynGlyphX;

class ProjectFile : public SynGlyphX::XMLPropertyTreeFile // just a property tree holder
{

public:
	ProjectFile() : SynGlyphX::XMLPropertyTreeFile() {}
	~ProjectFile() {}

	void ImportFromPropertyTree(const boost::property_tree::wptree& filePropertyTree) override { m_propertyTree = filePropertyTree; }

	void ExportToPropertyTree(boost::property_tree::wptree& filePropertyTree) const override { filePropertyTree = m_propertyTree; }

	//void ImportFromPropertyTree(const boost::property_tree::wptree& propertyTree, stlplus::ntree<QStringList>::iterator parent);

	boost::property_tree::wptree m_propertyTree;
};

class VisDialog : public QDialog
{
public:
	VisDialog(QWidget* parent, QTreeWidgetItem *item) : QDialog(parent),
		m_item(item)
	{
		Q_ASSERT(item);
		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		//QHBoxLayout *nameLayout = new QHBoxLayout(this);
		//mainLayout->addLayout(nameLayout);
		//nameLayout->addWidget(new QLabel("Name: ", this));
		//m_nameLineEdit = new QLineEdit(this);
		//m_nameLineEdit->setText(item->text(0));
		//nameLayout->addWidget(m_nameLineEdit);
		m_fileLineEdit = new SynGlyphX::BrowseLineEdit(SynGlyphX::BrowseLineEdit::FileDialogType::FileOpen, true, this);
		m_fileLineEdit->SetText(item->toolTip(0));
		m_fileLineEdit->SetFilters("*.xdt");
		QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
		QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &VisDialog::accept);
		QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &VisDialog::reject);
		mainLayout->addWidget(m_fileLineEdit);
		mainLayout->addWidget(dialogButtonBox);
		setLayout(mainLayout);
	}

	virtual void accept() override
	{
		//m_item->setText(0, m_nameLineEdit->text());
		m_item->setToolTip(0, m_fileLineEdit->GetText());
		QDialog::accept();
	}
	QTreeWidgetItem* m_item;
	//QLineEdit* m_nameLineEdit;
	BrowseLineEdit* m_fileLineEdit;
};

class TreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	TreeWidget(/*const QStringList& names,*/ QWidget* parent) : QTreeWidget(parent)/*,
		m_names(names)*/
	{
		setColumnCount(1);
		setHeaderLabel("Visulaizations:");


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

	void OnAddVisualization()
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		item->setText(0, tr("New Visualisation"));
		VisDialog dlg (this, item);
		if (dlg.exec() == QDialog::Accepted)
		{
			SetItem(item);
			//setItemWidget(conditionItem, 0, new StatementWidget(m_names, this));
			resizeColumnToContents(0);
			adjustSize();
		}
		else
		{
			delete item;
		}

	}

	void OnAddGroup()
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setData(0, Qt::UserRole, tr("Group"));
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		SetItem(item);
		//setItemWidget(item, 0, new GroupWidget(this));
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

	void OnEdit()
	{
		auto selection = selectedItems();
		VisDialog dlg(this, selection[0]);
		dlg.exec();
		resizeColumnToContents(0);
		adjustSize();
	}

	void AddVisualization(QTreeWidgetItem* parent, const boost::property_tree::wptree& tree)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, QString::fromStdWString(tree.get<std::wstring>(L"<xmlattr>.name")));
		item->setToolTip(0, QString::fromStdWString(tree.get<std::wstring>(L"")));
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		parent->addChild(item);
		//setItemWidget(widgetItem, 0, new StatementWidget(tree, m_names, this));
		resizeColumnToContents(0);
		adjustSize();
	}

	void AddGroup(QTreeWidgetItem* parent, const boost::property_tree::wptree& tree)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setData(0, Qt::UserRole, tr("Group"));
		item->setText(0, QString::fromStdWString(tree.get<std::wstring>(L"<xmlattr>.name")));
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		parent->addChild(item);
		//setItemWidget(widgetItem, 0, new GroupWidget(tree, this));
		resizeColumnToContents(0);
		adjustSize();
		for (const boost::property_tree::wptree::value_type& childItem : tree)
		{
			if (childItem.first == L"Group")
				AddGroup(item, childItem.second);
			else if (childItem.first == L"Visualization")
				AddVisualization(item, childItem.second);
		}
		item->setExpanded(true);
	}

	void AddTree(QTreeWidgetItem* parent, const boost::property_tree::wptree& tree)
	{
		for (const boost::property_tree::wptree::value_type& treeItem : tree)
		{
			if (treeItem.first == L"Visualization")
			{
				AddVisualization(parent, treeItem.second);
			}
			else if (treeItem.first == L"Group")
			{
				AddGroup(parent, treeItem.second);
			}
		}
	}

	void SetFromTree(const boost::property_tree::wptree& tree)
	{
		AddTree(invisibleRootItem(), tree.get_child(L"SharedVisualizations"));
	}

	boost::property_tree::wptree& ExportToPropertyTree(QTreeWidgetItem* item, boost::property_tree::wptree& tree)
	{
		if (item->data(0, Qt::UserRole).toString() == tr("Group"))
		{
			//boost::property_tree::wptree& entry = tree.add(L"Group", L"");
			//GroupWidget* sw = qobject_cast<GroupWidget*>(itemWidget(item, 0));
			//entry.put(L"<xmlattr>.logic", sw->m_cb->currentText().toStdWString());
			//for (int i = 0; i < item->childCount(); i++)
			//{
			//	auto child = item->child(i);
			//	ExportToPropertyTree(child, entry);
			//}
		}
		else
		{
			//StatementWidget* sw = qobject_cast<StatementWidget*>(itemWidget(item, 0));
			//if (sw)
			//{
			//	boost::property_tree::wptree& entry = tree.add(L"Visualization", L"");
			//	entry.put(L"<xmlattr>.columnname", sw->m_fieldCb->currentText().toStdWString());
			//	entry.put(L"<xmlattr>.operator", sw->m_conditionCb->currentText().toStdWString());
			//	if (sw->m_conditionCb->currentText() == "btw")
			//	{
			//		entry.put(L"<xmlattr>.min", sw->m_lineEdit->text().toStdWString());
			//		entry.put(L"<xmlattr>.max", sw->m_lineEdit2->text().toStdWString());
			//	}
			//	else
			//	{
			//		entry.put(L"<xmlattr>.value", sw->m_lineEdit->text().toStdWString());
			//	}
			//	
			//}

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

	//QStringList m_names;
};


ProjectEditDialog::ProjectEditDialog(QWidget* parent) : QDialog(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	//m_tableComboBox = new QComboBox(this);
	//mainLayout->addWidget(m_tableComboBox, 0, Qt::AlignHCenter);
	QGroupBox* queryGroupBox = new QGroupBox(tr("Project"), this);
	QVBoxLayout* queryLayout = new QVBoxLayout(queryGroupBox);
	mainLayout->addWidget(queryGroupBox);



	//auto model = DMGlobal::Services()->GetDataTransformModel();
	//const DataTransformMapping::DatasourceMap& datasources = model->GetDataMapping()->GetDatasources();
	//if (datasources.size() == 0)
	//	return;

	//for (auto ds = datasources.begin(); ds != datasources.end(); ++ds)
	//{
	//	if (Datasource::SourceType::DatabaseServer == ds->second->GetSourceType() || ds->second->IsOriginalDatasourceADatabase())
	//	{
	//		auto tableNames = ds->second->GetTableNames();
	//		for (auto tableName : tableNames)
	//		{
	//			QStringList names;
	//			InputTable inputTable(ds->first, tableName);
	//			m_tableList.push_back(inputTable);
	//			auto stats = model->GetTableStatsMap().at(inputTable);
	//			for (auto s : stats)
	//				names.push_back(s[0]);
	//			auto treeWidget = new TreeWidget(names, this);
	//			m_treeWidgets.push_back(treeWidget);
	//			treeWidget->SetFromTree(ds->second->GetQuery(tableName));
	//			queryLayout->addWidget(treeWidget);
	//			treeWidget->hide();
	//		}
	//		break; //currently only one database source supported 
	//	}
	//}

	//if (m_tableList.size() == 0)
	//	return;

	m_currentTreeWidget = new TreeWidget(this);
	queryLayout->addWidget(m_currentTreeWidget);

	//m_treeWidgets[0]->show();

	//for (int i = 0; i < m_tableList.size(); ++i)
	//{
	//	m_tableComboBox->insertItem(i, QString::fromStdWString(m_tableList[i].GetTable()));

	//}

	//QObject::connect(m_tableComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int index){
	//	m_currentTreeWidget->hide();
	//	m_currentTreeWidget = m_treeWidgets[index];
	//	m_currentTreeWidget->show();
	//});
	//m_treeWidget->SetFromTree(dataSource->GetQuery(dataSource->GetTableNames()[0]));


	//queryLayout->removeWidget(m_treeWidget);

	QPushButton* addConditionButton = new QPushButton(tr("Add Visualisation"), this);
	QObject::connect(addConditionButton, &QPushButton::clicked, this, [=]() { m_currentTreeWidget->OnAddVisualization(); });
	queryLayout->addWidget(addConditionButton);

	QPushButton* addGroupButton = new QPushButton(tr("Add Group"), this);
	QObject::connect(addGroupButton, &QPushButton::clicked, this, [=]() { m_currentTreeWidget->OnAddGroup(); });
	queryLayout->addWidget(addGroupButton);

	QPushButton* editButton = new QPushButton(tr("Change file"), this);
	editButton->setEnabled(false);
	QObject::connect(editButton, &QPushButton::clicked, this, [=]() { m_currentTreeWidget->OnEdit(); });
	queryLayout->addWidget(editButton);

	QObject::connect(m_currentTreeWidget, &TreeWidget::itemSelectionChanged, this, [=]()
	{
		auto selection = m_currentTreeWidget->selectedItems();
		if (selection[0] && selection[0]->data(0, Qt::UserRole).toString() != tr("Group"))
		{
			editButton->setEnabled(true);
		}
		else
		{
			editButton->setEnabled(false);
		}

	});

	QPushButton* removeButton = new QPushButton(tr("Remove"), this);
	QObject::connect(removeButton, &QPushButton::clicked, this, [=]() { m_currentTreeWidget->OnRemove(); });
	queryLayout->addWidget(removeButton);


	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
	mainLayout->addWidget(dialogButtonBox);
	QPushButton* openButton = dialogButtonBox->addButton(tr("Open"), QDialogButtonBox::ActionRole);

	QObject::connect(openButton, &QPushButton::clicked, this, [=]() 
	{
		QString openFile = DMGlobal::Services()->GetFileNameOpenDialog("ProjectDir", tr("Open Project"), "", tr("SynGlyphX Project Files (*.xdt)"));
		if (!openFile.isEmpty())
		{
			ProjectFile file;
			file.ReadFromFile(openFile.toStdString());
			m_currentTreeWidget->SetFromTree(file.m_propertyTree);
		}

	});

	QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &ProjectEditDialog::accept);
	QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &ProjectEditDialog::reject);



	setLayout(mainLayout);
}

void ProjectEditDialog::accept()
{
	//auto model = DMGlobal::Services()->GetDataTransformModel();
	//auto dataSource = model->GetDataSource(m_tableList[0].GetDatasourceID()); //one data source is currently supported
	//for (int i = 0; i < m_treeWidgets.size(); ++i)
	//{
	//	boost::property_tree::wptree  tree;
	//	m_treeWidgets[i]->GetPropertyTree(tree);
	//	dataSource->SetQuery(m_tableList[i].GetTable(), tree);
	//}

	//DMGlobal::Services()->SetModified(true);
	QDialog::accept();
}

#include "ProjectEditDialog.moc"