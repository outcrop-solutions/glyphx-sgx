#include "TitleTreeWidget.h"
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>

namespace SynGlyphX {

	TitleTreeWidget::TitleTreeWidget(QWidget *parent)
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

		QPushButton* selectAllButton = new QPushButton(tr("Select All"), titleWidget);
		selectAllButton->setStyleSheet("QPushButton{background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #e9edf3, stop: 1 #f3f5f8); border-radius: 8px; font-size: 14px; padding: 4px; border-top: 0.5px solid #fff; border-right: 1px outset #fff; border-bottom: 1px solid gray; border-left: 0.5px solid gray;} QPushButton:hover{background-color: #68cff5;}");
		titleLayout->addWidget(selectAllButton);

		titleWidget->setLayout(titleLayout);
		mainLayout->addWidget(titleWidget);

		QSizePolicy sizePolicy = selectAllButton->sizePolicy();
		sizePolicy.setRetainSizeWhenHidden(true);
		selectAllButton->setSizePolicy(sizePolicy);
		selectAllButton->hide();

		QFrame* hLine = new QFrame(this);
		hLine->setFrameStyle(QFrame::HLine | QFrame::Sunken);
		hLine->setLineWidth(1);
		hLine->setMidLineWidth(2);
		hLine->setContentsMargins(0, 0, 0, 0);
		mainLayout->addWidget(hLine);

		m_treeWidget = new QTreeWidget(this);
		m_treeWidget->setStyleSheet("font-size: 14px;");
		m_treeWidget->setFrameStyle(QFrame::NoFrame);
		m_treeWidget->setSortingEnabled(false);
		m_treeWidget->header()->close();

		mainLayout->addWidget(m_treeWidget);

		setLayout(mainLayout);

		QObject::connect(m_treeWidget, &QTreeWidget::itemSelectionChanged, this, &TitleTreeWidget::OnSelectionChanged);
	}

	TitleTreeWidget::~TitleTreeWidget()
	{

	}

	void TitleTreeWidget::SetAllowMultiselect(bool allow) {

		if (allow) {

			m_treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
		}
		else {

			m_treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
		}
	}

	void TitleTreeWidget::SetTitle(const QString& title) {

		m_titleLabel->setText(title);
	}

	void TitleTreeWidget::SetItem(stlplus::ntree<QStringList>::const_iterator iT, QTreeWidgetItem* item) {

		Qt::ItemFlags flags = Qt::ItemIsEnabled;
		if (iT->size() == 1) {

			item->setToolTip(0, (*iT)[0]);
		}
		else {

			flags |= Qt::ItemIsSelectable;
			item->setToolTip(0, (*iT)[1]);

			if (m_firstSelectableItem == nullptr) {

				m_firstSelectableItem = item;
			}
		}
		item->setFlags(flags);
		item->setText(0, (*iT)[0]);
	}

	void TitleTreeWidget::SetItems(const stlplus::ntree<QStringList>& labelsAndTooltips) {

		m_treeWidget->clear();
		m_firstSelectableItem = nullptr;

		stlplus::ntree<QStringList>::const_iterator iT = labelsAndTooltips.root();
		for (unsigned int i = 0; i < labelsAndTooltips.children(iT); ++i) {

			stlplus::ntree<QStringList>::const_iterator child = labelsAndTooltips.child(iT, i);
			QTreeWidgetItem* item = new QTreeWidgetItem(m_treeWidget);

			SetItem(child, item);

			m_treeWidget->addTopLevelItem(item);

			if (child->size() == 1) {

				SetItems(labelsAndTooltips, child, item);
			}
		}
	}

	void TitleTreeWidget::SetItems(const stlplus::ntree<QStringList>& labelsAndTooltips, stlplus::ntree<QStringList>::const_iterator iT, QTreeWidgetItem* parentItem) {

		for (unsigned int i = 0; i < labelsAndTooltips.children(iT); ++i) {

			stlplus::ntree<QStringList>::const_iterator child = labelsAndTooltips.child(iT, i);
			QTreeWidgetItem* item = new QTreeWidgetItem(parentItem);
			SetItem(child, item);

			parentItem->addChild(item);

			if (child->size() == 1) {

				SetItems(labelsAndTooltips, child, item);
			}
		}
	}

	void TitleTreeWidget::OnSelectionChanged() {

		QList<QTreeWidgetItem*> selection = m_treeWidget->selectedItems();
		if (!selection.isEmpty()) {

			emit SelectedTooltipChanged(selection[0]->data(0, Qt::ToolTipRole).toString());
		}
	}

	void TitleTreeWidget::SelectFirstSelectableItem() {

		m_treeWidget->setCurrentItem(m_firstSelectableItem);
	}

} //namespace SynGlyphX