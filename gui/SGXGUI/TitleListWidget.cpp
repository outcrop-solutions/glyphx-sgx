#include "TitleListWidget.h"
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QLineEdit>

namespace SynGlyphX {

	TitleListWidget::TitleListWidget(QWidget *parent)
		: QWidget(parent)
	{
		QVBoxLayout* mainLayout = new QVBoxLayout(this);
		mainLayout->setSpacing(0);
		setStyleSheet("QTableWidget::item:selected{ background-color: red }");

		QWidget* titleWidget = new QWidget(this);
		titleWidget->setStyleSheet("QWidget{background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #cdcfd4, stop: 1 #e7e9ea);}");
		QHBoxLayout* titleLayout = new QHBoxLayout(titleWidget);

		m_titleLabel = new QLabel(titleWidget);
		m_titleLabel->setStyleSheet("font-family:'Calibri', Helvetica, Arial, Sans; font-weight: bold; text-transform: uppercase; font-size: 12pt; line-height: 24px;");
		titleLayout->addWidget(m_titleLabel);
		titleLayout->addStretch(1);

		m_selectAllButton = new QPushButton(tr("Select All"), titleWidget);
		m_selectAllButton->setStyleSheet("QPushButton{background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e9edf3, stop: 1 #f3f5f8); border-radius: 8px; font-size: 12pt; padding: 4px; border-top: 0.5px solid #fff; border-right: 1px outset #fff; border-bottom: 1px solid #c3c5c5; border-left: 0.5px solid #c3c5c5;} QPushButton:hover{background-color: #68cff5;}");
		titleLayout->addWidget(m_selectAllButton);

		titleWidget->setLayout(titleLayout);
		mainLayout->addWidget(titleWidget);

		QSizePolicy sizePolicy = m_selectAllButton->sizePolicy();
		sizePolicy.setRetainSizeWhenHidden(true);
		m_selectAllButton->setSizePolicy(sizePolicy);
		m_search = new QLineEdit(this);
		m_search->setClearButtonEnabled(true);
		m_search->setPlaceholderText(tr("Search Filter"));
		m_search->setStyleSheet("QLineEdit{font-size: 11pt;}");
		mainLayout->addWidget(m_search);
		m_search->setVisible(false);
		QFrame* hLine = new QFrame(this);
		hLine->setFrameStyle(QFrame::HLine | QFrame::Plain);
		hLine->setLineWidth(1);
		hLine->setMidLineWidth(1);
		hLine->setContentsMargins(0, 0, 0, 0);
		mainLayout->addWidget(hLine);

		m_listWidget = new QListWidget(this);
		m_listWidget->setStyleSheet("QListWidget{font-size: 11pt;background-color: #eff2f7;} QListWidget::item:selected {background-color: #4acaf8;color: white;}");
		m_listWidget->setFrameStyle(QFrame::NoFrame);
		m_listWidget->setSortingEnabled(false);

		mainLayout->addWidget(m_listWidget);

		setLayout(mainLayout);

		QObject::connect(m_selectAllButton, &QPushButton::clicked, m_listWidget, &QListWidget::selectAll);
		QObject::connect(m_listWidget, &QListWidget::currentRowChanged, this, &TitleListWidget::CurrentRowChanged);

		QObject::connect(m_search, &QLineEdit::textChanged, this, &TitleListWidget::OnFilterTextChanged);
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

			for (int i = 0; i < labels.size(); ++i) {

				m_listWidget->item(i)->setToolTip(tooltips[i]);
			}
		}
	}

	void TitleListWidget::ShowSearchFilter(bool show) {
		m_search->setVisible(show);
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
	void TitleListWidget::OnFilterTextChanged(const QString& text) {

		m_listWidget->item(0)->setHidden(true);
		for (int row = 0 ; row < m_listWidget->count(); row++)
			m_listWidget->item(row)->setHidden(true);
		QList<QListWidgetItem*> matches(m_listWidget->findItems(text, Qt::MatchFlag::MatchContains));
		for (QListWidgetItem* item : matches)
			item->setHidden(false);
	}

} //namespace SynGlyphX