#include "glyphtreesview.h"
#include "glyphpropertieswidget.h"
#include "singlewidgetdialog.h"
#include "roledatafilterproxymodel.h"

GlyphTreesView::GlyphTreesView(DataTransformModel* sourceModel, QWidget *parent)
	: SynGlyphX::TreeEditView(parent),
	m_sourceModel(sourceModel)
{
	SynGlyphX::RoleDataFilterProxyModel* filterModel = new SynGlyphX::RoleDataFilterProxyModel(this);
	filterModel->setFilterRole(DataTransformModel::DataTypeRole);
	filterModel->setSourceModel(m_sourceModel);
	filterModel->SetFilterData(DataTransformModel::DataType::GlyphTrees);
	setModel(filterModel);

	setDragEnabled(true);
	setAcceptDrops(false);
	setDropIndicatorShown(true);
	setDragDropMode(QAbstractItemView::InternalMove);
	setDefaultDropAction(Qt::MoveAction);
	SetScrollOnSelection(true);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setHeaderHidden(true);
	setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

	m_addChildrenAction = m_glyphActions.AddAction(tr("Add Children"));
	QObject::connect(m_addChildrenAction, &QAction::triggered, this, &GlyphTreesView::AddChildren);

	m_glyphActions.AddSeparator();

	m_clearSelectedInputBindingsAction = m_glyphActions.AddAction(tr("Clear Input Bindings"));

	m_glyphActions.EnableActions(false);

	addAction(SynGlyphX::SharedActionList::CreateSeparator(this));
	addActions(m_glyphActions);
	addAction(SynGlyphX::SharedActionList::CreateSeparator(this));
	addActions(m_editActions);
}

GlyphTreesView::~GlyphTreesView()
{

}

const SynGlyphX::SharedActionList& GlyphTreesView::GetGlyphActions() {

	return m_glyphActions;
}

const QAction* const GlyphTreesView::GetClearSelectedInputBindingsAction() const {

	return m_clearSelectedInputBindingsAction;
}

void GlyphTreesView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	SynGlyphX::TreeEditView::selectionChanged(selected, deselected);
	
	SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(model());
	emit SelectionChangedSourceModel(filterModel->mapSelectionToSource(selected), filterModel->mapSelectionToSource(deselected));
}

void GlyphTreesView::EnableActions(const QItemSelection& selection) {

	const QModelIndexList& selectedIndexes = selection.indexes();
	if (!selectedIndexes.isEmpty()) {

		bool areMultipleObjectsSelected = (selectedIndexes.count() > 1);
		bool doesClipboardHaveGlyph = DoesClipboardHaveGlyph();

		m_cutAction->setEnabled(!areMultipleObjectsSelected);
		m_copyAction->setEnabled(!areMultipleObjectsSelected);
		m_copyWithChildrenAction->setEnabled(!areMultipleObjectsSelected);
		m_pasteAction->setEnabled(!areMultipleObjectsSelected && doesClipboardHaveGlyph);
		m_pasteAsChildAction->setEnabled(!areMultipleObjectsSelected && doesClipboardHaveGlyph);

		m_deleteAction->setEnabled(true);
		m_deleteChildrenAction->setEnabled(model()->rowCount(selectedIndexes.front()) > 0);

		m_addChildrenAction->setEnabled(true);
		m_clearSelectedInputBindingsAction->setEnabled(true);
	}
	else {

		m_glyphActions.EnableActions(false);
		m_editActions.EnableActions(false);
	}
}

void GlyphTreesView::DeleteSelected() {

	const QModelIndexList& selected = selectionModel()->selectedIndexes();
	if (!selected.isEmpty()) {

		const QModelIndex& index = selected.front();
		model()->removeRow(index.row(), index.parent());
	}
}

void GlyphTreesView::DeleteChildrenFromSelected() {

	const QModelIndexList& selected = selectionModel()->selectedIndexes();
	if (!selected.isEmpty()) {
		
		const QModelIndex& parent = selected.front();
		model()->removeRows(0, model()->rowCount(parent), parent);
	}
}

void GlyphTreesView::AddChildren() {

	SynGlyphX::GlyphPropertiesWidget* singleGlyphWidget = new SynGlyphX::GlyphPropertiesWidget(SynGlyphX::GlyphPropertiesWidget::ShowOnTop | SynGlyphX::GlyphPropertiesWidget::EnabledSpinBox, this);
	singleGlyphWidget->SetWidgetFromGlyph(SynGlyphX::Glyph::s_defaultGlyph, true);

	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, singleGlyphWidget, this);
	dialog.setWindowTitle(tr("Add Children"));
	if (dialog.exec() == QDialog::Accepted) {

		SynGlyphX::Glyph glyph;
		singleGlyphWidget->SetGlyphFromWidget(glyph);
		SynGlyphX::DataMappingGlyph minMaxGlyph(glyph);

		const QModelIndexList& selectedItems = selectionModel()->selectedIndexes();

		if (!selectedItems.isEmpty()) {

			SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(model());
			m_sourceModel->AddChildGlyph(filterModel->mapToSource(selectedItems.front()), minMaxGlyph, singleGlyphWidget->GetNumberOfChildren());
		}
	}
}

void GlyphTreesView::SelectLastGlyphTreeRoot() {

	selectionModel()->select(model()->index(m_sourceModel->GetDataMapping()->GetGlyphGraphs().size() - 1, 0), QItemSelectionModel::ClearAndSelect);
}

SynGlyphX::DataMappingGlyphGraph::LinklessGraph GlyphTreesView::GetGraphForCopyToClipboard(const QModelIndex& index) {

	SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(model());
	return m_sourceModel->GetSubgraph(filterModel->mapToSource(index));
}

SynGlyphX::DataMappingGlyph GlyphTreesView::GetGlyphForCopyToClipboard(const QModelIndex& index) {

	SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(model());
	return m_sourceModel->GetGlyph(filterModel->mapToSource(index));
}

void GlyphTreesView::OverwriteGlyph(const QModelIndex& index, const SynGlyphX::DataMappingGlyphGraph::LinklessGraph& graph) {

	SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(model());
	m_sourceModel->UpdateGlyph(filterModel->mapToSource(index), graph);
}

void GlyphTreesView::AddGlyphsAsChildren(const QModelIndex& index, const SynGlyphX::DataMappingGlyphGraph::LinklessGraph& graph) {

	SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(model());
	m_sourceModel->AddChildGlyphGraph(filterModel->mapToSource(index), graph);
}