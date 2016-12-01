#include "LinksListView.h"
#include "datatransformmodel.h"
#include <QtWidgets/QUndoStack>
#include <QtWidgets/QUndoCommand>
#include "Link.h"
#include "AppGlobal.h"

class RemoveLinkCommand : public QUndoCommand{
public:
	RemoveLinkCommand(SynGlyphX::DataTransformModel* dtm, int index) :
		m_index(index),
        m_dtm(dtm)
	{
		m_link = dtm->GetDataMapping()->GetLinks()[index];
	}
	void undo() override {
		m_dtm->InsertLink(m_index, m_link);
	}
	void redo() override {
		m_dtm->RemoveLink(m_index);
	}
	int m_index;
	SynGlyphX::DataTransformModel* m_dtm;
	SynGlyphX::Link m_link;
};

LinksListView::LinksListView(SynGlyphX::DataTransformModel* dataTransformModel, QWidget *parent)
	: DataMappingListView(parent),
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
	addActions(m_sharedActions.getActions());
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
		auto command = new RemoveLinkCommand(m_dataTransformModel, selected.front().row());
		command->setText(tr("Remove Link"));
		SynGlyphX::AppGlobal::Services()->GetUndoStack()->push(command);
		//model()->removeRow(selected.front().row());
	}
}

void LinksListView::EditLink() {

	const QModelIndexList& selected = selectionModel()->selectedIndexes();
	if (!selected.isEmpty()) {

		ShowEditDialog(selected.front());
	}
}

void LinksListView::ShowEditDialog(const QModelIndex &index) {

	unsigned int row = index.row();
	emit editLink(row);
}
