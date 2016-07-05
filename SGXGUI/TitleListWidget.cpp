#include "TitleListWidget.h"
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

namespace SynGlyphX {

	TitleListWidget::TitleListWidget(QWidget *parent)
		: QWidget(parent)
	{
		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->setSpacing(0);

		QWidget* titleWidget = new QWidget(this);
		QHBoxLayout* titleLayout = new QHBoxLayout(titleWidget);

		m_titleLabel = new QLabel(titleWidget);
		m_titleLabel->setStyleSheet("font-weight: bold");
		titleLayout->addWidget(m_titleLabel);
		titleLayout->addStretch(1);

		m_selectAllButton = new QPushButton(tr("Select All"), titleWidget);
		titleLayout->addWidget(m_selectAllButton);

		titleWidget->setLayout(titleLayout);
		mainLayout->addWidget(titleWidget);

		QSizePolicy sizePolicy = m_selectAllButton->sizePolicy();
		sizePolicy.setRetainSizeWhenHidden(true);
		m_selectAllButton->setSizePolicy(sizePolicy);

		QFrame* hLine = new QFrame(this);
		hLine->setFrameStyle(QFrame::HLine | QFrame::Sunken);
		hLine->setLineWidth(1);
		hLine->setMidLineWidth(2);
		hLine->setContentsMargins(0, 0, 0, 0);
		mainLayout->addWidget(hLine);

		m_listWidget = new QListWidget(this);
		m_listWidget->setFrameStyle(QFrame::NoFrame);
		m_listWidget->setSortingEnabled(false);
		
		mainLayout->addWidget(m_listWidget);

		setLayout(mainLayout);

		QObject::connect(m_selectAllButton, &QPushButton::clicked, m_listWidget, &QListWidget::selectAll);
		QObject::connect(m_listWidget, &QListWidget::currentRowChanged, this, &TitleListWidget::CurrentRowChanged);
	}

	TitleListWidget::~TitleListWidget()
	{

	}

	void TitleListWidget::ShowSelectAllButton(bool show) {

		m_selectAllButton->setVisible(show);
	}

	void TitleListWidget::SetAllowMultiselect(bool allow) {

		m_selectAllButton->setEnabled(allow);
		if (allow) {

			m_listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
		}
		else {

			m_listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
		}
	}

	void TitleListWidget::SetTitle(const QString& title) {

		m_titleLabel->setText(title);
	}

	void TitleListWidget::SetItems(const QStringList& items) {

		m_listWidget->clear();
		m_listWidget->addItems(items);
	}

	bool TitleListWidget::AreAnyItemsSelected() const {

		return !m_listWidget->selectedItems().isEmpty();
	}

	bool TitleListWidget::AreAllItemsSelected() const {

		return (m_listWidget->selectedItems().size() == m_listWidget->count());
	}

	void TitleListWidget::SelectItem(unsigned int index) {

		m_listWidget->selectionModel()->select(m_listWidget->model()->index(index, 0), QItemSelectionModel::SelectionFlag::ClearAndSelect);
	}

	QStringList TitleListWidget::GetSelectedItems() const {

		QStringList items;
		for (const auto& selectedItem : m_listWidget->selectedItems()) {

			items.append(selectedItem->text());
		}

		return items;
	}

} //namespace SynGlyphX