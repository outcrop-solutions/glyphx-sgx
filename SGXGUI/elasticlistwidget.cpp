#include "elasticlistwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtGui/QResizeEvent>
#include "roledatafilterproxymodel.h"
#include <QtWidgets/QHBoxLayout>
#include <QtGui/QMouseEvent>
#include "application.h"

namespace SynGlyphX {

	const unsigned int ElasticListWidget::MaximumNumberOfRowsShown = 4;

	ElasticListWidget::ElasticListWidget(QWidget *parent)
		: QFrame(parent),
		m_hasUserChangedSelectedItemsInTable(false)
	{
		setFrameStyle(QFrame::Shape::Box | QFrame::Shadow::Sunken);

		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->setContentsMargins(0, 0, 1, 1);
		mainLayout->setSpacing(0);

		QHBoxLayout* titleLayout = new QHBoxLayout();
		titleLayout->setContentsMargins(4, 4, 3, 4);

		m_title = new QLabel(this);
		titleLayout->addWidget(m_title);

		titleLayout->addStretch(1);

		m_showAllCheckBox = new QCheckBox(tr("Show All"), this);
		m_showAllCheckBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		titleLayout->addWidget(m_showAllCheckBox);
		QObject::connect(m_showAllCheckBox, &QCheckBox::toggled, this, &ElasticListWidget::ResizeTable);

		mainLayout->addLayout(titleLayout);

		QFrame* hLine = new QFrame(this);
		hLine->setFrameStyle(QFrame::Shape::HLine | QFrame::Shadow::Sunken);
		mainLayout->addWidget(hLine);

		m_model = new ElasticListModel(this);

		StringRoleDataFilterProxyModel* filterModel = new StringRoleDataFilterProxyModel(this);
		filterModel->setFilterKeyColumn(0);
		filterModel->setFilterRole(ElasticListModel::RawDataRole);
		filterModel->setSourceModel(m_model);

		m_dataAndCountView = new QTableView(this);
		m_dataAndCountView->setFrameShape(QFrame::Shape::NoFrame);
		m_dataAndCountView->setModel(filterModel);
		m_dataAndCountView->setSortingEnabled(true);
		m_dataAndCountView->setMinimumWidth(32);
		m_dataAndCountView->setShowGrid(false);
		
		m_dataAndCountView->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_dataAndCountView->setSelectionMode(QAbstractItemView::ExtendedSelection);
		m_dataAndCountView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		m_dataAndCountView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
		m_dataAndCountView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
		m_dataAndCountView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
		m_dataAndCountView->horizontalHeader()->setStretchLastSection(false);
		m_dataAndCountView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
		m_dataAndCountView->verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 4);
		m_dataAndCountView->verticalHeader()->hide();
		m_dataAndCountView->installEventFilter(this);
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

	bool ElasticListWidget::HasSelection() const {

		return (!m_selectedRawData.empty());
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

		m_selectedRawData.clear();
		m_dataAndCountView->selectionModel()->clear();

		if (data.size() > 1) {

			m_dataAndCountView->setSelectionMode(QAbstractItemView::ExtendedSelection);
		}
		else {

			m_dataAndCountView->setSelectionMode(QAbstractItemView::NoSelection);
		}

		m_model->ResetData(data);
		
		QSortFilterProxyModel* filterModel = dynamic_cast<QSortFilterProxyModel*>(m_dataAndCountView->model());
		filterModel->setFilterKeyColumn(-1);
		
		m_dataAndCountView->sortByColumn(1, Qt::DescendingOrder);
		ResizeTable();

		blockSignals(listBlockSignals);
	}
	/*
	void ElasticListWidget::SetSelectedData(const ElasticListModel::Data& data) {

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
	*/
	void ElasticListWidget::ResizeTable() {

		StringRoleDataFilterProxyModel* filterModel = dynamic_cast<StringRoleDataFilterProxyModel*>(m_dataAndCountView->model());
		if (m_selectedRawData.empty() || (m_showAllCheckBox->isChecked())) {

			filterModel->Clear();
		}
		else {

			filterModel->SetFilterData(m_selectedRawData);
		}

		unsigned int numberOfVisibleRows = 0; 
		if (m_showAllCheckBox->isChecked()) {

			numberOfVisibleRows = filterModel->rowCount();
		}
		else {

			unsigned int numberOfAvailableRows = m_dataAndCountView->selectionModel()->selectedRows().count();
			if (numberOfAvailableRows == 0) {

				numberOfAvailableRows = filterModel->rowCount();
			}
			numberOfVisibleRows = std::max(std::min(MaximumNumberOfRowsShown, numberOfAvailableRows), 1u);
		}
		
		m_dataAndCountView->setFixedHeight((numberOfVisibleRows + 1.5) * m_dataAndCountView->verticalHeader()->defaultSectionSize());
		m_dataAndCountView->resizeColumnToContents(1);
	}

	void ElasticListWidget::OnNewUserSelection() {

		Qt::KeyboardModifiers keyboardModifiers = Application::queryKeyboardModifiers();
		if (keyboardModifiers.testFlag(Qt::ControlModifier) || keyboardModifiers.testFlag(Qt::ShiftModifier)) {

			//if multiselection wait until Ctrl or Shift is released
			m_hasUserChangedSelectedItemsInTable = true;
		}
		else {

			//if single selection, update
			ChangeSelection();
		}
	}

	void ElasticListWidget::ChangeSelection() {

		SynGlyphX::Application::SetOverrideCursorAndProcessEvents(Qt::WaitCursor);
		m_hasUserChangedSelectedItemsInTable = false;

		m_selectedRawData.clear();
		StringRoleDataFilterProxyModel* filterModel = dynamic_cast<StringRoleDataFilterProxyModel*>(m_dataAndCountView->model());
		QItemSelection selection = filterModel->mapSelectionToSource(m_dataAndCountView->selectionModel()->selection());
		for (QModelIndex& selectedIndex : selection.indexes()) {

			m_selectedRawData.insert(m_model->data(selectedIndex, ElasticListModel::RawDataRole).toString());
		}

		ResizeTable();

		emit SelectionChanged();
		SynGlyphX::Application::restoreOverrideCursor();
	}

	const QSet<QString>& ElasticListWidget::GetSelectedRawData() const {

		return m_selectedRawData;
	}

	bool ElasticListWidget::eventFilter(QObject *obj, QEvent *ev) {

		//if in the middle of multiselect, then don't call OnNewUserSelection() until multiselect is done
		if (obj == m_dataAndCountView) {

			//handles multiselection involving ctrl or shift from table
			if (ev->type() == QEvent::KeyRelease) {

				QKeyEvent *keyEvent = static_cast<QKeyEvent*>(ev);
				Qt::KeyboardModifiers keyboardModifiers = keyEvent->modifiers();
				if (((keyEvent->key() == Qt::Key_Shift) || (keyEvent->key() == Qt::Key_Control)) && m_hasUserChangedSelectedItemsInTable) {

					OnNewUserSelection();
				}
			}
		}

		return false;
	}

} //namespace SynGlyphX