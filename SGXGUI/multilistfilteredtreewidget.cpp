#include "multilistfilteredtreewidget.h"
#include <QtWidgets/QTableView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>

namespace SynGlyphX {

	MultiListFilteredTreeWidget::MultiListFilteredTreeWidget(const QStringList& headers, AbstractTreeModel* model, QWidget *parent)
		: QWidget(parent),
		m_sourceModel(model)
	{
		if (headers.size() != model->GetMaxDepth()) {

			throw std::invalid_argument("Number of headers in multi-list widget must be the same as depth of associated model.");
		}

		QGridLayout* mainLayout = new QGridLayout(this);

		QModelIndex parentIndex = m_sourceModel->index(-1, -1);
		for (unsigned int i = 0; i < m_sourceModel->GetMaxDepth(); ++i) {

			QLabel* label = new QLabel(headers[i], this);
			mainLayout->addWidget(label, 0, i, Qt::AlignLeft);
			QTableView* tableView = new QTableView(this);
			//Tree2ListFilterProxyModel* filterModel = new Tree2ListFilterProxyModel(this);
			//filterModel->setSourceModel(m_sourceModel);
			//filterModel->SetFilterParent(parentIndex);
			//m_filterModels.push_back(filterModel);
			tableView->setModel(m_sourceModel);
			tableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
			tableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
			tableView->verticalHeader()->hide();

			tableView->setRootIndex(parentIndex);

			//m_fieldTableHeaderView = new SynGlyphX::CheckBoxHeaderView(Qt::Horizontal, this);

			tableView->resizeColumnsToContents();
			tableView->resizeRowsToContents();

			mainLayout->addWidget(tableView, 1, i);

			parentIndex = m_sourceModel->index(0, 0, parentIndex);
		}

		setLayout(mainLayout);
	}

	MultiListFilteredTreeWidget::~MultiListFilteredTreeWidget()
	{

	}

} //namespace SynGlyphX