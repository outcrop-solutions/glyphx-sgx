#include "elasticlistwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHeaderView>

namespace SynGlyphX {

	ElasticListWidget::ElasticListWidget(QWidget *parent)
		: QFrame(parent)
	{
		QVBoxLayout* layout = new QVBoxLayout(this);

		m_title = new QLabel(this);
		layout->addWidget(m_title);

		m_list = new QTableWidget(this);
		m_list->setColumnCount(2);
		m_list->setShowGrid(false);
		m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_list->setSelectionMode(QAbstractItemView::SingleSelection);
		m_list->horizontalHeader()->hide();
		m_list->verticalHeader()->hide();
		layout->addWidget(m_list, 1);

		setLayout(layout);
	}

	ElasticListWidget::~ElasticListWidget()
	{

	}

	void ElasticListWidget::SetTitle(const QString& title) {

		m_title->setText(title);
	}

	void ElasticListWidget::SetData(const Data& data) {

		m_list->clear();
		m_list->setRowCount(data.size());

		for (int i = 0; i < data.size(); ++i) {

			m_list->item(i, 0)->setData(Qt::DisplayRole, data[i].first);
			m_list->item(i, 0)->setData(Qt::DisplayRole, data[i].second);
		}

		m_list->sortByColumn(1, Qt::DescendingOrder);
		m_list->resizeRowsToContents();
		m_list->resizeColumnToContents(1);
	}

} //namespace SynGlyphX