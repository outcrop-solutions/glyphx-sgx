#include "sourcedataselectionwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QAbstractItemView>
#include <QtWidgets/QScrollArea>
#include "groupboxsinglewidget.h"
#include "elasticlistwidget.h"

SourceDataSelectionWidget::SourceDataSelectionWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, GlyphForestModel* model, QItemSelectionModel* selectionModel, QWidget *parent)
	: QWidget(parent),
	m_selectionModel(selectionModel),
	m_sourceDataCache(sourceDataCache)
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	m_tableComboBox = new QComboBox(this);
	m_tableComboBox->setEnabled(false);
	m_tableComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	m_tableComboBox->setMinimumContentsLength(15);
	m_tableComboBox->blockSignals(true);

	SynGlyphX::GroupBoxSingleWidget* tableComboGroupBox = new SynGlyphX::GroupBoxSingleWidget(tr("Source Data Table:"), m_tableComboBox, this);
	layout->addWidget(tableComboGroupBox);

	m_elasticListsLayout = new QStackedLayout(layout);
	layout->addLayout(m_elasticListsLayout, 1);
	QObject::connect(m_tableComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), m_elasticListsLayout, &QStackedLayout::setCurrentIndex);

	m_sourceWidgetButton = new QPushButton(tr("Show Selected Source Data"), this);
	m_sourceWidgetButton->setCheckable(true);
	m_sourceWidgetButton->setEnabled(false);
	layout->addWidget(m_sourceWidgetButton);

	setLayout(layout);

	m_sourceWidgetButton->setEnabled(!m_selectionModel->selection().empty());
	QObject::connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &SourceDataSelectionWidget::OnSelectionChanged);

	m_sourceDataWindow.reset(new SourceDataWidget(m_sourceDataCache, model, m_selectionModel));
	QObject::connect(m_sourceWidgetButton, &QPushButton::toggled, m_sourceDataWindow.data(), &SourceDataWidget::setVisible);
	m_sourceDataWindow->setVisible(false);

	QObject::connect(m_sourceDataWindow.data(), &SourceDataWidget::WindowHidden, this, &SourceDataSelectionWidget::OnSourceWidgetWindowHidden);
	QObject::connect(model, &GlyphForestModel::modelReset, this, &SourceDataSelectionWidget::OnModelReset);
}

SourceDataSelectionWidget::~SourceDataSelectionWidget()
{

}

void SourceDataSelectionWidget::OnSourceWidgetWindowHidden() {

	m_sourceWidgetButton->setChecked(false);
}

void SourceDataSelectionWidget::OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	m_sourceWidgetButton->setEnabled(!m_selectionModel->selection().empty());
}

void SourceDataSelectionWidget::OnModelReset() {

	if (m_sourceDataCache->IsValid()) {

		const SynGlyphX::SourceDataCache::TableNameMap& tableNameMap = m_sourceDataCache->GetFormattedNames();
		for (auto formattedName : tableNameMap) {

			m_tableComboBox->addItem(formattedName.second);
			
			QScrollArea* scrollArea = new QScrollArea(this);
			QWidget* widget = new QWidget(scrollArea);
			QVBoxLayout* layout = new QVBoxLayout(widget);

			SynGlyphX::SourceDataCache::TableColumnSet columns = m_sourceDataCache->GetColumnsForTable(formattedName.first);
			for(const QString& column : columns) {

				SynGlyphX::ElasticListWidget* elasticListWidget = new SynGlyphX::ElasticListWidget(widget);
				elasticListWidget->SetTitle(column);
				layout->addWidget(elasticListWidget);
			}

			widget->setLayout(layout);

			scrollArea->setWidget(widget);

			m_elasticListsLayout->addWidget(scrollArea);
		}
		m_tableComboBox->view()->setMinimumWidth(m_tableComboBox->view()->sizeHintForColumn(0));
		m_tableComboBox->setEnabled(true);
		m_tableComboBox->blockSignals(false);
	}
	else {

		m_tableComboBox->blockSignals(true);
		m_tableComboBox->clear();
		m_tableComboBox->setEnabled(false);
		while (!m_elasticListsLayout->isEmpty()) {

			m_elasticListsLayout->removeWidget(m_elasticListsLayout->widget(0));
		}
	}
}