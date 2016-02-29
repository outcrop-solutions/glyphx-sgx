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

			CheckBoxHeaderView* fieldTableHeaderView = new CheckBoxHeaderView(Qt::Horizontal, this);
			fieldTableHeaderView->setStretchLastSection(true);
			tableView->setHorizontalHeader(fieldTableHeaderView);
			QObject::connect(fieldTableHeaderView, &CheckBoxHeaderView::CheckBoxClicked, this, &MultiListFilteredTreeWidget::OnCheckBoxHeaderViewClicked);
			ConnectCheckBoxSignal();
			m_checkBoxHeaderViews.push_back(fieldTableHeaderView);

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

	void MultiListFilteredTreeWidget::ConnectCheckBoxSignal() {

		m_checkBoxConnection = QObject::connect(m_sourceModel, &AbstractTreeModel::dataChanged, this, &MultiListFilteredTreeWidget::OnCheckBoxChanged);
	}

	void MultiListFilteredTreeWidget::OnCheckBoxHeaderViewClicked(SynGlyphX::AllSomeNone state) {

		int checkState = Qt::CheckState::Unchecked;
		if (state == SynGlyphX::AllSomeNone::All) {

			checkState = Qt::CheckState::Checked;
		}
		
		QObject::disconnect(m_checkBoxConnection);
		for (int i = 0; i < m_sourceModel->rowCount(); ++i) {

			m_sourceModel->setData(m_sourceModel->index(i, 0), checkState, Qt::CheckStateRole);
		}
		ConnectCheckBoxSignal();
	}

	void MultiListFilteredTreeWidget::OnCheckBoxChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {

		if ((!roles.isEmpty()) && (roles[0] == Qt::CheckStateRole)) {

			bool checked = false;
			bool unchecked = false;
			for (int i = 0; i < m_sourceModel->rowCount(topLeft.parent()); ++i) {

				if (m_sourceModel->data(m_sourceModel->index(i, 0, topLeft.parent()), Qt::CheckStateRole).toInt() == Qt::Checked) {

					checked = true;
				}
				else {

					unchecked = true;
				}
			}

			if (checked && unchecked) {

				m_checkBoxHeaderViews[0]->SetState(AllSomeNone::Some);
			}
			else if (checked) {

				m_checkBoxHeaderViews[0]->SetState(AllSomeNone::All);
			}
			else {

				m_checkBoxHeaderViews[0]->SetState(AllSomeNone::None);
			}
		}
	}

} //namespace SynGlyphX