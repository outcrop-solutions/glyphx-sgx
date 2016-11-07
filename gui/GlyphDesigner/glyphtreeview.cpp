#include "glyphtreeview.h"
#include "singlewidgetdialog.h"
#include "visualglyphpropertieswidget.h"
#include "GDGlobal.h"
#include "GlyphTreeViewMementoBase.h"

class GlyphTreeViewMementoImpl : public GlyphTreeViewMemento {
public:
	GlyphTreeViewMementoImpl(const GlyphTreeView* tv) {
		m_tree = std::make_shared<SynGlyphX::DataMappingGlyphGraph>(*tv->m_model->GetMinMaxGlyphTree());
		for (int row = 0; row < tv->m_model->rowCount(); ++row)
			SaveExpandedOnLevel(tv, tv->m_model->index(row, 0));
			//if (tv->isExpanded(index)) {
			//	m_list << index.data(Qt::UserRole).toUInt();
			//}
		//}	
	}
	void SaveExpandedOnLevel(const GlyphTreeView* tv, const QModelIndex& index) {
		if (tv->isExpanded(index)) {
			if (index.isValid())
				m_list << index.data(Qt::UserRole).toUInt();
			for (int row = 0; row < tv->m_model->rowCount(index); ++row)
				SaveExpandedOnLevel(tv, index.child(row, 0));
		}
	}
	void RestoreExpandedOnLevel(GlyphTreeView* tv, const QModelIndex& index){
		if (m_list.contains(index.data(Qt::UserRole).toUInt())) {
			tv->setExpanded(index, true);
			for (int row = 0; row < tv->m_model->rowCount(index); ++row)
				RestoreExpandedOnLevel(tv, index.child(row, 0));
		}
	}
	virtual ~GlyphTreeViewMementoImpl() {
		//m_tree = nullptr;
	}
	SynGlyphX::DataMappingGlyphGraph::SharedPtr m_tree;	
	
	QList<unsigned long > m_list;

};

GlyphTreeView::GlyphTreeView(SynGlyphXANTz::MinMaxGlyphTreeModel* model, SynGlyphXANTz::MinMaxGlyphTreeModel::GlyphType glyphTreeType, QWidget *parent)
	: SynGlyphX::TreeEditView(parent),
	m_model(model),
	m_glyphTreeType(glyphTreeType)
{
	setModel(m_model);

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    setAcceptDrops(false);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::MoveAction);
	SetScrollOnSelection(true);
	SetPreventMouseFromCausingUnselect(true);

	CreateContextMenuActions();
}

GlyphTreeView::~GlyphTreeView()
{

}

GlyphTreeViewMemento* GlyphTreeView::CreateMemento() const {

	return new GlyphTreeViewMementoImpl(this);
}

void GlyphTreeView::ReinstateMemento(GlyphTreeViewMemento* memento) {
	GlyphTreeViewMementoImpl* m = dynamic_cast<GlyphTreeViewMementoImpl*>(memento);
	Q_ASSERT(m);
	m_model->SetMinMaxGlyphTree(m->m_tree);
	setUpdatesEnabled(false);
	for (int row = 0; row < m_model->rowCount(); ++row)
		m->RestoreExpandedOnLevel(this, m_model->index(row, 0));
	setUpdatesEnabled(true);
}

const SynGlyphX::SharedActionList& GlyphTreeView::GetGlyphActions() const {

	return m_glyphActions;
}

void GlyphTreeView::CreateContextMenuActions() {

	//Edit Actions

	m_editActions.AddSeparator();

	m_propertiesAction = m_editActions.AddAction(tr("Properties"));
	QObject::connect(m_propertiesAction, &QAction::triggered, this, &GlyphTreeView::PropertiesActivated);

	//Glyph Actions

	m_addChildrenAction = m_glyphActions.AddAction(tr("Add Children"));
	QObject::connect(m_addChildrenAction, &QAction::triggered, this, &GlyphTreeView::AddChildren);

	addAction(SynGlyphX::SharedActionList::CreateSeparator(this));
	addActions(m_glyphActions.getActions());
	addAction(SynGlyphX::SharedActionList::CreateSeparator(this));
	addActions(m_editActions.getActions());
	
	setContextMenuPolicy(Qt::ActionsContextMenu);
}

void GlyphTreeView::PropertiesActivated() {

	const QModelIndexList& selectedItems = selectionModel()->selectedIndexes();

	const QModelIndex& index = selectedItems.back();
	SynGlyphX::Glyph oldGlyph;
	if (m_glyphTreeType == SynGlyphXANTz::MinMaxGlyphTreeModel::GlyphType::Max) {

		oldGlyph = m_model->GetMinMaxGlyph(index)->second.GetMaxGlyph();
	}
	else {

		oldGlyph = m_model->GetMinMaxGlyph(index)->second.GetMinGlyph();
	}

	SynGlyphX::VisualGlyphPropertiesWidget* singleGlyphWidget = new SynGlyphX::VisualGlyphPropertiesWidget(true, SynGlyphX::VisualGlyphPropertiesWidget::ShowOnBottom, this);
	singleGlyphWidget->SetWidgetFromGlyph(oldGlyph, index.parent().isValid());
	singleGlyphWidget->SetNumberOfChildren(m_model->rowCount(index));

	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, singleGlyphWidget, this);
	dialog.setWindowTitle(tr("Glyph Properties"));
	if (dialog.exec() == QDialog::Accepted) {

		SynGlyphX::Glyph newGlyph;
		singleGlyphWidget->SetGlyphFromWidget(newGlyph);
		m_model->UpdateGlyphs(selectedItems, m_glyphTreeType, newGlyph);
	}
}

