#include "sourcedatawidget.h"
#include <QtGui/QCloseEvent>

SourceDataWidget::SourceDataWidget(SynGlyphX::SourceDataCache::SharedPtr sourceDataCache, GlyphForestModel* model, QItemSelectionModel* selectionModel, QWidget *parent)
	: QTabWidget(parent),
	m_model(model),
	m_sourceDataCache(sourceDataCache)
{
	setWindowTitle(tr("Source Data Of Selected Glyphs"));
	QObject::connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &SourceDataWidget::OnSelectionChanged);
	UpdateTables(selectionModel->selection());
}

SourceDataWidget::~SourceDataWidget()
{

}

void SourceDataWidget::closeEvent(QCloseEvent* event) {

	if (parentWidget() == nullptr) {

		setVisible(false);
		event->ignore();
		emit WindowHidden();
	}
}

void SourceDataWidget::OnSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	UpdateTables(selected);
}

void SourceDataWidget::UpdateTables(const QItemSelection& selected) {


}