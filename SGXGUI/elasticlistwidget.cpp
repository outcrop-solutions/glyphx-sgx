#include "elasticlistwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtGui/QResizeEvent>
#include <QtCore/QSortFilterProxyModel>
#include <QtWidgets/QHBoxLayout>

namespace SynGlyphX {

	const int ElasticListWidget::MaximumNumberOfRowsShown = 4;

	ElasticListWidget::ElasticListWidget(QWidget *parent)
		: QFrame(parent)
	{
		setFrameStyle(QFrame::Shape::Box | QFrame::Shadow::Sunken);

		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->setContentsMargins(0, 0, 1, 1);
		mainLayout->setSpacing(0);

		QHBoxLayout* titleLayout = new QHBoxLayout(this);
		titleLayout->setContentsMargins(4, 4, 3, 4);

		m_title = new QLabel(this);
		titleLayout->addWidget(m_title);

		titleLayout->addStretch(1);

		m_showAllCheckBox = new QCheckBox(tr("Show All"), this);
		m_showAllCheckBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		titleLayout->addWidget(m_showAllCheckBox);
		QObject::connect(m_showAllCheckBox, &QCheckBox::toggled, this, &ElasticListWidget::ResizeTable);
		m_showAllCheckBox->hide();

		mainLayout->addLayout(titleLayout);

		QFrame* hLine = new QFrame(this);
		hLine->setFrameStyle(QFrame::Shape::HLine | QFrame::Shadow::Sunken);
		mainLayout->addWidget(hLine);

		m_model = new ElasticListModel(this);

		QSortFilterProxyModel* sortModel = new QSortFilterProxyModel(this);
		sortModel->setFilterKeyColumn(-1);
		sortModel->setSourceModel(m_model);

		m_dataAndCountView = new QTableView(this);
		m_dataAndCountView->setFrameShape(QFrame::Shape::NoFrame);
		m_dataAndCountView->setModel(sortModel);
		m_dataAndCountView->setSortingEnabled(true);
		m_dataAndCountView->setMinimumWidth(32);
		m_dataAndCountView->setShowGrid(false);
		
		m_dataAndCountView->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_dataAndCountView->setSelectionMode(QAbstractItemView::MultiSelection);
		m_dataAndCountView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		m_dataAndCountView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
		m_dataAndCountView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
		m_dataAndCountView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
		m_dataAndCountView->horizontalHeader()->setStretchLastSection(false);
		m_dataAndCountView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
		m_dataAndCountView->verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 4);
		m_dataAndCountView->verticalHeader()->hide();
		QObject::connect(m_dataAndCountView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ElasticListWidget::OnNewUserSelection);
		mainLayout->addWidget(m_dataAndCountView, 1);

		setLayout(mainLayout);

		QMargins margins = contentsMargins();
		setMinimumWidth(16);
		m_dataAndCountView->setMinimumHeight(m_dataAndCountView->verticalHeader()->defaultSectionSize());
		m_dataAndCountView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
		ResizeTable();
	}

	ElasticListWidget::~ElasticListWidget()
	{

	}

	void ElasticListWidget::SetTitle(const QString& title) {

		m_title->setText(title);
	}

	QString ElasticListWidget::GetTitle() const {

		return m_title->text();
	}

	void ElasticListWidget::SetData(const ElasticListModel::Data& data) {

		bool listBlockSignals = signalsBlocked();
		blockSignals(true);

		bool isAllDataInSelection = true;
		for (int i = 0; i < data.size(); ++i) {

			if (m_selectedRawData.count(ElasticListModel::ConvertQVariantToRawString(data[i].first)) == 0) {

				isAllDataInSelection = false;
				break;
			}
		}

		m_model->ResetData(data);
		m_dataAndCountView->sortByColumn(1, Qt::DescendingOrder);
		ResizeTable();

		if (isAllDataInSelection) {

			QItemSelection selection;
			selection.select(m_dataAndCountView->model()->index(0, 0), m_dataAndCountView->model()->index(data.size()-1, 2));
			m_dataAndCountView->selectionModel()->select(selection, QItemSelectionModel::Select);
		}
		else {

			m_selectedRawData.clear();
		}

		blockSignals(listBlockSignals);
	}
	
	void ElasticListWidget::ResizeTable() {

		unsigned int numberOfVisibleRows = 0; 
		if (m_showAllCheckBox->isChecked()) {

			numberOfVisibleRows = m_model->rowCount();
		}
		else {

			numberOfVisibleRows = std::max(std::min(MaximumNumberOfRowsShown, m_model->rowCount()), 1);
		}
		
		m_dataAndCountView->setFixedHeight((numberOfVisibleRows + 1.5) * m_dataAndCountView->verticalHeader()->defaultSectionSize());
		m_dataAndCountView->resizeColumnToContents(1);
	}

	void ElasticListWidget::OnNewUserSelection() {

		m_selectedRawData.clear();
		QSortFilterProxyModel* sortModel = dynamic_cast<QSortFilterProxyModel*>(m_dataAndCountView->model());
		QItemSelection selection = sortModel->mapSelectionToSource(m_dataAndCountView->selectionModel()->selection());
		for (QModelIndex& selectedIndex : selection.indexes()) {

			m_selectedRawData.insert(m_model->data(selectedIndex, ElasticListModel::RawDataRole).toString());
		}

		emit SelectionChanged();
	}

	const std::set<QString>& ElasticListWidget::GetSelectedRawData() const {

		return m_selectedRawData;
	}

} //namespace SynGlyphX