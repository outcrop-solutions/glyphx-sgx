#include "datasourcesview.h"
#include "roledatafilterproxymodel.h"
#include "datasourceinfowidget.h"
#include "singlewidgetdialog.h"
#include <QtWidgets/QMessageBox>
#include "datatransformmodel.h"
#include "QueryBuilderDialog.h"

DataSourcesView::DataSourcesView(SynGlyphX::DataTransformModel* sourceModel, QWidget *parent)
	: DataMappingListView(parent),
	m_sourceModel(sourceModel)
{
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

	SynGlyphX::IntRoleDataFilterProxyModel* filterModel = new SynGlyphX::IntRoleDataFilterProxyModel(this);
	filterModel->setFilterRole(SynGlyphX::DataTransformModel::DataTypeRole);
	filterModel->setSourceModel(m_sourceModel);
	filterModel->SetFilterData(SynGlyphX::DataTransformModel::DataType::DataSources);
	setModel(filterModel);

	setDragEnabled(false);
	setAcceptDrops(false);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

	QAction* removeAction = m_sharedActions.AddAction(tr("Remove"));
	QObject::connect(removeAction, &QAction::triggered, this, &DataSourcesView::OnRemoveDatasource);

	m_sharedActions.AddSeparator();

	QAction* propertiesAction = m_sharedActions.AddAction(tr("Properties"));
	QObject::connect(propertiesAction, &QAction::triggered, this, &DataSourcesView::OnShowProperties);

	QAction* queryAction = m_sharedActions.AddAction(tr("Query"));
	QObject::connect(queryAction, &QAction::triggered, this, [=]() { QueryBuilderDialog dialog(this); dialog.exec(); });

	m_sharedActions.EnableActions(false);

	addActions(m_sharedActions.getActions());
}

DataSourcesView::~DataSourcesView()
{

}

const SynGlyphX::SharedActionList& DataSourcesView::GetSharedActions() {

	return m_sharedActions;
}

void DataSourcesView::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {

	m_sharedActions.EnableActions(!selected.isEmpty());

	QListView::selectionChanged(selected, deselected);
}

void DataSourcesView::OnShowProperties() {

	const QModelIndexList& selection = selectionModel()->selectedIndexes();
	if (!selection.isEmpty()) {

		ShowEditDialog(selection.front());
	}
}

void DataSourcesView::OnRemoveDatasource() {

	const QModelIndexList& selection = selectionModel()->selectedIndexes();
	if (!selection.isEmpty()) {

		if (QMessageBox::warning(this, tr("Remove Datasource"),
			tr("Removing this datasource will also remove any links, bindings in glyphs and field groups that use this datasource." 
			" This action cannot be reverted with 'Undo'. Do you wish to remove this datasource?"), 
			QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {

			model()->removeRow(selection.front().row());
		}
	}
}

QSize DataSourcesView::sizeHint() const {

	return QSize(250, 100);
}

void DataSourcesView::ShowEditDialog(const QModelIndex &index) {

	SynGlyphX::DataTransformMapping::DatasourceMap::const_iterator datasource = m_sourceModel->GetDataMapping()->GetDatasources().begin();
	std::advance(datasource, index.row());

	SynGlyphX::DatasourceInfoWidget* datasourceInfoWidget = new SynGlyphX::DatasourceInfoWidget(*datasource->second, this);
	SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::StandardButton::Ok, datasourceInfoWidget, this);
	dialog.setWindowTitle(tr("Datasource Info"));
	dialog.exec();
}