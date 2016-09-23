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
#include <QtWidgets/QMenu>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include "DMGlobal.h"
#include "datatransformmodel.h"
#include "databaseserverdatasource.h"
#include "xmlpropertytreefile.h"
#include "browselineedit.h"
using namespace SynGlyphX;

static QString s_currentFile;

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
		m_fileLineEdit->SetFilters("*.sdt");
		m_fileLineEdit->SetReadOnly(true);
		m_fileLineEdit->SetInitalBrowseDirectory(s_directory);
		QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
		QObject::connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &VisDialog::accept);
		QObject::connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &VisDialog::reject);
		mainLayout->addWidget(m_fileLineEdit);
		mainLayout->addWidget(dialogButtonBox);
		setLayout(mainLayout);
	}

	virtual void accept() override
	{
		QString file = m_fileLineEdit->GetText();
		if (!file.isEmpty())
		{
			QDir dir(QFileInfo(s_currentFile).absoluteDir()); //project directory
			s_directory = QFileInfo(file).absoluteDir().absolutePath();
			m_item->setToolTip(0, dir.relativeFilePath(file));
			QDialog::accept();
		}

	}
	QTreeWidgetItem* m_item;
	//QLineEdit* m_nameLineEdit;
	BrowseLineEdit* m_fileLineEdit;
	static QString s_directory;
};
QString VisDialog::s_directory = QString();


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
		item->setText(0, tr("New Group"));
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
			boost::property_tree::wptree& entry = tree.add(L"Group", L"");
			entry.put(L"<xmlattr>.name", item->text(0).toStdWString());
			for (int i = 0; i < item->childCount(); i++)
			{
				auto child = item->child(i);
				ExportToPropertyTree(child, entry);
			}
		}
		else
		{
			boost::property_tree::wptree& entry = tree.add(L"Visualization", L"");
			entry.put(L"<xmlattr>.name", item->text(0).toStdWString());
			entry.put(L"", item->toolTip(0).toStdWString());

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
	s_currentFile.clear();
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QGroupBox* queryGroupBox = new QGroupBox(tr("Project"), this);
	QVBoxLayout* queryLayout = new QVBoxLayout(queryGroupBox);
	mainLayout->addWidget(queryGroupBox);


	m_currentTreeWidget = new TreeWidget(this);
	queryLayout->addWidget(m_currentTreeWidget);

	QPushButton* addConditionButton = new QPushButton(tr("Add Visualisation"), this);
	QObject::connect(addConditionButton, &QPushButton::clicked, this, [=]() { m_currentTreeWidget->OnAddVisualization(); });
	queryLayout->addWidget(addConditionButton);

	QPushButton* addGroupButton = new QPushButton(tr("Add Group"), this);
	QObject::connect(addGroupButton, &QPushButton::clicked, this, [=]() { m_currentTreeWidget->OnAddGroup(); });
	queryLayout->addWidget(addGroupButton);

	QPushButton* editButton = new QPushButton(tr("Change visualisation"), this);
	editButton->setEnabled(false);
	QObject::connect(editButton, &QPushButton::clicked, this, [=]() { m_currentTreeWidget->OnEdit(); });
	queryLayout->addWidget(editButton);

	QPushButton* removeButton = new QPushButton(tr("Remove"), this);
	QObject::connect(removeButton, &QPushButton::clicked, this, [=]() { m_currentTreeWidget->OnRemove(); });
	queryLayout->addWidget(removeButton);
	removeButton->setEnabled(false);

	QObject::connect(m_currentTreeWidget, &TreeWidget::itemSelectionChanged, this, [=]()
	{
		auto selection = m_currentTreeWidget->selectedItems();

		if (selection.size() > 0)
		{
			removeButton->setEnabled(true);
		}
		else
		{
			removeButton->setEnabled(false);
		}

		if (selection.size() > 0 && selection[0] && selection[0]->data(0, Qt::UserRole).toString() != tr("Group"))
		{
			editButton->setEnabled(true);
		}
		else
		{
			editButton->setEnabled(false);
		}

	});

	m_currentTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_currentTreeWidget, &QTreeWidget::customContextMenuRequested, [=](const QPoint &pos){

		QMenu *menu = new QMenu(this);
		connect(menu, SIGNAL(aboutToHide()), menu, SLOT(deleteLater()));;
		QTreeWidgetItem* item = m_currentTreeWidget->itemAt(pos);
		QAction* addVisAction = menu->addAction(tr("Add Visualization"));
		QObject::connect(addVisAction, &QAction::triggered, [=]() {
			m_currentTreeWidget->OnAddVisualization();
		}
		);
		QAction* addGroupAction = menu->addAction(tr("Add Group"));
		QObject::connect(addGroupAction, &QAction::triggered, [=]() {
			m_currentTreeWidget->OnAddGroup();
		}
		);

		if (item && item->data(0, Qt::UserRole).toString() != tr("Group"))
		{
			QAction* changeAction = menu->addAction(tr("Change visualization"));
			QObject::connect(changeAction, &QAction::triggered, [=]() {
				m_currentTreeWidget->OnEdit();
			}
			);

		}
		if (item)
		{
			
			QAction* removeItemAction = menu->addAction(tr("Remove"));
			QObject::connect(removeItemAction, &QAction::triggered, [=]() {
				delete item;
			}
			);
		
		}

		menu->popup(m_currentTreeWidget->viewport()->mapToGlobal(pos));

	});


	QDialogButtonBox* dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
	mainLayout->addWidget(dialogButtonBox);
	QPushButton* openButton = dialogButtonBox->addButton(tr("Open"), QDialogButtonBox::ActionRole);

	QObject::connect(openButton, &QPushButton::clicked, this, [=]() 
	{
		QString openFile = DMGlobal::Services()->GetFileNameOpenDialog("ProjectDir", tr("Open Project"), "", tr("SynGlyphX Project Files (*.xdt)"));
		if (!openFile.isEmpty())
		{
			ProjectFile file;
			s_currentFile = openFile;
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
	QString saveFile = DMGlobal::Services()->GetFileNameSaveDialog("ProjectDir", tr("Save Project"), "", "SynGlyphX Project Files (*.xdt)");
	if (!saveFile.isEmpty())
	{
		ProjectFile file;
		boost::property_tree::wptree propertyTree;
		m_currentTreeWidget->GetPropertyTree(propertyTree.add(L"SharedVisualizations", L""));
		file.ImportFromPropertyTree(propertyTree);
		file.WriteToFile(saveFile.toStdString());
	}
	QDialog::accept();
}

#include "ProjectEditDialog.moc"