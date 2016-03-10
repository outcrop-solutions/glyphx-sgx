#include "filteringwidget.h"
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTabWidget>
#include "multitableelasticlistswidget.h"

FilteringWidget::FilteringWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, SourceDataSelectionModel* selectionModel, QWidget *parent)
	: QWidget(parent),
	m_selectionModel(selectionModel),
	m_sourceDataCache(sourceDataCache)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QHBoxLayout* topLayout = new QHBoxLayout;

	m_hideUnselectedTreesCheckbox = new QCheckBox(tr("Filter View"), this);
	topLayout->addWidget(m_hideUnselectedTreesCheckbox);

	m_clearButton = new QPushButton(tr("Clear"), this);
	topLayout->addWidget(m_clearButton);
	QObject::connect(m_clearButton, &QPushButton::clicked, this, &FilteringWidget::Clear);

	topLayout->addStretch(1);
	mainLayout->addLayout(topLayout);

	QTabWidget* filterMethodsWidget = new QTabWidget(this);

	MultiTableElasticListsWidget* elasticListsWidget = new MultiTableElasticListsWidget(sourceDataCache, selectionModel, filterMethodsWidget);
	filterMethodsWidget->addTab(elasticListsWidget, tr("Elastic"));

	mainLayout->addWidget(filterMethodsWidget, 1);

	QHBoxLayout* buttonsLayout = new QHBoxLayout(this);

	m_sourceWidgetButton = new QPushButton(tr("Show Selected Source Data"), this);
	m_sourceWidgetButton->setCheckable(true);
	buttonsLayout->addWidget(m_sourceWidgetButton);

	buttonsLayout->addStretch(1);
	mainLayout->addLayout(buttonsLayout);

	setLayout(mainLayout);

	EnableButtons(!m_selectionModel->GetSourceDataSelection().empty());
	m_sourceDataWindow.reset(new SourceDataWidget(m_selectionModel));
	QObject::connect(m_sourceWidgetButton, &QPushButton::toggled, m_sourceDataWindow.data(), &SourceDataWidget::setVisible);
	m_sourceDataWindow->setVisible(false);

	QObject::connect(m_sourceDataWindow.data(), &SourceDataWidget::WindowHidden, this, &FilteringWidget::OnSourceWidgetWindowHidden);
	QObject::connect(m_selectionModel, &SourceDataSelectionModel::SelectionChanged, this, &FilteringWidget::OnSourceDataStateChanged);
	QObject::connect(m_selectionModel->GetSceneSelectionModel()->model(), &QAbstractItemModel::modelReset, this, &FilteringWidget::OnSourceDataStateChanged);
}

FilteringWidget::~FilteringWidget()
{

}

void FilteringWidget::SetupLinkedWidgets(LinkedWidgetsManager& linkedWidgets) {

	linkedWidgets.AddFilterViewCheckbox(m_hideUnselectedTreesCheckbox);
}

void FilteringWidget::Clear() {

	m_selectionModel->ClearSourceDataSelection();
}

void FilteringWidget::OnSourceWidgetWindowHidden() {

	m_sourceWidgetButton->setChecked(false);
}

void FilteringWidget::OnSourceDataStateChanged() {

	bool isSelectionEmpty = m_selectionModel->GetSourceDataSelection().empty();
	EnableButtons(m_sourceDataCache->IsValid() && (!isSelectionEmpty));

	if (isSelectionEmpty) {

		m_sourceDataWindow->setVisible(false);
		OnSourceWidgetWindowHidden();
	}
}

void FilteringWidget::EnableButtons(bool enable) {

	m_sourceWidgetButton->setEnabled(enable);
	m_clearButton->setEnabled(enable);
}