#include "elasticlistwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHeaderView>

namespace SynGlyphX {

	const unsigned int ElasticListWidget::MaximumNumberOfRowsShown = 4u;

	ElasticListWidget::ElasticListWidget(QWidget *parent)
		: QFrame(parent)
	{
		QVBoxLayout* layout = new QVBoxLayout(this);

		m_title = new QLabel(this);
		layout->addWidget(m_title);

		m_list = new QTableWidget(this);
		m_list->setColumnCount(2);
		m_list->setMinimumWidth(32);
		m_list->setShowGrid(false);
		m_list->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_list->setSelectionMode(QAbstractItemView::SingleSelection);
		m_list->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
		m_list->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
		m_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		m_list->horizontalHeader()->hide();
		m_list->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
		m_list->verticalHeader()->hide();
		layout->addWidget(m_list, 1);

		setLayout(layout);

		setMinimumSize(16, m_title->sizeHint().height() + layout->spacing() + m_list->verticalHeader()->defaultSectionSize());
		setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

		/*m_list->setRowCount(4);
		m_list->setItem(0, 0, new QTableWidgetItem("blah"));
		m_list->setItem(1, 0, new QTableWidgetItem("blah"));
		m_list->setItem(2, 0, new QTableWidgetItem("blah"));
		m_list->setItem(3, 0, new QTableWidgetItem("blah"));

		m_list->setItem(0, 1, new QTableWidgetItem(QString::number(14)));
		m_list->setItem(1, 1, new QTableWidgetItem(QString::number(11)));
		m_list->setItem(2, 1, new QTableWidgetItem(QString::number(58)));
		m_list->setItem(3, 1, new QTableWidgetItem(QString::number(99)));

		m_list->sortByColumn(1, Qt::DescendingOrder);*/

		Resize(4);
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

			m_list->setItem(i, 0, new QTableWidgetItem(data[i].first));
			m_list->setItem(i, 1, new QTableWidgetItem(data[i].second));
		}

		m_list->sortByColumn(1, Qt::DescendingOrder);
		Resize(data.size());
	}

	void ElasticListWidget::Resize(unsigned int numberOfRows) {

		int listHeight = 0;
		int numberOfVisibleRows = std::min(MaximumNumberOfRowsShown, numberOfRows);
		for (unsigned int i = 0; i < numberOfVisibleRows; ++i) {

			listHeight += m_list->rowHeight(i) + 5;
		}

		int heightWidget = height();
		int heightWithoutList = height() - m_list->viewport()->height();
		setFixedHeight(heightWithoutList + listHeight);// +1 * (numberOfVisibleRows - 1));
	}

} //namespace SynGlyphX