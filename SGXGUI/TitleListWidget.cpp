#include "TitleListWidget.h"
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidget>

namespace SynGlyphX {

	TitleListWidget::TitleListWidget(QWidget *parent)
		: QWidget(parent)
	{
		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->setSpacing(0);

		QWidget* titleWidget = new QWidget(this);
		titleWidget->setStyleSheet("QWidget{background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #cdcfd4, stop: 1 #e7e9ea);}");
		QHBoxLayout* titleLayout = new QHBoxLayout(titleWidget);

		m_titleLabel = new QLabel(titleWidget);
		m_titleLabel->setStyleSheet("font-family:'Calibri', Helvetica, Arial, Sans; font-weight: bold; text-transform: uppercase; font-size: 16px; line-height: 24px;");
		titleLayout->addWidget(m_titleLabel);
		titleLayout->addStretch(1);

		m_selectAllButton = new QPushButton(tr("Select All"), titleWidget);
		m_selectAllButton->setStyleSheet("QPushButton{background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e9edf3, stop: 1 #f3f5f8); border-radius: 8px; font-size: 14px; padding: 4px; border-top: 0.5px solid #fff; border-right: 1px outset #fff; border-bottom: 1px solid gray; border-left: 0.5px solid gray;} QPushButton:hover{background-color: #68cff5;}");
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
		m_listWidget->setStyleSheet("font-size: 14px;");
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

	void TitleListWidget::SetItems(const QStringList& labels, const QStringList& tooltips) {

		m_listWidget->clear();
		m_listWidget->addItems(labels);

		if (!tooltips.isEmpty()) {

			for (unsigned int i = 0; i < labels.size(); ++i) {

				m_listWidget->item(i)->setToolTip(tooltips[i]);
			}
		}
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

	QStringList TitleListWidget::GetSelectedLabels() const {

		QStringList items;
		for (const auto& selectedItem : m_listWidget->selectedItems()) {

			items.append(selectedItem->text());
		}

		return items;
	}

	QStringList TitleListWidget::GetSelectedTooltips() const {

		QStringList items;
		for (const auto& selectedItem : m_listWidget->selectedItems()) {

			items.append(selectedItem->toolTip());
		}

		return items;
	}

} //namespace SynGlyphX