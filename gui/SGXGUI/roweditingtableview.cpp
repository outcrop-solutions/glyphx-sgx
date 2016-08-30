#include "roweditingtableview.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QHBoxLayout>
#include <set>
#include <functional>
#include "sharedactionlist.h"

namespace SynGlyphX {

	RowEditingTableView::RowEditingTableView(QWidget *parent)
		: QWidget(parent)
	{
		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->setContentsMargins(0, 0, 0, 0);
		mainLayout->setSpacing(0);

		m_topButtonLayout = new QHBoxLayout(this);
		m_topButtonLayout->setContentsMargins(8, 8, 8, 8);

		m_addButton = new QPushButton(tr("Add"), this);
		QObject::connect(m_addButton, &QPushButton::clicked, this, &RowEditingTableView::OnAddRows);
		m_topButtonLayout->addWidget(m_addButton, 1);
		m_addButton->setEnabled(false);

		m_removeAllButton = new QPushButton(tr("Remove All"), this);
		QObject::connect(m_removeAllButton, &QPushButton::clicked, this, &RowEditingTableView::OnRemoveAllRows);
		m_topButtonLayout->addWidget(m_removeAllButton, 1);
		m_removeAllButton->setEnabled(false);

		/*m_moveUpButton = new QPushButton(this);
		m_moveUpButton->setIcon(QIcon(":SGXGUI/Resources/up_arrow.png"));
		QObject::connect(m_moveUpButton, &QPushButton::clicked, this, &RowEditingTableView::OnMoveUpRow);
		m_topButtonLayout->addWidget(m_moveUpButton);
		m_moveUpButton->setEnabled(false);

		m_moveDownButton = new QPushButton(this);
		m_moveDownButton->setIcon(QIcon(":SGXGUI/Resources/down_arrow.png"));
		QObject::connect(m_moveDownButton, &QPushButton::clicked, this, &RowEditingTableView::OnMoveDownRow);
		m_topButtonLayout->addWidget(m_moveDownButton);
		m_moveDownButton->setEnabled(false);*/

		m_topButtonLayout->addStretch(1);

		mainLayout->addLayout(m_topButtonLayout);

		QFrame* border = new QFrame(this);
		border->setFrameStyle(QFrame::Shape::HLine | QFrame::Shadow::Sunken);
		mainLayout->addWidget(border);

		m_tableView = new QTableView(this);
		m_tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
		m_tableView->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
		m_tableView->setFrameShape(QFrame::Shape::NoFrame);
		m_tableView->horizontalHeader()->setStretchLastSection(true);
		m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
		m_tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
		m_tableView->verticalHeader()->hide();
		m_tableView->setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

		m_removeSelectedContextMenuAction = new QAction(tr("Remove"), m_tableView);
		QObject::connect(m_removeSelectedContextMenuAction, &QAction::triggered, this, &RowEditingTableView::OnRemoveSelectedRows);
		m_tableView->addAction(m_removeSelectedContextMenuAction);

		m_removeAllContextMenuAction = new QAction(tr("Remove All"), m_tableView);
		QObject::connect(m_removeAllContextMenuAction, &QAction::triggered, this, &RowEditingTableView::OnRemoveAllRows);
		m_tableView->addAction(m_removeAllContextMenuAction);

		/*m_tableView->addAction(SynGlyphX::SharedActionList::CreateSeparator(this));

		m_moveRowUpContextMenuAction = new QAction(tr("Move Up"), m_tableView);
		QObject::connect(m_moveRowUpContextMenuAction, &QAction::triggered, this, &RangeFilterListWidget::OnMoveUpRow);
		QObject::connect(m_moveRowUpContextMenuAction, &QAction::changed, this, [&, this](){ UpdatedEnableStateForButton(m_moveRowUpContextMenuAction, m_moveUpButton); });
		m_tableView->addAction(m_moveRowUpContextMenuAction);

		m_moveRowDownContextMenuAction = new QAction(tr("Move Down"), m_tableView);
		QObject::connect(m_moveRowDownContextMenuAction, &QAction::triggered, this, &RangeFilterListWidget::OnMoveDownRow);
		QObject::connect(m_moveRowDownContextMenuAction, &QAction::changed, this, [&, this](){ UpdatedEnableStateForButton(m_moveRowDownContextMenuAction, m_moveDownButton); });
		m_tableView->addAction(m_moveRowDownContextMenuAction);*/

		mainLayout->addWidget(m_tableView, 1);

		setLayout(mainLayout);
	}

	RowEditingTableView::~RowEditingTableView()
	{

	}

	void RowEditingTableView::OnAddRows() {

		AddRows();
		UpdateEnabledStates();
	}

	void RowEditingTableView::OnRemoveSelectedRows() {

		const QModelIndexList& modelIndexList = m_tableView->selectionModel()->selectedRows();

		//sort into highest row number first
		std::set<unsigned int, std::greater<unsigned int>> rowsToRemove;
		for (const auto& modelIndex : modelIndexList) {

			rowsToRemove.insert(modelIndex.row());
		}
		
		QAbstractItemModel* tableViewModel = m_tableView->model();
		for (unsigned int row : rowsToRemove) {

			tableViewModel->removeRow(row);
		}
		UpdateEnabledStates();
	}

	void RowEditingTableView::OnRemoveAllRows() {

		QAbstractItemModel* tableViewModel = m_tableView->model();
		tableViewModel->removeRows(0, tableViewModel->rowCount());
		UpdateEnabledStates();
	}

	void RowEditingTableView::UpdateEnabledStates() {

		bool doAnyRowsExist = (m_tableView->model()->rowCount() != 0);
		bool areAnyRowsSelected = (!m_tableView->selectionModel()->selectedRows().isEmpty());

		m_removeAllButton->setEnabled(doAnyRowsExist);
		m_removeAllContextMenuAction->setEnabled(doAnyRowsExist);

		m_removeSelectedContextMenuAction->setEnabled(areAnyRowsSelected);
	}

} //namespace SynGlyphX