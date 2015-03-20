#include "sourcedataselectionwidget.h"
#include <QtWidgets/QVBoxLayout>

SourceDataSelectionWidget::SourceDataSelectionWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, GlyphForestModel* model, QItemSelectionModel* selectionModel, QWidget *parent)
	: QWidget(parent),
	m_selectionModel(selectionModel)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addStretch(1);

	m_sourceWidgetButton = new QPushButton(tr("Show Selected Source Data"), this);
	m_sourceWidgetButton->setCheckable(true);
	m_sourceWidgetButton->setEnabled(false);
	layout->addWidget(m_sourceWidgetButton);

	setLayout(layout);

	m_sourceWidgetButton->setEnabled(!m_selectionModel->selection().empty());
	QObject::connect(m_selectionModel, &QItemSelectionModel::selectionChanged, this, &SourceDataSelectionWidget::OnSelectionChanged);

	m_sourceDataWindow.reset(new SourceDataWidget(sourceDataCache, model, m_selectionModel));
	QObject::connect(m_sourceWidgetButton, &QPushButton::toggled, m_sourceDataWindow.data(), &SourceDataWidget::setVisible);
	m_sourceDataWindow->setVisible(false);

	QObject::connect(m_sourceDataWindow.data(), &SourceDataWidget::WindowHidden, this, &SourceDataSelectionWidget::OnSourceWidgetWindowHidden);
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