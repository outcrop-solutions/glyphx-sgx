#include "legendlistview.h"
#include "legenddialog.h"
#include "datatransformmodel.h"

LegendListView::LegendListView(SynGlyphX::DataTransformModel* dataTransformModel, QWidget *parent)
	: QListView(parent),
	m_dataTransformModel(dataTransformModel)
{
	setSelectionMode(QAbstractItemView::SingleSelection);
	setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

	m_removeLegendAction = m_sharedActions.AddAction(tr("Remove"));
	QObject::connect(m_removeLegendAction, &QAction::triggered, this, &LegendListView::RemoveLegend);

	m_sharedActions.AddSeparator();

	m_propertiesAction = m_sharedActions.AddAction(tr("Properties"));
	QObject::connect(m_propertiesAction, &QAction::triggered, this, &LegendListView::ShowLegendProperties);

	m_sharedActions.EnableActions(false);
	addActions(m_sharedActions);
}

LegendListView::~LegendListView()
{

}

const SynGlyphX::SharedActionList& LegendListView::GetSharedActions() {

	return m_sharedActions;
}

void LegendListView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	QListView::selectionChanged(selected, deselected);
	EnableActions();
}

void LegendListView::EnableActions() {

	const QModelIndexList& selected = selectionModel()->selectedIndexes();
	m_sharedActions.EnableActions(!selected.isEmpty());
}

void LegendListView::RemoveLegend() {

	const QModelIndexList& selected = selectionModel()->selectedIndexes();
	if (!selected.isEmpty()) {

		model()->removeRow(selected.front().row());
	}
}

void LegendListView::ShowLegendProperties() {

	const QModelIndexList& selected = selectionModel()->selectedIndexes();
	if (!selected.isEmpty()) {

		unsigned int row = selected.front().row();
		SynGlyphX::LegendDialog dialog(this);
		dialog.setWindowTitle(tr("Legend Properties"));
		dialog.SetLegend(m_dataTransformModel->GetDataMapping()->GetLegends()[row]);
		if (dialog.exec() == QDialog::Accepted) {

			const SynGlyphX::Legend& legend = dialog.GetLegend();
			m_dataTransformModel->SetLegend(row, legend);
		}
	}
}