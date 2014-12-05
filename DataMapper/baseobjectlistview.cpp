#include "baseobjectlistview.h"
#include "baseimagedialog.h"

BaseObjectListView::BaseObjectListView(DataTransformModel* dataTransformModel, QWidget *parent)
	: QListView(parent),
	m_dataTransformModel(dataTransformModel)
{
	setSelectionMode(QAbstractItemView::SingleSelection);
	setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

	m_removeBaseObjectAction = m_sharedActions.AddAction(tr("Remove"));
	QObject::connect(m_removeBaseObjectAction, &QAction::triggered, this, &BaseObjectListView::RemoveBaseObject);

	m_sharedActions.AddSeparator();

	m_propertiesAction = m_sharedActions.AddAction(tr("Properties"));
	QObject::connect(m_propertiesAction, &QAction::triggered, this, &BaseObjectListView::ShowBaseObjectProperties);

	m_sharedActions.EnableActions(false);
	addActions(m_sharedActions);
}

BaseObjectListView::~BaseObjectListView()
{

}

const SynGlyphX::SharedActionList& BaseObjectListView::GetSharedActions() {

	return m_sharedActions;
}

void BaseObjectListView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	QListView::selectionChanged(selected, deselected);
	EnableActions();
}

void BaseObjectListView::EnableActions() {

	const QModelIndexList& selected = selectionModel()->selectedIndexes();
	if (!selected.isEmpty()) {

		m_removeBaseObjectAction->setEnabled(selected.front().row() > 0);
		m_propertiesAction->setEnabled(true);
	}
	else {

		m_sharedActions.EnableActions(false);
	}
}

void BaseObjectListView::RemoveBaseObject() {

	const QModelIndexList& selected = selectionModel()->selectedIndexes();
	if (!selected.isEmpty()) {

		model()->removeRow(selected.front().row());
	}
}

void BaseObjectListView::ShowBaseObjectProperties() {

	const QModelIndexList& selected = selectionModel()->selectedIndexes();
	if (!selected.isEmpty()) {

		unsigned int row = selected.front().row();
		BaseImageDialog dialog(this);
		dialog.setWindowTitle(tr("Base Object Properties"));
		dialog.SetBaseImage(m_dataTransformModel->GetDataMapping()->GetBaseObjects()[row]);
		if (dialog.exec() == QDialog::Accepted) {

			const SynGlyphX::BaseImage& baseImage = dialog.GetBaseImage();
			m_dataTransformModel->SetBaseObject(row, baseImage);
		}
	}
}