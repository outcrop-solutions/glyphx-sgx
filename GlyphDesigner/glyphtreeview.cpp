#include "glyphtreeview.h"

GlyphTreeView::GlyphTreeView(QWidget *parent)
    : QTreeView(parent)
{
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    setAcceptDrops(false);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::MoveAction);
}

GlyphTreeView::~GlyphTreeView()
{

}

const QList<QAction*>& GlyphTreeView::GetGlyphActions() const {

	return m_glyphActions;
}

const QList<QAction*>& GlyphTreeView::GetEditActions() const {

	return m_editActions;
}

void GlyphTreeView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

    QTreeView::selectionChanged(selected, deselected);
    const QModelIndexList& indicies = selected.indexes();
    if (indicies.length() > 0) {
        const QModelIndex& index = indicies.back();
        if (index.isValid()) {
            scrollTo(index);
        }
    }
}
/*
void GlyphTreeView::CreateContextMenuActions() {

	//Edit Actions

	m_cutAction = new QAction(tr("Cut"), this);
	m_cutAction->setShortcut(QKeySequence::Cut);
	//QObject::connect(m_cutAction, &QAction::triggered, m_model, &GlyphTreeModel::CutToClipboard);
	m_editActions.append(m_cutAction);

	m_copyAction = new QAction(tr("Copy"), this);
	m_copyAction->setShortcut(QKeySequence::Copy);
	//QObject::connect(m_copyAction, &QAction::triggered, m_model, &GlyphTreeModel::CopyToClipboard);
	m_editActions.append(m_copyAction);

	m_pasteAction = new QAction(tr("Paste"), this);
	m_pasteAction->setShortcut(QKeySequence::Paste);
	//QObject::connect(m_pasteAction, &QAction::triggered, m_model, &GlyphTreeModel::PasteFromClipboard);
	m_editActions.append(m_pasteAction);

	m_pasteAsChildAction = new QAction(tr("Paste As Child"), this);
	//QObject::connect(m_pasteAction, &QAction::triggered, m_model, &GlyphTreeModel::PasteChildFromClipboard);
	m_editActions.append(m_pasteAsChildAction);

	m_editActions.append(CreateSeparator());

	m_deleteAction = new QAction(tr("Delete"), this);
	m_deleteAction->setShortcut(QKeySequence::Delete);
	QObject::connect(m_deleteAction, &QAction::triggered, this, &GlyphTreeView::DeleteSelected);
	m_editActions.append(m_deleteAction);

	m_deleteChildrenAction = new QAction(tr("Delete Children"), this);
	QObject::connect(m_deleteChildrenAction, &QAction::triggered, this, &GlyphTreeView::DeleteChildrenFromSelected);
	m_editActions.append(m_deleteChildrenAction);

	m_editActions.append(CreateSeparator());

	m_propertiesAction = new QAction(tr("Properties"), this);
	QObject::connect(m_propertiesAction, &QAction::triggered, this, &GlyphTreeView::PropertiesActivated);
	m_editActions.append(m_propertiesAction);
}

void GlyphTreeView::DeleteSelected() {

	while (m_selectionModel->hasSelection()) {
		const QModelIndexList& selectedItems = m_selectionModel->selectedIndexes();
		m_model->removeRow(selectedItems.back().row(), selectedItems.back().parent());
	}
}

void GlyphTreeView::DeleteChildrenFromSelected() {

	//const QModelIndexList& selectedItems = m_selectionModel->selectedIndexes();
	QModelIndexList selectedItems = m_selectionModel->selectedIndexes();
	std::sort(selectedItems.begin(), selectedItems.end(), GlyphTreeModel::GreaterBranchLevel);
	for (int i = 0; i < selectedItems.length(); ++i) {
		m_model->removeRows(0, m_model->rowCount(selectedItems[i]), selectedItems[i]);
	}
}
*/