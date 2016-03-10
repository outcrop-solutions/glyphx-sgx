#include "rangefilterlistwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>

RangeFilterListWidget::RangeFilterListWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, SourceDataSelectionModel* selectionModel, QWidget *parent)
	: QWidget(parent),
	m_selectionModel(selectionModel),
	m_sourceDataCache(sourceDataCache)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QHBoxLayout* buttonLayout = new QHBoxLayout(this);
	m_addButton = new QPushButton(tr("Add"), this);
	QObject::connect(m_addButton, &QPushButton::clicked, this, &RangeFilterListWidget::OnAddFilter);
	buttonLayout->addWidget(m_addButton);
	m_addButton->setEnabled(false);
	
	m_removeAllButton = new QPushButton(tr("RemoveAll"), this);
	QObject::connect(m_removeAllButton, &QPushButton::clicked, this, &RangeFilterListWidget::OnRemoveAllFilters);
	buttonLayout->addWidget(m_removeAllButton);
	m_removeAllButton->setEnabled(false);

	buttonLayout->addStretch(1);
	mainLayout->addLayout(buttonLayout);

	mainLayout->addStretch(1);

	setLayout(mainLayout);

	QObject::connect(m_selectionModel->GetSceneSelectionModel()->model(), &QAbstractItemModel::modelReset, this, &RangeFilterListWidget::OnModelReset);
}

RangeFilterListWidget::~RangeFilterListWidget()
{

}

void RangeFilterListWidget::SwitchTable(const QString& table) {


}

void RangeFilterListWidget::OnModelReset() {

	m_addButton->setEnabled(m_sourceDataCache->IsValid());
	m_removeAllButton->setEnabled(false);

	if (!m_sourceDataCache->IsValid()) {

		OnRemoveAllFilters();
	}
}

void RangeFilterListWidget::OnAddFilter() {

	SynGlyphX::SingleNumericRangeFilterWidget* filter = new SynGlyphX::SingleNumericRangeFilterWidget(Qt::Horizontal, this);
	QVBoxLayout* mainLayout = dynamic_cast<QVBoxLayout*>(layout());
	mainLayout->insertWidget(m_numericFilters.size() + 1, filter);
	m_numericFilters.push_back(filter);
	m_removeAllButton->setEnabled(true);
}

void RangeFilterListWidget::OnRemoveAllFilters() {

	for (SynGlyphX::SingleNumericRangeFilterWidget* filter : m_numericFilters) {

		layout()->removeWidget(filter);
		delete filter;
	}
	m_numericFilters.clear();
	m_removeAllButton->setEnabled(false);
}