void GlyphTreeView::AddChildren() {
	const QModelIndexList& selectedItems = selectionModel()->selectedIndexes();

	SynGlyphX::VisualGlyphPropertiesWidget* singleGlyphWidget = new SynGlyphX::VisualGlyphPropertiesWidget(true, SynGlyphX::VisualGlyphPropertiesWidget::ShowOnTop | SynGlyphX::VisualGlyphPropertiesWidget::EnabledSpinBox, this);
	singleGlyphWidget->SetWidgetFromGlyph(SynGlyphX::Glyph::s_defaultGlyph, true);

	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, singleGlyphWidget, this);
	dialog.setWindowTitle(tr("Add Children"));
	if (dialog.exec() == QDialog::Accepted) {
		GDGlobal::Services()->BeginTransaction("Add Children", SynGlyphX::TransactionType::ChangeTree);
		SynGlyphX::Glyph glyph;
		singleGlyphWidget->SetGlyphFromWidget(glyph);
		SynGlyphX::DataMappingGlyph minMaxGlyph(glyph);
		for (int i = 0; i < selectedItems.length(); ++i) {

			m_model->AppendChild(selectedItems[i], minMaxGlyph, singleGlyphWidget->GetNumberOfChildren());
		}
		GDGlobal::Services()->EndTransaction();
	}
}

void GlyphTreeView::EnableActions(const QItemSelection& selected) {

	const QModelIndexList& selectedIndexes = selected.indexes();
	bool isObjectSelected = !selectedIndexes.isEmpty();

	bool hasChildren = false;
	for (int j = 0; j < selectedIndexes.count(); ++j) {
		
		if (m_model->hasChildren(selectedIndexes[j])) {
			
			hasChildren = true;
			break;
		}
	}

	bool isRootObjectSelected = false;
	for (int j = 0; j < selectedIndexes.count(); ++j) {

		if (!selectedIndexes[j].parent().isValid()) {

			isRootObjectSelected = true;
			break;
		}
	}

	m_glyphActions.EnableActions(isObjectSelected);

	bool areMultipleObjectsSelected = (selected.count() > 1);

	if (isObjectSelected) {

		bool isRootObjectOnlySelected = isRootObjectSelected && !areMultipleObjectsSelected;
		bool doesClipboardHaveGlyph = DoesClipboardHaveGlyph();

		m_cutAction->setEnabled(!isRootObjectSelected && !areMultipleObjectsSelected);
		m_copyAction->setEnabled(!areMultipleObjectsSelected);
		m_copyWithChildrenAction->setEnabled(!areMultipleObjectsSelected);
		m_pasteAction->setEnabled(!areMultipleObjectsSelected && doesClipboardHaveGlyph);
		m_pasteAsChildAction->setEnabled(!areMultipleObjectsSelected && doesClipboardHaveGlyph);

		m_deleteAction->setEnabled(!isRootObjectOnlySelected);
		m_deleteChildrenAction->setEnabled(hasChildren);
		m_propertiesAction->setEnabled(true);
	}
	else {
		
		m_editActions.EnableActions(false);
	}
}

SynGlyphX::DataMappingGlyphGraph GlyphTreeView::GetGraphForCopyToClipboard(const QModelIndex& index, bool includeChildren) {

	return m_model->GetSubgraph(index, includeChildren);
}

void GlyphTreeView::OverwriteGlyph(const QModelIndex& index, const SynGlyphX::DataMappingGlyphGraph& graph) {

	m_model->OverwriteGlyph(index, graph);
}

void GlyphTreeView::AddGlyphsAsChildren(const QModelIndex& index, const SynGlyphX::DataMappingGlyphGraph& graph) {

	m_model->AppendChildGraph(index, graph);
}

bool GlyphTreeView::DoInputBindingsNeedToBeClearedBeforePaste() {

	return true;
}

QModelIndex GlyphTreeView::GetNewSelectedIndexAfterDelete() const {

	QModelIndexList selectedItems = selectionModel()->selectedIndexes();

	if (selectedItems.isEmpty()) {

		return QModelIndex();
	}

	QModelIndex& lowestBranchLevelIndex = selectedItems[0];
	unsigned int minBranchLevel = m_model->GetBranchLevel(lowestBranchLevelIndex);
	for (int i = 1; i < selectedItems.length(); ++i) {

		unsigned int branchLevel = m_model->GetBranchLevel(selectedItems[i]);
		if (branchLevel < minBranchLevel) {

			minBranchLevel = branchLevel;
			lowestBranchLevelIndex = selectedItems[i];
		}
	}

	return lowestBranchLevelIndex.parent();
}

bool GlyphTreeView::CanIndexBeDeleted(const QModelIndex& index) const {

	return index.parent().isValid();
}
