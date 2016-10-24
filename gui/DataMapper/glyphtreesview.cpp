#include "glyphtreesview.h"
#include "visualglyphpropertieswidget.h"
#include "singlewidgetdialog.h"
#include "roledatafilterproxymodel.h"
#include <QtWidgets/QFileDialog>
#include <QtCore/QSettings>
#include "datamappingglyphfile.h"
#include "antzcsvwriter.h"
#include "minmaxglyphtreemodel.h"
#include "glyphoptionswidget.h"
#include "datatransformmodel.h"
#include "datatransformmapping.h"
#include "DMGlobal.h"

using namespace SynGlyphX;
class GlyphTreesViewMemento {
public:
	typedef  QPair<boost::uuids::uuid, unsigned long> TreeNode;
	GlyphTreesViewMemento(const GlyphTreesView* tv) :
		m_selection(nullptr)
	{
		auto trees = tv->m_sourceModel->GetDataMapping()->GetGlyphGraphs();
		// create deep copy of glyphTrees
		for (auto glyphTree : trees) {
			m_glyphTrees.insert(std::pair<boost::uuids::uuid, DataMappingGlyphGraph::SharedPtr>(glyphTree.first, std::make_shared<SynGlyphX::DataMappingGlyphGraph>(*glyphTree.second)));
		}
		//m_glyphGraph //= std::make_shared<SynGlyphX::DataMappingGlyphGraph>(*tv->m_sourceModel->GetGlyphGraph());
		for (int row = 0; row < tv->m_sourceModel->rowCount(); ++row) {
			SaveExpandedOnLevel(tv, tv->m_sourceModel->index(row, 0));
		}
		m_selection = DMGlobal::Services()->CreateTreeSelection();
	}
	void SaveExpandedOnLevel(const GlyphTreesView* tv, const QModelIndex& index) {
		//OutputDebugStringA(index.data(Qt::DisplayRole).toString().toStdString().c_str());
		//OutputDebugStringA("\n");
		if (tv->isExpanded(dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(tv->model())->mapFromSource(index))) {
			if (index.isValid()){

				SynGlyphX::DataMappingGlyphGraph::Node* treeNode = static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer());
				SynGlyphX::DataMappingGlyphGraph::GlyphIterator glyph(treeNode);
				//SynGlyphX::LinkNode node(m_dataTransformModel->GetTreeId(sourceIndex), fromGlyph->first, lineEdit->GetInputField().GetHashID());
				m_list << TreeNode(tv->m_sourceModel->GetTreeId(index), glyph->first);

				//OutputDebugStringA(QString::number(glyph->first).toStdString().c_str());
				//OutputDebugStringA("\n");
			}

			//for (int row = 0; row < tv->m_sourceModel->rowCount(index); ++row)
			//	SaveExpandedOnLevel(tv, index.child(row, 0));
		}
		for (int row = 0; row < tv->m_sourceModel->rowCount(index); ++row)
			SaveExpandedOnLevel(tv, index.child(row, 0));
	}
	void RestoreExpandedOnLevel(GlyphTreesView* tv, const QModelIndex& index){

		SynGlyphX::DataMappingGlyphGraph::Node* treeNode = static_cast<SynGlyphX::DataMappingGlyphGraph::Node*>(index.internalPointer());
		if (treeNode){
			SynGlyphX::DataMappingGlyphGraph::GlyphIterator glyph(treeNode);
			if (m_list.contains(TreeNode(tv->m_sourceModel->GetTreeId(index), glyph->first))) {
				tv->setExpanded(dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(tv->model())->mapFromSource(index), true);
				for (int row = 0; row < tv->m_sourceModel->rowCount(index); ++row)
					RestoreExpandedOnLevel(tv, index.child(row, 0));
			}
		}
	}
	~GlyphTreesViewMemento() {
		//m_tree = nullptr;
		if (!m_selection)
			delete m_selection;
	}
	DataTransformMapping::DataMappingGlyphGraphMap m_glyphTrees;
	TreeSelection* m_selection;
	QList<TreeNode > m_list;

};

