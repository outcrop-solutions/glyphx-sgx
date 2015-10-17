#include "glyphtreesview.h"
#include "glyphpropertieswidget.h"
#include "singlewidgetdialog.h"
#include "roledatafilterproxymodel.h"
#include <QtWidgets/QFileDialog>
#include <QtCore/QSettings>
#include "datamappingglyphfile.h"

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

	m_exportGlyphToFileAction = m_glyphActions.AddAction(tr("Export To File"));
	QObject::connect(m_exportGlyphToFileAction, &QAction::triggered, this, &GlyphTreesView::ExportGlyphToFile);

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
		m_exportGlyphToFileAction->setEnabled(true);
	}
	else {

		m_glyphActions.EnableActions(false);
		m_editActions.EnableActions(false);
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

void GlyphTreesView::ExportGlyphToFile() {

	QSettings settings;
	settings.beginGroup("ExportGlyphFile");
	QString glyphFilename = QFileDialog::getSaveFileName(this, tr("Export Glyph To File"), settings.value("LastDir", QDir::currentPath()).toString(), "SynGlyphX Glyph Files (*.sgt)");
	if (!glyphFilename.isEmpty()) {

		const QModelIndexList& selectedItems = selectionModel()->selectedIndexes();

		if (!selectedItems.isEmpty()) {

			SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(model());

			SynGlyphX::DataMappingGlyphGraph::SharedPtr glyphGraph = std::make_shared<SynGlyphX::DataMappingGlyphGraph>(m_sourceModel->GetSubgraph(filterModel->mapToSource(selectedItems.front()), true));
			glyphGraph->ClearAllInputBindings();

			SynGlyphX::DataMappingGlyphFile glyphFile(glyphGraph);
			glyphFile.WriteToFile(glyphFilename.toStdString());

			settings.setValue("LastDir", glyphFilename);
		}
	}

	settings.endGroup();
}

void GlyphTreesView::SelectLastGlyphTreeRoot() {

	selectionModel()->select(model()->index(m_sourceModel->GetDataMapping()->GetGlyphGraphs().size() - 1, 0), QItemSelectionModel::ClearAndSelect);
}

SynGlyphX::DataMappingGlyphGraph GlyphTreesView::GetGraphForCopyToClipboard(const QModelIndex& index, bool includeChildren) {

	SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(model());
	return m_sourceModel->GetSubgraph(filterModel->mapToSource(index), includeChildren);
}

void GlyphTreesView::OverwriteGlyph(const QModelIndex& index, const SynGlyphX::DataMappingGlyphGraph& graph) {

	SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(model());
	m_sourceModel->UpdateGlyph(filterModel->mapToSource(index), graph);
}

void GlyphTreesView::AddGlyphsAsChildren(const QModelIndex& index, const SynGlyphX::DataMappingGlyphGraph& graph) {

	SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(model());
	m_sourceModel->AddChildGlyphGraph(filterModel->mapToSource(index), graph);
}