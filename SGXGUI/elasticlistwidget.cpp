#include "elasticlistwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtGui/QResizeEvent>

namespace SynGlyphX {

	const int ElasticListWidget::MaximumNumberOfRowsShown = 4;

	ElasticListWidget::ElasticListWidget(QWidget *parent)
		: QFrame(parent)
	{
		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->setSizeConstraint(QLayout::SetMinAndMaxSize);

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
		m_list->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
		m_list->verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 4);
		m_list->verticalHeader()->hide();
		layout->addWidget(m_list, 1);

		setLayout(layout);

		QMargins margins = contentsMargins();
		setMinimumWidth(16);
		m_list->setMinimumHeight(m_list->verticalHeader()->defaultSectionSize());
		m_list->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
		ResizeTable();
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

			m_list->setItem(i, 0, CreateTableWidgetItem(data[i].first));
			m_list->setItem(i, 1, CreateTableWidgetItem(data[i].second));
		}

		m_list->sortByColumn(1, Qt::DescendingOrder);
		ResizeTable();
	}
	
	void ElasticListWidget::ResizeTable() {

		int numberOfVisibleRows = std::max(std::min(MaximumNumberOfRowsShown, m_list->rowCount()), 1);
		m_list->setFixedHeight(numberOfVisibleRows * m_list->verticalHeader()->defaultSectionSize());
	}

	QTableWidgetItem* ElasticListWidget::CreateTableWidgetItem(const QString& text) const {

		QTableWidgetItem* item = new QTableWidgetItem(text);
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		return item;
	}

} //namespace SynGlyphX