GlyphTreesView::GlyphTreesView(SynGlyphX::DataTransformModel* sourceModel, QWidget *parent)
	: SynGlyphX::TreeEditView(parent),
	m_sourceModel(sourceModel)
{
	SynGlyphX::IntRoleDataFilterProxyModel* filterModel = new SynGlyphX::IntRoleDataFilterProxyModel(this);
	filterModel->setFilterRole(SynGlyphX::DataTransformModel::DataTypeRole);
	filterModel->setSourceModel(m_sourceModel);
	filterModel->SetFilterData(SynGlyphX::DataTransformModel::DataType::GlyphTrees);
	setModel(filterModel);

	setDragEnabled(true);
	setAcceptDrops(false);
	setDropIndicatorShown(true);
	setDragDropMode(QAbstractItemView::InternalMove);
	setDefaultDropAction(Qt::MoveAction);
	SetScrollOnSelection(true);
	SetPreventMouseFromCausingUnselect(true);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setHeaderHidden(true);
	setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

	m_addChildrenAction = m_glyphActions.AddAction(tr("Add Children"));
	QObject::connect(m_addChildrenAction, &QAction::triggered, this, &GlyphTreesView::AddChildren);

	m_exportGlyphToFileAction = m_glyphActions.AddAction(tr("Export Glyph Template To File"));
	QObject::connect(m_exportGlyphToFileAction, &QAction::triggered, this, &GlyphTreesView::ExportGlyphToFile);

	m_glyphOptionsAction = m_glyphActions.AddAction(tr("Options"));
	QObject::connect(m_glyphOptionsAction, &QAction::triggered, this, &GlyphTreesView::ChangeOptions);

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

GlyphTreesViewMemento* GlyphTreesView::CreateMemento() const {

	return new GlyphTreesViewMemento(this);
}

void GlyphTreesView::ReinstateMemento(GlyphTreesViewMemento* m) {
	m_sourceModel->SetGlyphGraphMap(m->m_glyphTrees);
	setUpdatesEnabled(false);
	for (int row = 0; row < m_sourceModel->rowCount(); ++row)
		m->RestoreExpandedOnLevel(this, m_sourceModel->index(row, 0));
	setUpdatesEnabled(true);
	DMGlobal::Services()->ApplyTreeSelection(*m->m_selection);
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
	emit SelectionChangedSourceModel(filterModel->mapSelectionToSource(selectionModel()->selection()).indexes());
}

void GlyphTreesView::EnableActions(const QItemSelection& selection) {

	const QModelIndexList& selectedIndexes = selection.indexes();
	if (!selectedIndexes.isEmpty()) {

		bool areMultipleObjectsSelected = (selectedIndexes.count() > 1);
		bool doesClipboardHaveGlyph = DoesClipboardHaveGlyph();
		bool isSingleRootSelected = false;
		if (!areMultipleObjectsSelected) {

			isSingleRootSelected = !selectedIndexes.back().parent().isValid();
		}

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

		m_glyphOptionsAction->setEnabled(isSingleRootSelected);
	}
	else {

		m_glyphActions.EnableActions(false);
		m_editActions.EnableActions(false);
	}
}

void GlyphTreesView::AddChildren() {

	SynGlyphX::VisualGlyphPropertiesWidget* singleGlyphWidget = new SynGlyphX::VisualGlyphPropertiesWidget(true, SynGlyphX::VisualGlyphPropertiesWidget::ShowOnTop | SynGlyphX::VisualGlyphPropertiesWidget::EnabledSpinBox, this);
	singleGlyphWidget->SetWidgetFromGlyph(SynGlyphX::Glyph::s_defaultGlyph, true);

	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, singleGlyphWidget, this);
	dialog.setWindowTitle(tr("Add Children"));
	if (dialog.exec() == QDialog::Accepted) {
		DMGlobal::Services()->BeginTransaction("Add Children", SynGlyphX::TransactionType::ChangeTree);
		SynGlyphX::Glyph glyph;
		singleGlyphWidget->SetGlyphFromWidget(glyph);
		SynGlyphX::DataMappingGlyph minMaxGlyph(glyph);

		const QModelIndexList& selectedItems = selectionModel()->selectedIndexes();

		if (!selectedItems.isEmpty()) {

			SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(model());
			m_sourceModel->AddChildGlyph(filterModel->mapToSource(selectedItems.front()), minMaxGlyph, singleGlyphWidget->GetNumberOfChildren());
		}
		DMGlobal::Services()->EndTransaction();
	}
}

void GlyphTreesView::ExportGlyphToFile() {

	const QModelIndexList& selectedItems = selectionModel()->selectedIndexes();
	if (selectedItems.isEmpty()) {

		return;
	}

	QSettings settings;
	settings.beginGroup("ExportGlyphFile");
	QString glyphFilename = QFileDialog::getSaveFileName(this, tr("Export Glyph To File"), settings.value("LastDir", QDir::currentPath()).toString(), "SynGlyphX Glyph Template Files (*.sgt *.csv)");
	if (!glyphFilename.isEmpty()) {		

		SynGlyphX::RoleDataFilterProxyModel* filterModel = dynamic_cast<SynGlyphX::RoleDataFilterProxyModel*>(model());

		SynGlyphX::DataMappingGlyphGraph::SharedPtr glyphGraph = std::make_shared<SynGlyphX::DataMappingGlyphGraph>(m_sourceModel->GetSubgraph(filterModel->mapToSource(selectedItems.front()), true));
		glyphGraph->ClearAllInputBindings();

		if (glyphFilename.endsWith(".sgt")) {

			SynGlyphX::DataMappingGlyphFile glyphFile(glyphGraph);
			glyphFile.WriteToFile(glyphFilename.toStdString());
		}
		else {

			bool writeMaxGlyph = true;
			SynGlyphX::GlyphGraph::ConstSharedVector trees;
			if (writeMaxGlyph) {

				trees.push_back(glyphGraph->GetMaxGlyphTree());
			}
			else {

				trees.push_back(glyphGraph->GetMinGlyphTree());
			}

			SynGlyphXANTz::ANTzCSVWriter& csvWriter = SynGlyphXANTz::ANTzCSVWriter::GetInstance();
			csvWriter.Write({ { glyphFilename.toStdString() } }, trees, std::vector<SynGlyphXANTz::ANTzGrid>(), SynGlyphXANTz::ANTzCSVWriter::OutputPlatform::Windows);
		}

		settings.setValue("LastDir", glyphFilename);

		emit UpdateStatusBar("Glyph successfully saved to a file", 3000);
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

void GlyphTreesView::ChangeOptions() {

	const QModelIndexList& selectedItems = selectionModel()->selectedIndexes();
	QModelIndex index = selectedItems.back();

	GlyphOptionsWidget* glyphOptionsWidget = new GlyphOptionsWidget(this);
	glyphOptionsWidget->SetMergeRoots(model()->data(index, SynGlyphX::DataTransformModel::OptionsRole).toBool());

	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, glyphOptionsWidget, this);
	dialog.setWindowTitle(tr("Glyph Options"));
	if (dialog.exec() == QDialog::Accepted) {

		model()->setData(index, glyphOptionsWidget->GetMergeRoots(), SynGlyphX::DataTransformModel::OptionsRole);
	}
}

QSize GlyphTreesView::sizeHint() const {

	return QSize(250, 500);
}