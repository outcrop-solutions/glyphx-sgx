#include "LoadingFilterWidget.h"
#include <QtWidgets/QListView>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>

LoadingFilterWidget::LoadingFilterWidget(QWidget *parent)
	: QSplitter(Qt::Horizontal, parent)
{
	setChildrenCollapsible(false);
}

LoadingFilterWidget::~LoadingFilterWidget()
{

}

void LoadingFilterWidget::AddFilter(const QString& name, bool allowMultiselect, const QStringList& filterValues) {

	QWidget* filterWidget = new QWidget(this);
	QVBoxLayout* filterWidgetLayout = new QVBoxLayout(filterWidget);
	filterWidgetLayout->setSpacing(0);

	QWidget* titleWidget = new QWidget(filterWidget);
	QHBoxLayout* titleLayout = new QHBoxLayout(titleWidget);

	QLabel* titleLabel = new QLabel(name, titleWidget);
	titleLayout->addWidget(titleLabel);
	titleLayout->addStretch(1);

	QPushButton* selectAllButton = new QPushButton(tr("Select All"), titleWidget);
	selectAllButton->setEnabled(allowMultiselect);
	titleLayout->addWidget(selectAllButton);

	titleWidget->setLayout(titleLayout);
	filterWidgetLayout->addWidget(titleWidget);

	QFrame* hLine = new QFrame(filterWidget);
	hLine->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	hLine->setLineWidth(1);
	hLine->setMidLineWidth(2);
	filterWidgetLayout->addWidget(hLine);

	QListWidget* filterValueList = new QListWidget(filterWidget);
	filterValueList->setSortingEnabled(false);
	filterValueList->addItems(filterValues);
	if (allowMultiselect) {

		QObject::connect(selectAllButton, &QPushButton::clicked, filterValueList, &QListWidget::selectAll);
		filterValueList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	}
	else {

		filterValueList->setSelectionMode(QAbstractItemView::SingleSelection);
	}
	filterWidgetLayout->addWidget(filterValueList);

	filterWidget->setLayout(filterWidgetLayout);

	addWidget(filterWidget);
	m_filterListWidgets.append(filterValueList);
}

bool LoadingFilterWidget::DoAllFiltersHaveASelection() const {

	for (unsigned int i = 0; i < m_filterListWidgets.size(); ++i) {

		if (m_filterListWidgets[i]->selectedItems().isEmpty()) {

			return false;
		}
	}

	return true;
}