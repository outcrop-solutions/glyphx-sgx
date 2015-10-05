#include "datasourcesview.h"
#include "roledatafilterproxymodel.h"
#include "datasourceinfowidget.h"
#include "singlewidgetdialog.h"

DataSourcesView::DataSourcesView(DataTransformModel* sourceModel, QWidget *parent)
	: QListView(parent),
	m_sourceModel(sourceModel)
{
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

	SynGlyphX::RoleDataFilterProxyModel* filterModel = new SynGlyphX::RoleDataFilterProxyModel(this);
	filterModel->setFilterRole(DataTransformModel::DataTypeRole);
	filterModel->setSourceModel(m_sourceModel);
	filterModel->SetFilterData(DataTransformModel::DataType::DataSources);
	setModel(filterModel);

	setDragEnabled(false);
	setAcceptDrops(false);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);

	m_sharedActions.AddSeparator();

	QAction* propertiesAction = m_sharedActions.AddAction(tr("Properties"));
	QObject::connect(propertiesAction, &QAction::triggered, this, &DataSourcesView::OnShowProperties);

	m_sharedActions.EnableActions(false);

	addActions(m_sharedActions);
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

		SynGlyphX::DatasourceMaps::FileDatasourceMap::const_iterator datasource = m_sourceModel->GetDataMapping()->GetDatasources().GetFileDatasources().begin();
		std::advance(datasource, selection.front().row());

		SynGlyphX::DatasourceInfoWidget* datasourceInfoWidget = new SynGlyphX::DatasourceInfoWidget(datasource->second, this);
		SynGlyphX::SingleWidgetDialog dialog(QDialogButtonBox::StandardButton::Ok, datasourceInfoWidget, this);
		dialog.setWindowTitle(tr("Datasource Info"));
		dialog.exec();
	}
}