#include "LinksListView.h"
#include "datatransformmodel.h"

LinksListView::LinksListView(SynGlyphX::DataTransformModel* dataTransformModel, QWidget *parent)
	: QListView(parent),
	m_dataTransformModel(dataTransformModel)
{
	setSelectionMode(QAbstractItemView::SingleSelection);
	setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

	m_removeLinkAction = m_sharedActions.AddAction(tr("Remove"));
	QObject::connect(m_removeLinkAction, &QAction::triggered, this, &LinksListView::RemoveLink);

	m_sharedActions.AddSeparator();

	m_propertiesAction = m_sharedActions.AddAction(tr("Edit"));
	QObject::connect(m_propertiesAction, &QAction::triggered, this, &LinksListView::EditLink);

	m_sharedActions.EnableActions(false);
	addActions(m_sharedActions);
}

LinksListView::~LinksListView()
{

}

const SynGlyphX::SharedActionList& LinksListView::GetSharedActions() {

	return m_sharedActions;
}

void LinksListView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	QListView::selectionChanged(selected, deselected);
	EnableActions();
}

void LinksListView::EnableActions() {

	const QModelIndexList& selected = selectionModel()->selectedIndexes();
	if (!selected.isEmpty()) {

		m_removeLinkAction->setEnabled(true); 
		m_propertiesAction->setEnabled(true);
	}
	else {

		m_sharedActions.EnableActions(false);
	}
}  

void LinksListView::RemoveLink() {

	const QModelIndexList& selected = selectionModel()->selectedIndexes();
	if (!selected.isEmpty()) {

		model()->removeRow(selected.front().row());
	}
}

void LinksListView::EditLink() {

	const QModelIndexList& selected = selectionModel()->selectedIndexes();
	if (!selected.isEmpty()) {

		unsigned int row = selected.front().row();
		emit editLink(row);

	}
}
